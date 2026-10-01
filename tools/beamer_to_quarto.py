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
            body = self.convert_lists(text[match.end() : body_end])
            items = re.split(r"\\item(?:\s*\[[^]]*\])?\s*", body)[1:]
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
        destination = self.output_root / relative
        if self.write_assets:
            destination.parent.mkdir(parents=True, exist_ok=True)
            if self.assets_mode == "symlink":
                if destination.exists() or destination.is_symlink():
                    destination.unlink()
                destination.symlink_to(source)
            else:
                shutil.copy2(source, destination)
        digest = hashlib.sha256(source.read_bytes()).hexdigest()
        self.asset_manifest[relative.as_posix()] = {"source": source.as_posix(), "sha256": digest}
        if source.suffix.lower() in {".pdf", ".eps"}:
            self.diagnose("warning", "image-browser-format", f"copied {source.suffix} image; convert to SVG for browsers", source)
        return relative.as_posix()

    def convert_body(self, body: str) -> str:
        listings: list[str] = []

        def listing(match: re.Match[str]) -> str:
            option = match.group(1) or ""
            language_match = re.search(r"language\s*=\s*([^,\]]+)", option)
            language = (language_match.group(1).strip().lower() if language_match else "c")
            language = {"bash": "bash", "python": "python", "c++": "cpp"}.get(language, language)
            token = f"@@LISTING{len(listings)}@@"
            listings.append(f"```{language}\n{match.group(2).strip()}\n```")
            return token

        body = re.sub(
            r"\\begin\{lstlisting\}(?:\[([^]]*)\])?(.*?)\\end\{lstlisting\}",
            listing, body, flags=re.DOTALL,
        )
        body = strip_comments(body)
        body = re.sub(r"\\setlength\s*\{[^{}]*\}\s*\{[^{}]*\}", "", body)
        body = re.sub(r"\\(?:vspace|hspace)\*?\s*\{[^{}]*\}", "", body)
        body = body.replace("\\titlepage", "").replace("\\tableofcontents", "")
        body = self.convert_lists(body)
        body = re.sub(r"\\\[(.*?)\\\]", lambda m: f"\n$$\n{m.group(1).strip()}\n$$\n", body, flags=re.DOTALL)
        body = re.sub(
            r"\\begin\{(align\*?|equation\*?|split|cases)\}(.*?)\\end\{\1\}",
            lambda m: f"\n$$\n\\begin{{{m.group(1)}}}{m.group(2)}\\end{{{m.group(1)}}}\n$$\n",
            body, flags=re.DOTALL,
        )

        image_pattern = re.compile(r"\\includegraphics(?:\[([^]]*)\])?\{([^}]+)\}")
        def image(match: re.Match[str]) -> str:
            path = self.copy_image(match.group(2))
            width = ""
            if match.group(1):
                found = re.search(r"width\s*=\s*([0-9.]+)\\textwidth", match.group(1))
                if found:
                    width = f'{{width="{float(found.group(1)) * 100:g}%"}}'
            return f"![]({path}){width}"
        body = image_pattern.sub(image, body)
        body = re.sub(r"\\(?:begin|end)\{(?:center|small)\}", "", body)
        body = self.inline(body)
        for index, value in enumerate(listings):
            body = body.replace(f"@@LISTING{index}@@", value)
        body = re.sub(r"[ \t]+\n", "\n", body)
        body = re.sub(r"\n{3,}", "\n\n", body)
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
        lines += ["lang: ja", "format:", "  revealjs:", "    slide-number: c/t", "    hash: true", "    transition: none", "---", ""]
        for kind, title, body in nodes:
            if kind == "section":
                if title:
                    lines.extend((f"# {title}", ""))
            else:
                lines.extend((f"## {title}", "", self.convert_body(body), ""))
        return "\n".join(lines).rstrip() + "\n", {"frames_in": frames, "slides_out": frames}


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
