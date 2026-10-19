#!/usr/bin/env python3
"""Reproduce the illustrative harmonic-oscillator figures for Lecture 2 (2026).

Numerical kernels use only the standard library. Figure generation additionally
requires numpy and matplotlib. Run from any directory; outputs go to lecture/image.
"""
from __future__ import annotations

import json
import math
from pathlib import Path
import random


def energy(q, p, mass=1.0, spring=1.0):
    return p * p / (2 * mass) + spring * q * q / 2


def euler(q, p, h, mass=1.0, spring=1.0):
    return q + h * p / mass, p - h * spring * q


def symplectic_euler(q, p, h, mass=1.0, spring=1.0):
    q += h * p / mass
    return q, p - h * spring * q


def velocity_verlet(q, p, h, mass=1.0, spring=1.0):
    p -= h * spring * q / 2
    q += h * p / mass
    p -= h * spring * q / 2
    return q, p


def hmc_step(q, rng, h, steps, metropolis=True):
    """One transition for S(q)=q^2/2, M=1, including a momentum flip.

    Returns the retained q (also on rejection), acceptance flag and energy error.
    metropolis=False is deliberately biased and used only as a counterexample.
    """
    p = rng.gauss(0.0, 1.0)
    initial_energy = energy(q, p)
    proposal = q
    for _ in range(steps):
        proposal, p = velocity_verlet(proposal, p, h)
    p = -p
    delta = energy(proposal, p) - initial_energy
    # 1-random() is in (0,1], so the logarithm is always defined.
    accepted = math.isfinite(delta) and (
        not metropolis or math.log(1.0 - rng.random()) < min(0.0, -delta)
    )
    return (proposal if accepted else q), accepted, delta


def sample_gaussian(seed=20261023, samples=40000, burn_in=2000,
                    h=1.2, steps=3, metropolis=True):
    rng = random.Random(seed)
    q = 0.0
    retained, accepted = [], 0
    for i in range(samples + burn_in):
        q, accept, _ = hmc_step(q, rng, h, steps, metropolis)
        if i >= burn_in:
            retained.append(q)
            accepted += accept
    return retained, accepted / samples


def main():
    import matplotlib
    matplotlib.use("Agg")
    import matplotlib.pyplot as plt
    import numpy as np

    output = Path(__file__).resolve().parents[1] / "lecture" / "image"
    plt.rcParams.update({"font.size": 13, "axes.spines.top": False,
                         "axes.spines.right": False, "svg.fonttype": "path"})
    metadata = {"CreationDate": None, "ModDate": None}
    fig, axes = plt.subplots(1, 2, figsize=(10, 3.4), layout="constrained")
    for stepper, name, color in [(euler, "Euler", "#a8343a"),
                                 (symplectic_euler, "Symplectic Euler", "#cf7917"),
                                 (velocity_verlet, "Velocity Verlet", "#176b9b")]:
        h = 0.1
        q, p = 1.0, 0.0
        values = [0.0]
        for _ in range(400):
            q, p = stepper(q, p, h)
            values.append(energy(q, p) / 0.5 - 1)
        axes[0].plot(np.arange(401)*h, values, label=name, color=color)
    axes[0].set(xlabel="Time", ylabel=r"$(H(t)-H(0))/H(0)$", title=r"Same step size $h=0.1$")
    axes[0].legend(fontsize=10)
    for h, color in [(0.2, "#a8343a"), (0.1, "#176b9b")]:
        q, p = 1.0, 0.0
        count = round(20 / h)
        values = [0.0]
        for _ in range(count):
            q, p = velocity_verlet(q, p, h)
            values.append(energy(q, p) / 0.5 - 1)
        axes[1].plot(np.arange(count+1)*h, values, label=f"h={h}", color=color)
    axes[1].set(xlabel="Time", ylabel=r"$(H(t)-H(0))/H(0)$", title="Velocity Verlet: step-size dependence")
    axes[1].legend(fontsize=10)
    fig.savefig(output / "md-2026-energy.pdf", metadata=metadata)
    plt.close(fig)

    fig, axes = plt.subplots(1, 2, figsize=(10, 3.4), layout="constrained")
    x = np.linspace(-4.5, 4.5, 400)
    stats = {"seed": 20261023, "samples": 40000, "burn_in": 2000, "h": 1.2, "L": 3}
    for ax, corrected in zip(axes, [True, False]):
        samples, acceptance = sample_gaussian(metropolis=corrected)
        samples = np.array(samples)
        label = "HMC (Metropolis corrected)" if corrected else "Without Metropolis correction"
        color = "#176b9b" if corrected else "#a8343a"
        ax.hist(samples, bins=np.linspace(-4.5, 4.5, 61), density=True,
                color=color, alpha=0.5, label="Generated samples")
        ax.plot(x, np.exp(-x*x/2)/math.sqrt(2*math.pi), "k-", label="Target N(0,1)")
        ax.set(xlabel="q", ylabel="Density", title=label, ylim=(0, 0.47))
        ax.text(0.03, 0.96, f"mean(q²) = {np.mean(samples*samples):.3f}",
                transform=ax.transAxes, va="top", fontsize=11)
        ax.legend(loc="upper right", fontsize=9)
        stats["hmc" if corrected else "uncorrected"] = {
            "mean": float(np.mean(samples)), "mean_q2": float(np.mean(samples*samples)),
            "acceptance": acceptance}
    fig.savefig(output / "md-2026-sampling.pdf", metadata=metadata)
    plt.close(fig)
    print(json.dumps(stats, ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()
