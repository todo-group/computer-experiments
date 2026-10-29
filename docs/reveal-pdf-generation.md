# Reveal.js からの PDF 生成と空白ページの対策

## 生成方法

QMD → Quarto / Reveal.js → Chrome の `Page.printToPDF` という経路を使用する。
Beamer への変換は行わない。既存の `lecture/*.tex` と講義本文の QMD は変更しない。

```sh
cd quarto
make lecture-2-2.pdf
# Chrome の場所が異なる場合
make lecture-2-2.pdf CHROME=/path/to/chrome
```

必要な実行環境は Quarto、Node.js、Chrome/Chromium。
MathJax と Web フォントは外部から取得するため、ネットワーク接続が必要。
切断・読み込み失敗時はエラーで終了し、既存の PDF を置き換えない。

## 確認できた原因

2026-10-28 に `master` の `8c35459` を調査した。
`quarto/Makefile` はすでに Reveal.js 方式で、確認した PR #5 の先端にも
Beamer PDF への切替は含まれていなかった。

旧スクリプトで講義2を出力すると、DOM 上は53スライドでも、PDFは59ページになり、
「エバルト法」「ツリー法」「温度の制御」などに見出しのみのページが生じた。
数式が読み込めない条件では、未処理の TeX を含む PDF も成功扱いで保存された。

主な問題は二つある。

1. **箇条書きの改ページとクリッピング。** Reveal.js のリストは `inline-block`。
   ページの残り高さに収まらないリストは、Chrome の印刷で全体が次ページに移る。
   スライドを包む `.pdf-page` の `overflow: hidden` と組み合わさり、
   本文が消えたり、見出しだけを残して本文が次ページへ送られたりする。
   `ul` / `ol` を印刷時に `display: block` にすると、この症状は解消したが、
   長い数式のはみ出しや、本文だけの継続ページは残った。
2. **数式処理とページ配置の順序。** `?print-pdf` で最初から開くと、
   Reveal.js が数式・フォントの描画完了前にページの高さを確定し得る。
   その後に `MathJax.startup.promise` を待っても、確定済みの高さは再計算されない。
   加えて旧設定は MathJax 4 を Reveal の MathJax 2 プラグイン経由で読み込み、
   実際に `MathJax.Hub.Config` の実行時エラーが発生した。

したがって、固定の待ち時間を長くするだけでは十分ではない。

## 修正内容

- `tools/mathjax-tex.lua` で数式の TeX を保持し、直接読み込む MathJax 4.1.3 に渡す。
  `aligned`、`\textcolor`、`\rm` を Pandoc の MathML 変換に通さず、変換警告を防ぐ。
  HTML の特殊文字をエスケープし、インライン数式・独立数式の区切りを付ける。
  フィルターは Reveal.js 出力にだけ適用し、Makefile の再生成依存にも加える。
  `html-math-method: plain` は旧 MathJax プラグインを無効にするための設定であり、
  **この設定だけでは不十分**。TeX の変換を避けるため上記フィルターも必要。
  MathJax とフォントの版を固定する。
  `slide-number: true` は Quarto の既定の `c/t` 表示を使う。
- 印刷ビューのリストを `display: block` にする。通常のスライド表示には適用しない。
- `tools/print_reveal_pdf.mjs` は、通常ビューで全画像を読み込み、
  `MathJax.startup.promise`、`typesetPromise()`、`document.fonts.ready` の順に待つ。
  画像は遅延読み込みを解除し、`decode()` の完了を待つ。
- 描画済み DOM と設定を保ったまま Reveal を破棄・再初期化し、印刷ビューを作る。
  Reveal 5 の `configure({view: 'print'})` だけでは印刷ビューは起動しない。
  再初期化前に `pdf-ready` を購読して、全スライドのページ配置完了を待つ。
- 各スライドの本文・画像・数式の表示範囲を測り、はみ出すものだけ
  **PDF上でスライド全体を縮小**して1スライド1ページにする。
  改ページ計算に反映される CSS `zoom` を使い、縮小した番号と倍率を出力する。
  上限ページ数だけを1に設定して本文を切り捨てる方式ではない。
- 画像欠落、数式エラー、JavaScript エラー、必要なリソースの取得失敗は非0終了にする。
  出力は従来どおり一時ファイルからの置換とし、失敗時に既存PDFを壊さない。

この縮小処理は `make pdf` の自動出力に適用される。
ブラウザーからの手動印刷ではリストのCSS修正だけが適用され、長いスライドが
複数ページになることがある。配布用PDFには `make` を使う。
縮小率が小さすぎるスライドは、必要に応じて原稿側で分割する。

## 検証

Quarto 1.8.27（同梱 Reveal.js）、Chrome Headless Shell 142.0.7444.175、Linuxで実施。
Mac版Chrome固有の挙動は未検証。

- 既存の変換器テスト16件、TeX保持のテスト5件、PDF生成の統合テスト5件が成功。
- 講義1・2を `quarto render ... --to revealjs --fail-if-warnings` で生成し、警告なしで成功。
  TeX保持のテストでも実際の Pandoc を使い、複数行・色指定・旧書式命令・HTML特殊文字と、
  Reveal.js 以外の出力を変更しないことを確認。
- 統合テストでは、1.2秒遅れて描画される数式、長いリスト、遅延読み込み画像を含む
  3スライドが3ページになり、末尾の項目までPDFのテキストに残ることを確認。
- 画像欠落時の失敗と既存PDFの保持、MathJax未読込、スクリプト例外、不正なタイムアウトも検証。
- 講義2の全53ページを画像化して確認。本文が欠けるページ、白紙、ページ外のテキストはなし。
  スライド9・11・25・27・30・31・35・44・48・50を縮小し、最小倍率は約94%。
- 講義1も51ページで出力し、全ページの描画とページ外のテキストがないことを確認。

統合テストには、Chrome、Reveal.js の `dist` ディレクトリ、Poppler の `pdftotext` が必要。
環境変数未指定時はブラウザーを必要とする4件をスキップする。
TeX保持の5件は Pandoc が必要。PATH 上にない場合は `PANDOC` で指定する。

```sh
PANDOC=/path/to/pandoc \
python3 -m pytest tests/test_beamer_to_quarto.py tests/test_mathjax_tex.py -q
CHROME=/path/to/chrome \
REVEAL_DIST=/path/to/quarto/share/formats/revealjs/reveal/dist \
node --test tests/test_print_reveal_pdf.mjs
```

## 参考

- [Reveal.js PDF Export](https://revealjs.com/pdf-export/): 印刷ビュー、ページ寸法、長いスライドの扱い。
- [Quarto: Presenting Slides](https://quarto.org/docs/presentations/revealjs/presenting.html#print-to-pdf): 手動印刷とPDF設定。
- [MathJax: Typesetting Mathematics](https://docs.mathjax.org/en/latest/web/typeset.html): 非同期処理とフォント読み込みの完了待ち。
- [Pandoc: Math rendering in HTML](https://pandoc.org/MANUAL.html#math-rendering-in-html): HTML向けの数式出力方法。
