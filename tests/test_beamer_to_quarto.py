import importlib.util
import json
import shutil
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


def test_gray_groups_preserve_nested_markup_and_scope():
    assert module.Converter.inline(
        r"前 {\color{gray} \textbf{休講} と $x_{n}$} 後 {\color{gray} 次回}"
    ) == r"前 [**休講** と $x_{n}$]{.gray} 後 [次回]{.gray}"
    assert module.Converter.inline(
        r"{\color{gray} 外 {\color{gray} 内}}"
    ) == "[外 [内]{.gray}]{.gray}"
    assert module.Converter.inline(r"{\color{gray} 未完") == r"{\color{gray} 未完"
    assert module.Converter.inline(r"{\color{blue} 青}") == r"{\color{blue} 青}"


def test_red_groups_preserve_markup_and_nested_colors(tmp_path):
    converter = module.Converter(tmp_path, tmp_path / "output")
    result = converter.convert_body(r"""\begin{itemize}
\item 前 {\color{red} \textbf{重要} と $x_{n}$ {\color{gray} 注記} } 後
\end{itemize}
\begin{lstlisting}[language=Python]
text = r"{\color{red} code}"
\end{lstlisting}
""")
    assert "- 前 [**重要** と $x_{n}$ [注記]{.gray}]{.red} 後" in result
    assert 'text = r"{\\color{red} code}"' in result
    assert converter.inline(r"{\color{red} 未完") == r"{\color{red} 未完"


def test_gray_group_in_list_does_not_change_code(tmp_path):
    converter = module.Converter(tmp_path, tmp_path / "output")
    result = converter.convert_body(r"""\begin{itemize}
\item {\color{gray} 10月9日 \textbf{休講}}
\item 通常
\end{itemize}
\begin{lstlisting}[language=Python]
text = r"{\color{gray} code}"
\end{lstlisting}
""")
    assert "- [10月9日 **休講**]{.gray}" in result
    assert "- 通常" in result
    assert 'text = r"{\\color{gray} code}"' in result


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


def test_toc_uses_active_sections_and_ignores_commented_commands(tmp_path):
    source = tmp_path / "deck.tex"
    source.write_text(r"""\documentclass{beamer}
\begin{frame}{目次}
\tableofcontents
\end{frame}
% \section{非表示}
\section{第一節}
\begin{frame}{説明}本文\end{frame}
\section{第二節}
""", encoding="utf-8")
    converter = module.Converter(tmp_path, tmp_path / "output")
    qmd, counts = converter.convert(source)
    assert "    toc: true\n    toc-depth: 1" in qmd
    assert "# 第一節" in qmd and "# 第二節" in qmd
    assert "非表示" not in qmd
    assert r"\tableofcontents" not in qmd
    assert counts == {"frames_in": 2, "slides_out": 2}
    source.write_text(source.read_text().replace(r"\tableofcontents", r"% \tableofcontents"))
    qmd, _ = converter.convert(source)
    assert "toc: true" not in qmd


def test_title_page_is_not_duplicated(tmp_path):
    source = tmp_path / "deck.tex"
    source.write_text(r"""\documentclass{beamer}
\title{講義}
\begin{frame}\titlepage\tableofcontents\end{frame}
\section{節}
\begin{frame}{本文}内容\end{frame}
""", encoding="utf-8")
    qmd, counts = module.Converter(tmp_path, tmp_path / "output").convert(source)
    assert 'title: "講義"' in qmd
    assert "## タイトル" not in qmd
    assert "## 本文" in qmd
    assert "toc: true" in qmd
    assert counts == {"frames_in": 2, "slides_out": 1}


def test_nested_list_math_has_one_display_delimiter_pair(tmp_path):
    converter = module.Converter(tmp_path, tmp_path / "output")
    body = r"""\begin{itemize}
\item 外側
  \begin{itemize}
  \item 内側
    \begin{align*}
      F(r) &= \sqrt{-2\log(1-q)} \\
      x &= 1
    \end{align*}
  \item 次の項目
  \end{itemize}
\end{itemize}
\[ \theta = \begin{cases}1 & x > 0 \\ 0 & x \le 0\end{cases} \]
"""
    result = converter.convert_body(body)
    assert result.count("$$") == 4
    assert "\n      $$\n      \\begin{aligned}" in result
    assert r"x &= 1" in result
    assert r"\begin{cases}1 & x > 0 \\ 0 & x \le 0\end{cases}" in result
    assert "@@PROTECTED" not in result


