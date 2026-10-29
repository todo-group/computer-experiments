"""Exercise the real Pandoc writer, including its fail-on-warning mode."""
import json
import os
from html.parser import HTMLParser
from pathlib import Path
import shutil
import subprocess

import pytest


PANDOC = os.environ.get("PANDOC") or shutil.which("pandoc")
FILTER = Path(__file__).parents[1] / "tools" / "mathjax-tex.lua"
pytestmark = pytest.mark.skipif(not PANDOC, reason="set PANDOC to the Pandoc executable")


class MathText(HTMLParser):
    def __init__(self):
        super().__init__()
        self.tags = []
        self.text = ""

    def handle_starttag(self, tag, attrs):
        self.tags.append((tag, dict(attrs)))

    def handle_data(self, data):
        self.text += data


def render(tex, kind="DisplayMath", target="revealjs"):
    ast = json.loads(subprocess.run(
        [PANDOC, "-f", "markdown", "-t", "json"], input="",
        text=True, capture_output=True, check=True,
    ).stdout)
    ast["blocks"] = [{"t": "Para", "c": [{"t": "Math", "c": [{"t": kind}, tex]}]}]
    return subprocess.run(
        [PANDOC, "-f", "json", "-t", target, "--lua-filter", str(FILTER), "--fail-if-warnings"],
        input=json.dumps(ast), text=True, capture_output=True, check=True,
    ).stdout


@pytest.mark.parametrize("kind,tex", [
    ("DisplayMath", r"""\begin{aligned}
    \langle A\rangle &= \frac{1}{Z(E)}\int A\,dp\,dx \\
    &\simeq \frac{1}{t_{\rm max}}\int_0^{t_{\rm max}} A(t)\,dt
    \end{aligned}"""),
    ("DisplayMath", r"""\begin{aligned}
    \textcolor{red}{p(t+h/2)} &= p(t)-hf(t)/2 \\
    \textcolor{blue}{q(t+h)} &= q(t)+h\textcolor{red}{p(t+h/2)}
    \end{aligned}"""),
    ("InlineMath", r"p_i=p_i'/\textcolor{red}{s}"),
    ("InlineMath", r"a < b \quad c > d \quad \text{A&B}"),
])
def test_tex_reaches_mathjax_without_conversion_warnings(kind, tex):
    html = render(tex, kind)
    parsed = MathText()
    parsed.feed(html)
    display = kind == "DisplayMath"
    opening, closing = (r"\[", r"\]") if display else (r"\(", r"\)")
    assert parsed.text.strip() == opening + tex + closing
    assert ("span", {"class": "math " + ("display" if display else "inline")}) in parsed.tags
    assert all(tag in {"section", "p", "span"} for tag, _ in parsed.tags)
    assert "<math" not in html


def test_non_reveal_output_keeps_native_math():
    tex = r"\textcolor{red}{x^2}"
    assert render(tex, target="latex") == render_without_filter(tex)


def render_without_filter(tex):
    return subprocess.run(
        [PANDOC, "-f", "markdown", "-t", "latex", "--fail-if-warnings"],
        input="$$" + tex + "$$", text=True, capture_output=True, check=True,
    ).stdout
