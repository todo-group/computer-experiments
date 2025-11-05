import numpy as np
import matplotlib.pyplot as plt
from scipy.special import erf, erfc

# パラメータ設定
alpha = 1
r = np.linspace(0.01, 3, 500)  # r=0を避ける

# erfc, erf の計算
real_space = erfc(alpha * r) / r
fourier_space = erf(alpha * r) / r
inverse_r = 1 / r

# プロット
plt.figure(figsize=(5, 5))
plt.plot(r, inverse_r, label=r'$1/r$', color='blue')
plt.plot(r, real_space, label=r'$\mathrm{erfc}(\alpha r)/r$', color='red')
plt.plot(r, fourier_space, label=r'$\mathrm{erf}(\alpha r)/r$', color='orange')
plt.axvline(1/alpha, color='k', linestyle='--', label=r'split point $\alpha^{-1} (= 1)$')

plt.xlabel(r'distance $r$', fontsize=12)
plt.ylabel(r'potential term', fontsize=12)
plt.legend()
plt.ylim(0, 2)
plt.grid(True)
# plt.tight_layout()
plt.savefig('ewald.pdf')
