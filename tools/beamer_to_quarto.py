#!/usr/bin/env python3
"""Convert the Beamer sources in this repository to Quarto reveal.js decks.

The converter intentionally preserves unsupported LaTeX instead of silently
dropping it.  Run with --report to obtain machine-readable diagnostics.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import re
import shutil
import subprocess
import sys
import tempfile
from dataclasses import asdict, dataclass
from pathlib import Path


VERSION = "0.1.0"


@dataclass
class Diagnostic:
    severity: str
    code: str
    message: str
    path: str
    line: int = 1


def line_number(text: str, offset: int) -> int:
    return text.count("\n", 0, offset) + 1


def strip_comments(text: str) -> str:
    """Remove TeX comments, while retaining escaped percent signs."""
    return re.sub(r"(?<!\\)%[^\n]*", "", text)


def read_group(text: str, start: int, opening: str = "{", closing: str = "}") -> tuple[str, int]:
    """Read a balanced TeX group beginning at *start*."""
    if start >= len(text) or text[start] != opening:
        raise ValueError(f"expected {opening!r} at offset {start}")
    depth = 0
    escaped = False
    for pos in range(start, len(text)):
        char = text[pos]
        if escaped:
            escaped = False
            continue
        if char == "\\":
            escaped = True
        elif char == opening:
            depth += 1
        elif char == closing:
            depth -= 1
            if depth == 0:
                return text[start + 1 : pos], pos + 1
    raise ValueError(f"unterminated {opening}{closing} group")


def find_environment_end(text: str, name: str, content_start: int) -> tuple[int, int]:
    token = re.compile(rf"\\(begin|end)\{{{re.escape(name)}\}}")
    depth = 1
    for match in token.finditer(text, content_start):
        depth += 1 if match.group(1) == "begin" else -1
        if depth == 0:
            return match.start(), match.end()
    raise ValueError(f"unterminated {name} environment")


class Converter:
    def __init__(self, source_root: Path, output_root: Path, assets: str = "copy", write_assets: bool = True) -> None:
        self.source_root = source_root.resolve()
        self.output_root = output_root.resolve()
        self.assets_mode = assets
        self.write_assets = write_assets
        self.diagnostics: list[Diagnostic] = []
        self.asset_manifest: dict[str, dict[str, str]] = {}

    def diagnose(self, severity: str, code: str, message: str, path: Path, line: int = 1) -> None:
        try:
            display_path = path.resolve().relative_to(Path.cwd().resolve()).as_posix()
        except ValueError:
            display_path = path.as_posix()
        self.diagnostics.append(Diagnostic(severity, code, message, display_path, line))

    def resolve_input(self, value: str, master_dir: Path, referring: Path) -> Path | None:
        candidate = Path(value.strip())
        if not candidate.suffix:
            candidate = candidate.with_suffix(".tex")
        for base in (master_dir, referring.parent):
            resolved = (base / candidate).resolve()
            if resolved.is_file():
                return resolved
        return None

    def expand_inputs(self, path: Path, master_dir: Path, stack: tuple[Path, ...] = ()) -> str:
        path = path.resolve()
        if path in stack:
            self.diagnose("error", "input-cycle", "cyclic \\input detected", path)
            return f"\\textbf{{[cyclic input: {path.name}]}}"
        try:
            text = path.read_text(encoding="utf-8")
        except (OSError, UnicodeError) as exc:
            self.diagnose("error", "input-read", str(exc), path)
            return f"\\textbf{{[unreadable input: {path.name}]}}"

        pattern = re.compile(r"\\input\s*\{([^}]+)\}")
        chunks: list[str] = []
        cursor = 0
        for match in pattern.finditer(text):
            # Ignore commands on a commented-out line.
            line_start = text.rfind("\n", 0, match.start()) + 1
            if re.search(r"(?<!\\)%", text[line_start : match.start()]):
                continue
            chunks.append(text[cursor : match.start()])
            included = self.resolve_input(match.group(1), master_dir, path)
            if included is None:
                self.diagnose(
                    "error", "input-missing", f"input not found: {match.group(1)}", path,
                    line_number(text, match.start()),
                )
                chunks.append(f"\\textbf{{[missing input: {match.group(1)}]}}")
            else:
                chunks.append(self.expand_inputs(included, master_dir, stack + (path,)))
            cursor = match.end()
        chunks.append(text[cursor:])
        return "".join(chunks)

    @staticmethod
    def metadata(text: str) -> dict[str, str]:
        values: dict[str, str] = {}
        for key in ("title", "author", "date"):
            match = re.search(rf"\\{key}\s*\{{", text)
            if match:
                try:
                    values[key], _ = read_group(text, match.end() - 1)
                except ValueError:
                    pass
        return values

    def extract_structure(self, text: str, source: Path) -> list[tuple[str, str, str]]:
        """Return ordered (kind, title, body) section/frame nodes."""
        nodes: list[tuple[str, str, str]] = []
        event = re.compile(r"\\section\s*\{|\\begin\s*\{frame\}")
        cursor = 0
        while match := event.search(text, cursor):
            line_start = text.rfind("\n", 0, match.start()) + 1
            if re.search(r"(?<!\\)%", text[line_start:match.start()]):
                cursor = match.end()
                continue
            if match.group(0).startswith("\\section"):
                try:
                    title, cursor = read_group(text, match.end() - 1)
                    nodes.append(("section", self.inline(title), ""))
                except ValueError as exc:
                    self.diagnose("error", "section-parse", str(exc), source, line_number(text, match.start()))
                    cursor = match.end()
                continue

            pos = match.end()
            while pos < len(text) and text[pos].isspace():
                pos += 1
            if pos < len(text) and text[pos] == "[":
                try:
                    _, pos = read_group(text, pos, "[", "]")
                except ValueError:
                    pass
            while pos < len(text) and text[pos].isspace():
                pos += 1
            title = ""
            if pos < len(text) and text[pos] == "{":
                try:
                    title, pos = read_group(text, pos)
                except ValueError:
                    pass
            try:
                body_end, cursor = find_environment_end(text, "frame", pos)
            except ValueError as exc:
                self.diagnose("error", "frame-parse", str(exc), source, line_number(text, match.start()))
                break
            body = text[pos:body_end]
            if not title:
                title = "タイトル" if "\\titlepage" in body else "無題"
            nodes.append(("frame", self.inline(title), body))
        return nodes

    def convert_lists(self, text: str) -> str:
        pattern = re.compile(r"\\begin\{(itemize|enumerate)\}")
        while match := pattern.search(text):
            name = match.group(1)
            try:
                body_end, env_end = find_environment_end(text, name, match.end())
            except ValueError:
                break
            body = text[match.end() : body_end]
            item_events = re.compile(r"\\(begin|end)\{(?:itemize|enumerate)\}|\\item\b(?:\s*\[[^]]*\])?\s*")
            depth = 0
            item_starts = []
            for event in item_events.finditer(body):
                if event.group(1):
                    depth += 1 if event.group(1) == "begin" else -1
                elif depth == 0:
                    item_starts.append((event.start(), event.end()))
            items = [body[end : item_starts[index + 1][0] if index + 1 < len(item_starts) else len(body)]
                     for index, (_, end) in enumerate(item_starts)]
            marker = "1." if name == "enumerate" else "-"
            rendered = []
            for item in items:
                lines = self.convert_body(item.strip()).strip().splitlines()
                if not lines:
                    continue
                rendered.append(f"{marker} {lines[0]}")
                rendered.extend(f"   {line}" if line else "" for line in lines[1:])
            text = text[: match.start()] + "\n" + "\n".join(rendered) + "\n" + text[env_end:]
        return text

    @staticmethod
    def inline(text: str) -> str:
        text = text.strip()
        color_group = re.compile(r"(?<!\\)\{\s*\\color\s*\{(gray|red)\}\s*")
        cursor = 0
        while match := color_group.search(text, cursor):
            try:
                _, end = read_group(text, match.start())
            except ValueError:
                cursor = match.end()
                continue
            content = Converter.inline(text[match.end() : end - 1])
            replacement = f"[{content}]{{.{match.group(1)}}}"
            text = text[:match.start()] + replacement + text[end:]
            cursor = match.start() + len(replacement)
        text = re.sub(r"\\(?:textbf|bfseries)\s*\{([^{}]*)\}", r"**\1**", text)
        text = re.sub(r"\\(?:texttt|ttfamily)\s*\{([^{}]*)\}", r"`\1`", text)
        text = re.sub(r"\\emph\s*\{([^{}]*)\}", r"*\1*", text)
        text = re.sub(r"\\href\s*\{([^{}]*)\}\s*\{([^{}]*)\}", r"[\2](\1)", text)
        text = re.sub(r"\\url\s*\{([^{}]*)\}", r"<\1>", text)
        text = text.replace(r"\%", "%").replace("~", " ")
        return text

    def copy_image(self, raw_path: str) -> str:
        source = (self.source_root / raw_path).resolve()
        if not source.suffix:
            for suffix in (".pdf", ".png", ".jpg", ".jpeg", ".svg", ".eps"):
                if source.with_suffix(suffix).exists():
                    source = source.with_suffix(suffix)
                    break
        if not source.is_file():
            self.diagnose("error", "image-missing", f"image not found: {raw_path}", self.source_root)
            return raw_path
        try:
            source_relative = source.relative_to(self.source_root)
        except ValueError:
            source_relative = Path(source.name)
        relative = Path("assets") / source_relative
        convert_pdf = source.suffix.lower() == ".pdf"
        if convert_pdf:
            relative = relative.with_suffix(".svg")
        destination = self.output_root / relative
        if self.write_assets:
            destination.parent.mkdir(parents=True, exist_ok=True)
            if convert_pdf:
                executable = shutil.which("pdftocairo")
                if executable is None:
                    self.diagnose("error", "image-converter-missing", "PDF conversion requires pdftocairo (Poppler)", source)
                    return raw_path
                if destination.is_symlink():
                    destination.unlink()
                try:
                    subprocess.run(
                        [executable, "-svg", "-f", "1", "-l", "1", str(source), str(destination)],
                        check=True, capture_output=True, text=True,
                    )
                except subprocess.CalledProcessError as exc:
                    self.diagnose("error", "image-conversion", exc.stderr.strip(), source)
                    return raw_path
            elif self.assets_mode == "symlink":
                if destination.exists() or destination.is_symlink():
                    destination.unlink()
                destination.symlink_to(source)
            else:
                shutil.copy2(source, destination)
        digest = hashlib.sha256(source.read_bytes()).hexdigest()
        self.asset_manifest[relative.as_posix()] = {"source": source.as_posix(), "sha256": digest}
        if source.suffix.lower() == ".eps":
            self.diagnose("warning", "image-browser-format", f"copied {source.suffix} image; convert to SVG for browsers", source)
        return relative.as_posix()

    def convert_body(self, body: str) -> str:
        listings: list[str] = []
        protected: list[str] = []

        def protect(value: str) -> str:
            token = f"@@PROTECTED{len(protected)}@@"
            protected.append(value)
            return token

        def listing(match: re.Match[str]) -> str:
            environment = match.group("environment")
            content = match.group("content")
            option = ""
            if environment == "lstlisting" and content.startswith("["):
                option, end = read_group(content, 0, "[", "]")
                content = content[end:]
            language_match = re.search(r"language\s*=\s*([^,\]]+)", option)
            language = (language_match.group(1).strip().lower() if language_match else
                        "c" if environment == "lstlisting" else "")
            language = {"bash": "bash", "python": "python", "c++": "cpp"}.get(language, language)
            # Remove only the newlines separating the environment delimiters from code.
            content = re.sub(r"^[ \t]*\r?\n", "", content, count=1)
            content = re.sub(r"\r?\n[ \t]*$", "", content, count=1)
            fence = "`" * max(3, max((len(run) + 1 for run in re.findall(r"`+", content)), default=0))
            token = f"@@LISTING{len(listings)}@@"
            attributes = language if environment == "lstlisting" else "{.verbatim}"
            listings.append(f"{fence}{attributes}\n{content}\n{fence}")
            return "\n\n" + token + "\n\n"

        body = re.sub(
            r"\\begin\{(?P<environment>lstlisting|verbatim\*?)\}(?P<content>.*?)\\end\{(?P=environment)\}",
            listing, body, flags=re.DOTALL,
        )
        body = strip_comments(body)
        body = re.sub(r"\\(?:vspace|hspace)\*?\s*\{[^{}]*\}", "", body)
        # Protect complete math expressions before processing lists or inline text.
        # Nested cases/split/align environments inside display math stay untouched.
        math_pattern = re.compile(
            r"(?<!\\)\\\[(.*?)\\\]|(?<!\\)\\\((.*?)\\\)|(?<!\\)\$\$(.*?)\$\$"
            r"|(?<!\\)\$(?!\$)(.*?)(?<!\\)\$"
            r"|\\begin\{(align\*?|equation\*?|split|cases)\}(.*?)\\end\{\5\}",
            re.DOTALL,
        )

        def math(match: re.Match[str]) -> str:
            inline = match.group(2) if match.group(2) is not None else match.group(4)
            if inline is not None:
                return protect(f"${inline.strip()}$")
            content = next((match.group(i) for i in (1, 3) if match.group(i) is not None), None)
            if content is None:
                name = match.group(5).rstrip("*")
                # aligned is valid inside $$; align/equation are outer environments.
                content = match.group(6).strip()
                if name == "align":
                    content = f"\\begin{{aligned}}\n{content}\n\\end{{aligned}}"
                elif name != "equation":
                    content = f"\\begin{{{name}}}\n{content}\n\\end{{{name}}}"
            content = "\n".join(line.strip() for line in content.strip().splitlines())
            return "\n\n" + protect(f"$$\n{content}\n$$") + "\n\n"

        body = math_pattern.sub(math, body)

        def table(match: re.Match[str]) -> str:
            columns = re.findall(r"[lcr]", match.group(1))
            rows = []
            content = re.sub(r"\\hline\b", "", match.group(2))
            for row in re.split(r"\\\\(?:\s*\[[^]]*\])?", content):
                if row.strip():
                    cells = [self.inline(cell.strip()).replace("|", r"\|") for cell in re.split(r"(?<!\\)&", row)]
                    if len(cells) != len(columns):
                        return match.group(0)
                    rows.append(cells)
            if not rows or not re.fullmatch(r"[|lcr\s]+", match.group(1)):
                return match.group(0)
            separators = {"l": ":---", "c": ":---:", "r": "---:"}
            rendered = ["| " + " | ".join(rows[0]) + " |",
                        "| " + " | ".join(separators[col] for col in columns) + " |"]
            rendered.extend("| " + " | ".join(row) + " |" for row in rows[1:])
            return "\n\n" + protect("\n".join(rendered)) + "\n\n"

        body = re.sub(r"\\begin\{tabular\}\{([^{}]*)\}(.*?)\\end\{tabular\}", table, body, flags=re.DOTALL)
        # TeX source indentation has no layout meaning; retaining it creates code blocks.
        body = "\n".join(line.lstrip() for line in body.splitlines())
        body = re.sub(r"\\\\\*?(?:\s*\[[^]]*\])?", "\n", body)
        body = re.sub(r"\\setlength\s*\{[^{}]*\}\s*\{[^{}]*\}", "", body)
        body = re.sub(r"\\(?:noindent|hfill)\b", "", body)
        body = body.replace("\\titlepage", "").replace("\\tableofcontents", "")
        body = self.convert_lists(body)
        image_pattern = re.compile(r"\\includegraphics(?:\[([^]]*)\])?\{([^}]+)\}")
        cursor = 0
        resize_pattern = re.compile(r"\\resizebox\*?\s*\{")
        while match := resize_pattern.search(body, cursor):
            try:
                width, pos = read_group(body, match.end() - 1)
                while body[pos:pos + 1].isspace():
                    pos += 1
                height, pos = read_group(body, pos)
                while body[pos:pos + 1].isspace():
                    pos += 1
                content, end = read_group(body, pos)
            except ValueError:
                cursor = match.end()
                continue
            if image_pattern.sub("", content).strip():
                cursor = end
                continue
            dimensions = []
            if width.strip() != "!":
                dimensions.append(f"width={width.strip()}")
            if height.strip() != "!":
                dimensions.append(f"height={height.strip()}")
            def resized_image(image_match: re.Match[str]) -> str:
                options = ",".join(filter(None, [image_match.group(1), *dimensions]))
                return f"\\includegraphics[{options}]{{{image_match.group(2)}}}"
            replacement = image_pattern.sub(resized_image, content)
            body = body[:match.start()] + replacement + body[end:]
            cursor = match.start() + len(replacement)

        def image(match: re.Match[str]) -> str:
            path = self.copy_image(match.group(2))
            attributes = []
            if match.group(1):
                found = re.search(r"width\s*=\s*([0-9.]+)\\textwidth", match.group(1))
                if found:
                    attributes.append(f'width="{float(found.group(1)) * 100:g}%"')
                found = re.search(r"height\s*=\s*([0-9.]+)\\textheight", match.group(1))
                if found:
                    attributes.append(f'height="{float(found.group(1)) * 700:g}"')
            suffix = "{" + " ".join(attributes) + "}" if attributes else ""
            return f"![]({path}){suffix}"
        body = image_pattern.sub(image, body)
        body = re.sub(r"\\(?:begin|end)\{(?:center|small)\}", "", body)
        body = self.inline(body)
        for index in reversed(range(len(protected))):
            value = protected[index]
            token = f"@@PROTECTED{index}@@"
            # Keep Markdown blocks at the indentation of their enclosing list.
            body = re.sub(
                rf"(?m)^([ \t]*){re.escape(token)}$",
                lambda match: "\n".join(match.group(1) + line if line else "" for line in value.splitlines()),
                body,
            )
            body = body.replace(token, value)
        body = re.sub(r"[ \t]+\n", "\n", body)
        body = re.sub(r"\n{3,}", "\n\n", body)
        # Beamer places these figures beside the table with negative spacing.
        # Use columns so the figure stays within the slide after removing that spacing.
        standalone_images = list(re.finditer(r"(?m)^!\[\]\([^)]+\)(?:\{[^}]*\})?[ \t]*$", body))
        if re.search(r"(?m)^\s*\| .* \|$", body) and len(standalone_images) == 1:
            figure = standalone_images[0]
            if not body[figure.end():].strip():
                text_column = body[:figure.start()].strip()
                body = (
                    '::: {.columns}\n\n::: {.column width="60%"}\n\n'
                    + text_column
                    + '\n\n:::\n\n::: {.column width="40%"}\n\n'
                    + figure.group().strip()
                    + '\n\n:::\n\n:::'
                )
        # Restore code last so whitespace, comments, and TeX-like strings stay literal.
        for index, value in enumerate(listings):
            token = f"@@LISTING{index}@@"

            def restore_code(match: re.Match[str]) -> str:
                indent = match.group(1)
                marker = match.group(2) or ""
                continuation = indent + (" " * max(3, len(marker)) if marker else "")
                lines = value.split("\n")
                return indent + marker + lines[0] + "\n" + "\n".join(continuation + line for line in lines[1:])

            body = re.sub(rf"(?m)^([ \t]*)([-] |[0-9]+\. )?{re.escape(token)}[ \t]*$", restore_code, body)
            body = body.replace(token, value)
        return body.strip()

    def convert(self, source: Path) -> tuple[str, dict[str, int]]:
        expanded = self.expand_inputs(source, source.parent.resolve())
        metadata = self.metadata(expanded)
        nodes = self.extract_structure(expanded, source)
        frames = sum(kind == "frame" for kind, _, _ in nodes)
        lines = ["---", f'title: "{metadata.get("title", source.stem).replace(chr(34), chr(39))}"']
        if metadata.get("author"):
            lines.append(f'author: "{self.inline(metadata["author"]).replace(chr(34), chr(39))}"')
        if metadata.get("date"):
            lines.append(f'date: "{metadata["date"]}"')
        lines += ["lang: ja", "format:", "  revealjs:", "    slide-number: c/t", "    hash: true", "    transition: none"]
        if any(re.search(r"\\tableofcontents\b", strip_comments(body)) for kind, _, body in nodes if kind == "frame"):
            lines += ["    toc: true", "    toc-depth: 1", '    toc-title: "目次"']
        lines += ["---", ""]
        for kind, title, body in nodes:
            if kind == "section":
                if title:
                    lines.extend((f"# {title}", ""))
            else:
                converted_body = self.convert_body(body)
                if "\\titlepage" in strip_comments(body) and not converted_body:
                    continue
                lines.extend((f"## {title}", "", converted_body, ""))
        slides = sum(line.startswith("## ") for line in lines)
        return "\n".join(lines).rstrip() + "\n", {"frames_in": frames, "slides_out": slides}


def find_masters(input_path: Path, all_masters: bool) -> list[Path]:
    if input_path.is_file():
        return [input_path]
    if not all_masters:
        raise ValueError("a directory input requires --all-masters")
    return sorted(path for path in input_path.rglob("*.tex") if "\\documentclass" in path.read_text(encoding="utf-8"))


def parser() -> argparse.ArgumentParser:
    result = argparse.ArgumentParser(description=__doc__)
    result.add_argument("input", type=Path)
    result.add_argument("--output-dir", type=Path, default=Path("quarto"))
    result.add_argument("--all-masters", action="store_true")
    result.add_argument("--assets", choices=("copy", "symlink"), default="copy")
    result.add_argument("--report", type=Path)
    result.add_argument("--check", action="store_true")
    result.add_argument("--strict", action="store_true")
    result.add_argument("--version", action="version", version=VERSION)
    return result


def main(argv: list[str] | None = None) -> int:
    args = parser().parse_args(argv)
    input_path = args.input.resolve()
    if not input_path.exists():
        parser().error(f"input does not exist: {args.input}")
    try:
        masters = find_masters(input_path, args.all_masters)
    except ValueError as exc:
        parser().error(str(exc))
    source_root = input_path if input_path.is_dir() else input_path.parent
    output_root = args.output_dir.resolve()
    converter = Converter(source_root, output_root, args.assets, write_assets=not args.check)
    decks = []
    staged = Path(tempfile.mkdtemp(prefix="beamer-quarto-"))
    try:
        for master in masters:
            qmd, counts = converter.convert(master)
            relative = master.relative_to(source_root).with_suffix(".qmd")
            target = output_root / relative
            decks.append({"input": master.as_posix(), "output": target.as_posix(), "counts": counts})
            if not args.check:
                temporary = staged / relative
                temporary.parent.mkdir(parents=True, exist_ok=True)
                temporary.write_text(qmd, encoding="utf-8")
        if not args.check:
            output_root.mkdir(parents=True, exist_ok=True)
            for temporary in staged.rglob("*.qmd"):
                destination = output_root / temporary.relative_to(staged)
                destination.parent.mkdir(parents=True, exist_ok=True)
                temporary.replace(destination)
            if converter.asset_manifest:
                (output_root / "asset-manifest.json").write_text(
                    json.dumps(converter.asset_manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8"
                )
    finally:
        shutil.rmtree(staged, ignore_errors=True)

    report = {
        "schema_version": 1,
        "tool_version": VERSION,
        "decks": decks,
        "diagnostics": [asdict(item) for item in converter.diagnostics],
    }
    if args.report:
        args.report.parent.mkdir(parents=True, exist_ok=True)
        args.report.write_text(json.dumps(report, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    errors = sum(item.severity == "error" for item in converter.diagnostics)
    print(f"converted {len(decks)} deck(s); {len(converter.diagnostics)} diagnostic(s), {errors} error(s)")
    return 1 if args.strict and errors else 0


if __name__ == "__main__":
    sys.exit(main())
