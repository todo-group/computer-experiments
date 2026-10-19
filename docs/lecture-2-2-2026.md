# 計算機実験II 講義2（2026年度）

2026年10月23日（金）16:50–18:35「分子動力学と統計的サンプリング」。
講義予定は `quarto/lecture-2-1.qmd` の2026年度版に従う。

## 原稿と構成

* 新しいBeamer原稿：`lecture/lecture-2-2-2026.tex`
* Quarto/Reveal.js原稿：`quarto/lecture-2-2.qmd`
* 本文42枚（本編36枚＋補足・文献6枚）。Beamerは表紙を加えて43ページ。
  Reveal.jsは表紙・目次・6枚のセクション扉を加えて50画面。
* 旧 `lecture/lecture-2-2.tex`、`lecture/lecture-2-3.tex` と共有フラグメントは変更しない。
  2026年度版は別入口とし、2025年度の講義をそのまま再現できるようにする。

105分の進行案（質疑を各ブロックに含む）。補足は時間がある場合または自習用。

| 時刻 | 分 | 内容 |
|---|---:|---|
| 16:50–16:55 | 5 | 到達目標、講義1・実習1との接続 |
| 16:55–17:15 | 20 | 古典粒子系、力・境界条件、時間平均とアンサンブル |
| 17:15–17:45 | 30 | Euler法との比較、シンプレクティック性、速度ベルレ法、検証 |
| 17:45–18:05 | 20 | 温度のゆらぎ、Langevin・Andersen・Nosé–Hoover熱浴、統計誤差 |
| 18:05–18:30 | 25 | 拡張分布、HMCの手順と正当性、数値例、効率の評価 |
| 18:30–18:35 | 5 | 要点、10月30日実習2への確認課題 |

実習に向けた課題は講義内の確認課題であり、提出物・締切・採点条件を新たに設定するものではない。

## 2025年度素材との対応

| 元素材（`lecture/` 内） | 2026年度版での扱い |
|---|---|
| `6_monte_carlo/molecular_dynamics-15,01,02,03.tex` | 古典粒子系・MD・周期境界を再構成。元フローチャートを再利用 |
| `2_ode/ode-03,07,08.tex` | 一般ODEの復習を、調和振動子のEuler法との比較に集約 |
| `2_ode/symplectic-01`〜`08.tex` | シンプレクティックEuler、リープフロッグ、速度ベルレを本編に配置 |
| `2_ode/symplectic-09`〜`12.tex` | 分割写像と四次合成を補足に集約。中央係数の負号を明示 |
| `6_monte_carlo/virial-01`〜`03.tex` | 温度・自由度を本編、ビリアル・圧力を補足へ |
| `6_monte_carlo/longrange-01`〜`04.tex` | エバルト分割・ツリー法の概念と元図を補足へ。詳細な展開式は省略 |
| `6_monte_carlo/molecular_dynamics-04`〜`14.tex` | 熱浴を整理。Noséの仮想時間による長い導出は、実時間の定常Liouville方程式による確認に置換 |
| `6_monte_carlo/hamiltonian_montecarlo-01`〜`06.tex` | 本編のまとめとして展開。運動量反転・可逆性・棄却処理・効率の評価を補強 |
| `0_about/about2-2.tex` | 旧年度の日程は再利用せず、現在の講義予定に整合させる |

数学・物理上の整理：

* シンプレクティック性と体積保存を区別し、元のエネルギーの厳密保存とは区別する。
* 長時間エネルギー誤差の説明に、滑らかさ・刻み幅・安定性の条件を付ける。
* 速度と運動量、力と加速度を区別し、一般の質量を式に残す。
* MDの時間平均と平衡平均の一致に、保存量の拘束とエルゴード性を明記する。
* 温度と運動エネルギーの記号を分け、自由度の拘束と運動エネルギーのゆらぎを示す。
* Andersenの衝突確率は `1-exp(-νh)`、`νh` は小刻み幅の近似とする。
* HMCの作用 `S=βU` と無次元ハミルトニアンを定義し、βの二重計上を防ぐ。
* HMCの提案を `R Φ_h^L` として可逆性・体積保存を示す。
  棄却時も元の状態を測定する。採択率と探索効率を区別する。
* 積分自己相関時間は講義1と同じ `Σ_{t=1} C(t)` の規約を使う。

## ビルド

Quarto、Poppler、Node.js、Chrome（またはChromium）を用いる。
Reveal.jsの本文・見出しは既存の `quarto/_quarto.yml` のBIZ UDPGothic・26pxを継承する。
講義2のみMathML＋直接読み込むMathJax 4を使う。これはMathJax 4に対して
Revealの旧MathJax 2 API（`Hub.Config`）を呼ぶエラーを避けるためである。
共通設定と講義1は変更しない。

```sh
quarto render quarto/lecture-2-2.qmd --to revealjs
make -C quarto lecture-2-2.pdf CHROME=/path/to/chrome

# 画像とReveal.jsの配布ファイルを1つにまとめたHTML
quarto render quarto/lecture-2-2.qmd --to revealjs --embed-resources
```

