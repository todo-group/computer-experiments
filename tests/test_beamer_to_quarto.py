import importlib.util
import json
import sys
from pathlib import Path


MODULE_PATH = Path(__file__).parents[1] / "tools" / "beamer_to_quarto.py"
SPEC = importlib.util.spec_from_file_location("beamer_to_quarto", MODULE_PATH)
module = importlib.util.module_from_spec(SPEC)
assert SPEC.loader
sys.modules[SPEC.name] = module
SPEC.loader.exec_module(module)


def test_balanced_group_supports_nested_content():
    assert module.read_group("{outer {inner}} tail", 0) == ("outer {inner}", 15)


def test_convert_deck_with_include_list_math_and_code(tmp_path):
    source = tmp_path / "source"
    source.mkdir()
    (source / "part.tex").write_text(
        r"""\section{節}
\begin{frame}[t,fragile]{例}
\begin{itemize}
\item 値は $x_1$ です
\item \href{https://example.com}{リンク}
\end{itemize}
\[
x = 1
\]
\begin{lstlisting}[language=Python]
print("ok")
\end{lstlisting}
\end{frame}
""",
        encoding="utf-8",
    )
    master = source / "deck.tex"
    master.write_text(
        r"""\documentclass{beamer}
\title{テスト}
\date{2026-10-01}
\begin{document}
\input{part.tex}
\end{document}
""",
        encoding="utf-8",
    )
    output = tmp_path / "output"
    report = tmp_path / "report.json"
    assert module.main([str(master), "--output-dir", str(output), "--report", str(report), "--strict"]) == 0
    qmd = (output / "deck.qmd").read_text(encoding="utf-8")
    assert 'title: "テスト"' in qmd
    assert "# 節" in qmd
    assert "## 例" in qmd
    assert "- 値は $x_1$ です" in qmd
    assert "[リンク](https://example.com)" in qmd
    assert "```python\nprint(\"ok\")\n```" in qmd
    assert json.loads(report.read_text(encoding="utf-8"))["decks"][0]["counts"] == {
        "frames_in": 1,
        "slides_out": 1,
    }


def test_strict_fails_for_missing_input(tmp_path):
    master = tmp_path / "deck.tex"
    master.write_text(
        r"\documentclass{beamer}\begin{document}\input{missing}\end{document}", encoding="utf-8"
    )
    report = tmp_path / "report.json"
    assert module.main([str(master), "--check", "--strict", "--report", str(report)]) == 1
    diagnostics = json.loads(report.read_text(encoding="utf-8"))["diagnostics"]
    assert diagnostics[0]["code"] == "input-missing"
