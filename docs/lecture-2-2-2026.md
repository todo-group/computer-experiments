# 講義2：2025年版講義3のQuarto変換

`quarto/lecture-2-2.qmd` は `lecture/lecture-2-3.tex` とそこから参照される
フラグメントを展開した変換版。元の5セクション・本文44枚の内容と順序を維持する。
表紙・目次・セクション扉を含むReveal.js版は51画面。
既存のBeamer原稿・共有フラグメント・共通Quarto設定は変更しない。

## 最小限の変更

- 表紙を講義2「分子動力学と統計的サンプリング」、2026年10月23日に更新。
- 表、太字、数式の色指定と文字の記法をQuarto/MathJaxで表示できる形に変換。
- PDF図4点をSVGに変換。フローチャートの高さ、エバルト法の図の右配置、
  密なスライドの余白・文字サイズを調整。スライドの分割・削除・順序変更はしない。
- 元原稿の明らかな誤植2か所をQMD側だけで修正：
  リープ・フロッグ式の色指定の閉じ括弧、吉田法の係数
  `d_2 = -2^{1/3}/(2-2^{1/3})` の負号。
- 講義2だけMathMLとMathJax 4の直接読込みを使用し、Revealの旧MathJax APIとの
  不整合を回避。フォントとMathJaxの読込みにはネット接続が必要。

前回案の再構成、新しいBeamer入口、追加の数値実験図・確認課題は取り下げた。
本文の説明は原稿を維持しており、全面的な内容改訂ではない。

## 再変換・ビルド

```sh
# 手動調整済みのQMDを上書きしないよう、別ディレクトリに変換する
python3 tools/beamer_to_quarto.py lecture/lecture-2-3.tex \
  --output-dir /tmp/lecture-2-3-conversion \
  --report /tmp/lecture-2-3-conversion/report.json --strict
quarto render quarto/lecture-2-2.qmd
python3 -m pytest -q
```

変換器による結果は45フレームから本文44枚、診断0件。
変換後の見出しと順序を原稿と照合し、表示数式76ブロックを確認した。
元図のパスとSHA-256は `quarto/lecture-2-2-assets.json` に記録している。

検証結果（Quarto 1.8.27、Chrome 131）：

- 既存テスト16件成功。`git diff --check` 成功。
- 全51画面で数式226個の描画、図の読込み、表示領域への収まりを確認。
  JavaScriptエラー・MathJaxエラー・画像欠落・領域超過はいずれも0件。
- 色指定など一部のTeXはQuartoのMathML変換時に警告が出るが、
  MathJaxのTeXフォールバックで表示を確認済み。
- 元の5セクション・本文44枚の見出しと順序、表示数式76ブロックを照合。
  色指定の同値な書換えを除く数式の変更は、上記の誤植2か所と
  ビリアル比の文字と数式を分ける同値な表記変更のみ。