HTMLはMathJax・Webフォントの読み込みにインターネット接続を要する。
オフライン配布には生成したPDFを使う。MathJaxのスクリプトは動的なフォント読込みの
基準URLを保持するため `data-external` とし、HTMLへの埋込み対象から外す。

新しいBeamer版はXeLaTeXを使用する。BIZ UDPGothic（Regular/Bold）と
Latin Modern Math、およびTeX LiveのBeamer・fontspec・unicode-mathが必要。
旧原稿のupLaTeX/DVI用設定とは独立している。

```sh
cd lecture
xelatex -halt-on-error lecture-2-2-2026.tex
xelatex -halt-on-error lecture-2-2-2026.tex
```

## 図と再生成

`md-flowchart.pdf`、`ewald.pdf`、`tree-1.pdf`、`tree-2.pdf` は旧講義の図を再利用。
元PDFを変更せず、PDFに保存されているCropBoxをそのまま使う。
`quarto/lecture-2-2-assets.json` に元PDFのパス・SHA-256を記録する。
フローチャートのSVGはCropBoxに対応する211.27023 × 298.25475の範囲となる。

追加した2枚の図は、調和振動子の数値計算結果であり実測データではない。

```sh
python3 -m pip install numpy matplotlib
python3 tools/generate_md_sampling_figures.py
```

生成先は `lecture/image/md-2026-energy.pdf` と `md-2026-sampling.pdf`。
標本分布の条件は `h=1.2, L=3, seed=20261023`、平衡化2000回、測定40000回。
運動量はPython標準ライブラリの `random.Random` で生成する。
結果のモーメントと採択率は標準出力のJSONに出る。
HMCの `mean(q²)` は約1.031、補正なしは約1.560、HMCの採択率は約0.903。
補正なしの定常分散の解析値は `1/(1-h²/4)=1.5625`（軌道長の共鳴を除く）。
これらの標本平均は有限サンプルの結果であり、目標値そのものではない。

元の変換手順は `docs/beamer-to-quarto-design.md` を参照。
本講義を再変換する場合は、一時ディレクトリへ出力する。

```sh
python3 tools/beamer_to_quarto.py lecture/lecture-2-2-2026.tex \
  --output-dir /tmp/lecture-2-2-converted \
  --report /tmp/lecture-2-2-conversion.json --strict
```

`quarto/lecture-2-2.qmd` は変換後に手調整済み。
再変換時にはタイトルの副題、MathML/MathJax設定、図の大きさ・代替テキスト、
ツリー図の2カラム、数値例の発表者ノートを維持する。
PDF図を再生成した場合は、SVGを再変換し、manifestのSHA-256を更新する。

## 検証

```sh
python3 -m pip install -r requirements.txt
python3 -m pytest -q
```

`tests/test_md_sampling.py` は、Euler法のエネルギー増大、修正ハミルトニアン、
速度ベルレの質量因子・可逆性・シンプレクティック性・二次収束、HMC棄却時の状態保持、
ガウス標本分布、吉田の合成係数を検証する。
さらにBeamerとQuartoの42枚の見出し・全表示数式の一致、図の参照と元PDFのSHA-256を検証する。

ブラウザーではMathJaxの完了後に全スライドを巡回し、JavaScriptエラー、数式エラー、
画像欠落、スライド境界からのはみ出しを検査し、スクリーンショットを目視する。
配布PDFはページ数・数式・日本語・図を別途確認する。

```sh
# Playwrightを別ディレクトリにインストールして使う例
npm install --prefix /tmp/reveal-qa playwright
NODE_PATH=/tmp/reveal-qa/node_modules node tools/check_reveal_layout.cjs \
  /path/to/chrome quarto/lecture-2-2.html /tmp/lecture-2-2-layout
```

`report.json` と全画面のPNGを出力する。数式未描画や読み込みエラーも非0終了とする。
これは矩形による自動検査なので、文字の読みやすさと図の内容は目視で確認する。

使用文献はスライド末尾に記載。HMCはNealのレビュー、熱浴はHoover (1985)、
高次合成はYoshida (1990) を参照する。

### 実施結果（2026年10月19日）

* `python3 -m pytest -q`：既存16件＋追加8件、計24件成功。
* Beamer変換器の `--strict`：43入力フレーム／42本文スライド、診断0件。
* Quarto 1.8.27：Reveal.jsを生成。Chrome 131で50画面・154数式を確認。
  JavaScriptエラー、MathJaxエラー、画像欠落、境界超過はいずれも0件。
* XeLaTeX（TeX Live 2023）：43ページ。Overfull、文字欠落、フォント警告なし。
* 既存の `tools/print_reveal_pdf.mjs` によるReveal.js PDF：50ページ。
* 全スライドの画面確認と、PDFの代表的な数式・図の目視確認を実施。
* `git diff --check` 成功。旧年度原稿・既存共有フラグメント・講義1・共通設定の差分なし。

ブラウザー確認では実行環境のプロキシに対応する設定を使用した。
検証結果はこの環境でのものであり、他のブラウザーでの描画結果までは保証しない。