def test_tex_indentation_and_spacing_do_not_create_code_blocks(tmp_path):
    result = module.Converter(tmp_path, tmp_path / "output").convert_body(r"""\begin{itemize}
  \item 式
      $F(x) = 1 - e^{-x/\mu}$
  \item 本文\\[2em]次の行\\*[.5em]末尾
\end{itemize}
\begin{lstlisting}[language=Python]
text = r"\\[2em]"
    indented_code()
\end{lstlisting}
""")
    assert "\n   $F(x)" in result
    assert "本文\n   次の行\n   末尾" in result
    assert 'text = r"\\\\[2em]"\n    indented_code()' in result
    result = module.Converter(tmp_path, tmp_path / "output").convert_body(
        r"本文\\[2em]次の行\[x=1\]"
    )
    assert "本文\n次の行" in result
    assert result.count("$$") == 2 and "[2em]" not in result


def test_tabular_preserves_math_cells_inside_list(tmp_path):
    result = module.Converter(tmp_path, tmp_path / "output").convert_body(r"""\begin{itemize}
\item 結果
\begin{tabular}{|c|c|c|}
\hline
$M$ & 平均値 & 誤差 \\
\hline
100 & 4.8 & 1.3 \\
10000 & 3.12 & 0.11 \\
\hline
\end{tabular}
\end{itemize}
""")
    assert "   | $M$ | 平均値 | 誤差 |" in result
    assert "   | :---: | :---: | :---: |" in result
    assert "   | 10000 | 3.12 | 0.11 |" in result
    assert "tabular" not in result and "@@PROTECTED" not in result


def test_pdf_images_are_converted_to_svg_with_resize_dimensions(tmp_path):
    source_root = MODULE_PATH.parents[1] / "lecture"
    output = tmp_path / "output"
    converter = module.Converter(source_root, output, assets="symlink")
    result = converter.convert_body(
        r"\resizebox{!}{.45\textheight}{\includegraphics{image/coth-1.pdf}}"
    )
    if not shutil.which("pdftocairo"):
        assert converter.diagnostics[0].code == "image-converter-missing"
        return
    assert result == '![](assets/image/coth-1.svg){height="315"}'
    svg = output / "assets/image/coth-1.svg"
    assert svg.is_file() and not svg.is_symlink()
    assert "<svg" in svg.read_text()
    assert converter.asset_manifest["assets/image/coth-1.svg"]["source"].endswith("coth-1.pdf")


def test_table_and_figure_are_placed_side_by_side(tmp_path):
    converter = module.Converter(MODULE_PATH.parents[1] / "lecture", tmp_path, write_assets=False)
    result = converter.convert_body(r"""\begin{itemize}
\item 結果
\begin{tabular}{cc}
$M$ & 平均値 \\
100 & 4.8 \\
\end{tabular}
\end{itemize}
\resizebox{!}{.45\textheight}{\includegraphics{image/coth-1.pdf}}
""")
    assert '::: {.column width="60%"}' in result
    assert '::: {.column width="40%"}' in result
    assert '| $M$ | 平均値 |' in result
    assert 'coth-1.svg' in result


def test_verbatim_preserves_literal_content_and_whitespace(tmp_path):
    converter = module.Converter(tmp_path, tmp_path / "output")
    code = "    if (x % 2) {  \n      value = \"$x$ ~ \\textbf{literal}\";\n\n\n      // ```\n    }"
    result = converter.convert_body("前\n\\begin{verbatim}\n" + code + "\n\\end{verbatim}\n後")
    assert "\n\n````{.verbatim}\n" + code + "\n````\n\n" in result
    assert r"\begin{verbatim}" not in result
    assert "@@LISTING" not in result


def test_verbatim_in_nested_list_retains_code_indentation(tmp_path):
    converter = module.Converter(tmp_path, tmp_path / "output")
    result = converter.convert_body(r"""\begin{itemize}
\item 外側
\begin{itemize}
\item 例
\begin{verbatim}
for (;;) {
  work();
}
\end{verbatim}
\item 続き
\end{itemize}
\end{itemize}
""")
    assert "      ```{.verbatim}\n      for (;;) {\n        work();\n      }\n      ```" in result
    assert "   - 続き" in result


def test_verbatim_star_as_first_list_content(tmp_path):
    converter = module.Converter(tmp_path, tmp_path / "output")
    result = converter.convert_body(r"""\begin{enumerate}
\item \begin{verbatim*}
[literal]
  indented
\end{verbatim*}
\end{enumerate}
""")
    assert result == "1. ```{.verbatim}\n   [literal]\n     indented\n   ```"
