"""Check the equations and numerical examples used in Lecture 2 (2026)."""
import importlib.util
import hashlib
import json
import math
from pathlib import Path
import re

import pytest

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location("md_figures", ROOT / "tools/generate_md_sampling_figures.py")
md = importlib.util.module_from_spec(spec)
spec.loader.exec_module(md)


def test_euler_energy_growth_and_symplectic_euler_invariant():
    q, p, h = 0.7, -0.4, 0.15
    q1, p1 = md.euler(q, p, h)
    assert md.energy(q1, p1) == pytest.approx((1+h*h)*md.energy(q, p))
    invariant = md.energy(q, p) + h*q*p/2
    for _ in range(1000):
        q, p = md.symplectic_euler(q, p, h)
    assert md.energy(q, p) + h*q*p/2 == pytest.approx(invariant, abs=1e-12)


def test_verlet_mass_factors_reversibility_and_symplectic_matrix():
    q, p, h, mass, spring = 0.7, -0.4, 0.1, 2.3, 1.7
    q1, p1 = md.velocity_verlet(q, p, h, mass, spring)
    assert q1 == pytest.approx(q+h*p/mass-h*h*spring*q/(2*mass))
    assert p1 == pytest.approx(p-h*spring*(q+q1)/2)
    for _ in range(100):
        q, p = md.velocity_verlet(q, p, h, mass, spring)
    p = -p
    for _ in range(100):
        q, p = md.velocity_verlet(q, p, h, mass, spring)
    assert (q, -p) == pytest.approx((0.7, -0.4), abs=1e-12)
    a, c = md.velocity_verlet(1, 0, h, mass, spring)
    b, d = md.velocity_verlet(0, 1, h, mass, spring)
    # In two dimensions det=1 is equivalent to M^T J M=J.
    assert a*d-b*c == pytest.approx(1, abs=1e-14)


def test_verlet_second_order_at_fixed_time():
    errors = []
    for h in (0.1, 0.05, 0.025):
        q, p = 1.0, 0.0
        for _ in range(round(1/h)):
            q, p = md.velocity_verlet(q, p, h)
        errors.append(math.hypot(q-math.cos(1), p+math.sin(1)))
    assert 3.9 < errors[0]/errors[1] < 4.1
    assert 3.9 < errors[1]/errors[2] < 4.1


def test_hmc_rejection_retains_the_original_state():
    class ControlledRng:
        def gauss(self, mean, std):
            return 2.0
        def random(self):
            return 0.01  # u=0.99: reject the positive energy error below.
    q, accepted, delta = md.hmc_step(0.0, ControlledRng(), h=1.2, steps=1)
    assert delta > 0
    assert not accepted
    assert q == 0.0


def test_gaussian_hmc_and_uncorrected_counterexample():
    values, acceptance = md.sample_gaussian()
    assert len(values) == 40000  # Includes repeated states on rejection.
    assert abs(sum(values)/len(values)) < 0.06
    assert abs(sum(q*q for q in values)/len(values)-1) < 0.06
    assert 0.6 < acceptance < 0.99
    wrong, _ = md.sample_gaussian(metropolis=False)
    # Leapfrog preserves p²+(1-h²/4)q². Full momentum refresh without
    # Metropolis therefore samples variance 1/(1-h²/4), away from resonances.
    assert sum(q*q for q in wrong)/len(wrong) == pytest.approx(1/(1-1.2**2/4), abs=0.08)


def test_yoshida_composition_coefficients():
    w1 = 1/(2-2**(1/3))
    w0 = -2**(1/3)/(2-2**(1/3))
    assert 2*w1+w0 == pytest.approx(1)
    assert 2*w1**3+w0**3 == pytest.approx(0, abs=1e-14)


def test_beamer_and_quarto_slide_and_equation_parity(tmp_path):
    converter_spec = importlib.util.spec_from_file_location("converter", ROOT / "tools/beamer_to_quarto.py")
    converter_module = importlib.util.module_from_spec(converter_spec)
    # dataclasses resolves annotations through sys.modules.
    import sys
    sys.modules[converter_spec.name] = converter_module
    converter_spec.loader.exec_module(converter_module)
    converter = converter_module.Converter(ROOT / "lecture", tmp_path, write_assets=False)
    generated, counts = converter.convert(ROOT / "lecture/lecture-2-2-2026.tex")
    edited = (ROOT / "quarto/lecture-2-2.qmd").read_text()
    assert counts == {"frames_in": 43, "slides_out": 42}
    assert re.findall(r"^## (.*)$", generated, re.M) == re.findall(r"^## (.*)$", edited, re.M)
    assert re.findall(r"\$\$(.*?)\$\$", generated, re.S) == re.findall(r"\$\$(.*?)\$\$", edited, re.S)
    assert 'subtitle: "分子動力学と統計的サンプリング"' in edited
    assert 'date: "2026/10/23"' in edited
    assert not converter.diagnostics


def test_figure_sources_and_references_match_manifest():
    manifest = json.loads((ROOT / "quarto/lecture-2-2-assets.json").read_text())
    qmd = (ROOT / "quarto/lecture-2-2.qmd").read_text()
    assert set(re.findall(r"!\[.*?\]\((assets/[^)]+)\)", qmd)) == set(manifest)
    for asset, info in manifest.items():
        assert (ROOT / "quarto" / asset).is_file()
        assert hashlib.sha256((ROOT / info['source']).read_bytes()).hexdigest() == info['sha256']
