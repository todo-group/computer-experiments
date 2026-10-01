# Beamer から Quarto + Reveal.js への変換設計

## 1. 目的と設計方針

`lecture/` 以下の Beamer 資産を、編集可能な Quarto (`.qmd`) として段階的に移行する。
変換結果を一度生成して終わりにするのではなく、移行中は同じ入力から同じ結果を再生成できる
**決定的な変換器**とする。

完全自動変換を装わず、次の三つを成果物とする。

1. Reveal.js で表示できる `.qmd`
2. コピーまたは変換した画像などのアセット
3. 自動変換できなかった箇所を、入力ファイルと行番号付きで示す JSON レポート

原稿を黙って捨てたり見た目を推測したりしない。意味を保てない構文は、原文を
`raw-latex` ブロックに残して警告する。この方針なら自動変換率を上げながら、人手確認の範囲を
レポートで管理できる。

## 2. リポジトリ調査結果

2026-10-01 時点の `lecture/` を静的に数えた結果は次の通りである。

| 対象 | 件数 | 移行上の意味 |
|---|---:|---|
| `.tex` | 355 | 入口と部品を区別する必要がある |
| `\documentclass` を持つ入口 | 27 | 原則として 1 入口を 1 `.qmd` にする |
| 部品 `.tex` | 328 | 単独変換せず `\input` 時に展開する |
| `\input` | 641 | 再帰展開、循環検出、出典位置の保持が必要 |
| `frame` | 415 | Reveal.js の第2レベル見出しへ変換する |
| `section` | 124 | Reveal.js の第1レベル見出しへ変換する |
| `itemize` / `enumerate` | 680 / 7 | Markdown リストへ変換可能 |
| `align*` | 125 | MathJax の display math として保持する |
| `lstlisting` | 39 | fenced code block へ変換する |
| `includegraphics` | 74 | パス解決と PDF/EPS 対応が必要 |
| `resizebox` | 71 | 幅・高さを Reveal.js 属性へ近似する |
| `columns` / `tabular` | 2 / 6 | 専用変換または要手動確認とする |

アニメーション用の `\pause`、`\only`、`\uncover`、`\onslide` は現在 0 件なので、初版の
必須範囲から外せる。画像ディレクトリには PDF 62 件、EPS 4 件のほか、図の生成元である
gnuplot 9 件、データ 8 件、Python 1 件がある。ブラウザー互換性のため PDF/EPS は SVG または
PNG に変換する必要がある。

`appendix.tex` には存在しない入力が 6 件ある。この入口は strict モードでは失敗させ、
通常モードでは欠落位置をレポートに残す。なお、トップレベルには総集編 (`1_basics.tex` など)、
開催回別 (`lecture-1-1.tex` など)、小規模な個別資料 (`matrix.tex` など) が混在するため、
ファイル名から用途を推測して統合しない。

## 3. 提案するコマンドライン

スクリプトは Python 標準ライブラリだけを使う `tools/beamer_to_quarto.py` として実装する。
TeX の構造解析と、この資料群で使われている主要なインライン命令の変換を自前で行う。
未対応命令は消去せず LaTeX のまま残すため、後から Pandoc フィルターなどへ移行できる。

```console
# 入口を一つ変換
python tools/beamer_to_quarto.py \
  lecture/lecture-1-1.tex --output-dir quarto

# 全入口を検出して変換（CI では strict を使用）
python tools/beamer_to_quarto.py lecture --all-masters \
  --output-dir quarto --report quarto/conversion-report.json --strict

# ファイルを作らず、入力グラフと未対応構文だけを調査
python tools/beamer_to_quarto.py lecture --all-masters --check
```

オプションは以下に固定する。

| オプション | 動作 |
|---|---|
| `INPUT` | 入口 `.tex`、または `--all-masters` と組み合わせるディレクトリ |
| `--output-dir DIR` | 出力ルート。入力ツリーを保って `.qmd` を配置 |
| `--all-masters` | `\documentclass` のある `.tex` のみを入口として検出 |
| `--assets copy\|symlink` | 既定は `copy`。出力を自己完結させる |
| `--report FILE` | 診断と変換統計を JSON で保存 |
| `--check` | 解析と診断のみ。出力を書かない |
| `--strict` | error 診断、欠落 input、変換不能アセットがあれば非 0 終了 |

入力を上書きする `--in-place` は設けない。出力は一時ディレクトリに生成し、全工程が成功した
後に rename する。これにより途中失敗で前回の正常な成果物を壊さない。

## 4. 変換パイプライン

### Phase 0: 事前検査

1. UTF-8 として全入力を読む（不正バイトは error）。
2. レンダリング検証を行う場合は `quarto` のバージョンを記録する。
3. 入口ごとに出力パスを予約し、衝突を検出する。

Quarto 自体はレンダリング時だけ必要とし、`--check` では不要とする。外部コマンドは
`shell=True` を使わず引数配列で実行する。

### Phase 1: 語彙を壊さない字句解析

正規表現だけで TeX 全体を変換しない。最低限、次の状態を持つ scanner を実装する。

* 通常テキスト、コメント、コマンド、波括弧グループ、環境
* `$...$`、`\[...\]`、数式環境
* `lstlisting` / `verbatim`（中身ではコマンドを解釈しない）

各 token に `SourceSpan(path, start_line, start_column, end_line, end_column)` を付ける。
診断と source map はこの位置を使い、展開後の行番号を入力位置として誤報しない。

### Phase 2: `\input` の再帰展開

TeX の実行時カレントディレクトリに合わせ、まず入口ファイルの親（このリポジトリでは
`lecture/`）から解決し、見つからない場合だけ参照元ファイルの親を試す。拡張子なしなら
`.tex` を補う。

展開器は include stack を持って循環を検出する。同じ部品を複数回読むこと自体は許可し、
グローバルな visited set で省略してはならない。コメント中、コード中、数式中の `\input` は
展開しない。欠落 input は placeholder と error 診断に変換する。

### Phase 3: 中間表現 (IR)

Pandoc に文書全体を直接渡す前に、スライド固有構造を次の IR にする。

```text
Deck(metadata, blocks, source)
  Section(title, children, source)
  Slide(title?, options, blocks, source)
  List(ordered, items, source)
  CodeBlock(language?, text, source)
  Math(display, text, source)
  Image(path, width?, height?, source)
  RawLatex(text, reason, source)
```

`frame` のオプション `t` は既定レイアウトで吸収し、`fragile` はコードブロックが正しく抽出
できた時だけ除去する。タイトルなし frame、同名 frame、空 section は合法として扱う。

### Phase 4: 本文変換

構造を確定してから、各 paragraph/list item のリンク、強調、等幅文字など、意味が一意な
インライン LaTeX を Markdown に変換する。数式命令と未対応命令はそのまま残し、Quarto の
MathJax または raw LaTeX として扱えるようにする。

主な規則は次の通り。

| Beamer / LaTeX | Quarto / Reveal.js |
|---|---|
| `\title`, `\author`, `\date` | YAML front matter |
| `\section{題}` | `# 題` |
| `\begin{frame}...{題}` | `## 題` |
| `itemize`, `enumerate` | `-`、`1.` のネストリスト |
| `$...$`, `\[...\]`, `align*` | MathJax 対応の数式として内容を保持 |
| `lstlisting` | 言語付き fenced code（既定言語は C） |
| `\href{url}{label}`, `\url{url}` | Markdown link |
| `\includegraphics` | Markdown image と幅/高さ属性 |
| `columns` | Reveal.js の `.columns` / `.column` div |
| `\bf`, `\tt`, `\rm` | 対応範囲では strong/code、曖昧なら raw + warning |
| `\vspace`, `\hspace`, `\setlength` | 原則削除し info 診断。意味のある配置は要確認 |
| `\tableofcontents` | `toc: true`。タイトル frame 内の命令自体は削除 |

`\resizebox` は子が画像だけの場合に限って寸法を画像属性へ移す。数式や表を包む場合は
RawLatex と warning にする。`\color`、負の余白、絶対配置があるスライドは visual-review
フラグを付ける。

### Phase 5: アセット処理

TeX と異なりブラウザーは PDF/EPS 画像を安定して表示できないので、以下の順で処理する。

1. `\includegraphics` の拡張子省略を TeX と同様の候補順で解決する。
2. SVG/PNG/JPEG はコピーする。
3. PDF は `pdftocairo -svg`、EPS は `epstopdf` の後 `pdftocairo -svg` で変換する。
4. 複数ページ PDF は既定で第1ページを使い warning を出す。
5. 内容 SHA-256 を manifest に記録し、同じ入力・設定なら再変換しない。

生成元 `.gp` / `.dat` / `.py` は参照画像と一緒にコピーするが、自動実行はしない。任意コードを
変換中に実行しないためである。

### Phase 6: 出力と検証

各 `.qmd` は共通 front matter を持つ。

```yaml
---
title: "計算機実験I (第1回)"
author: "藤堂眞治"
date: "2026-04-08"
format:
  revealjs:
    theme: simple
    slide-number: c/t
    hash: true
    transition: none
lang: ja
---
```

変換後に以下を自動検証する。

* 入力 frame 数と出力 `Slide` 数が一致する。
* section、リスト項目、コードブロック、画像、数式の前後件数をレポートする。
* 全画像リンクの存在を確認する。
* `quarto render` が成功する。
* HTML 内のスライド数を確認し、Playwright 等で代表入口のスクリーンショットを保存する。

HTML の見た目の完全一致を自動成功条件にはしない。Antibes/rose 固有の装飾、フォント、余白は
CSS の別作業として扱い、visual-review フラグがあるスライドを人が確認する。

## 5. 診断レポートの契約

JSON は将来の CI でも読めるよう `schema_version` を持たせる。

```json
{
  "schema_version": 1,
  "tool_version": "0.1.0",
  "decks": [{
    "input": "lecture/lecture-1-1.tex",
    "output": "quarto/lecture-1-1.qmd",
    "counts": {"frames_in": 33, "slides_out": 33},
    "diagnostics": [{
      "severity": "warning",
      "code": "layout-negative-space",
      "message": "negative vertical space requires visual review",
      "path": "lecture/1_basics/diff-10.tex",
      "line": 12,
      "column": 5
    }]
  }]
}
```

診断コードは文字列として安定させる。severity は `info`（安全に正規化）、`warning`（出力は
可能だが確認が必要）、`error`（内容欠落または出力不能）の三段階とする。`--strict` は error
だけで失敗し、warning は CI artifact として可視化する。

## 6. 実装単位とテスト戦略

一つの巨大な置換関数にせず、次のモジュール境界にする。

```text
tools/beamer_to_quarto.py       CLI と orchestration
tools/beamer_quarto/scanner.py  token と SourceSpan
tools/beamer_quarto/includes.py input graph と展開
tools/beamer_quarto/parser.py   token から IR
tools/beamer_quarto/convert.py  本文、数式、コード、画像規則
tools/beamer_quarto/render.py   YAML/QMD/manifest の出力
tools/beamer_quarto/report.py   診断 schema と JSON
tests/fixtures/beamer/          小さな入力と期待する .qmd/.json
```

テストは次の順で追加する。

1. scanner の brace、コメント、verbatim、複数行 frame title の単体テスト
2. include の拡張子補完、重複展開、欠落、循環の単体テスト
3. リスト、数式、listing、画像、columns の golden test
4. `lecture/lecture-1-1.tex` の変換 smoke test
5. 27 入口の `--check --strict`（既知の欠落は fixture 化するか明示的 allowlist）
6. Quarto render と代表スライドの visual regression

golden file の一括更新には専用の `UPDATE_GOLDEN=1` を要求し、通常テストで期待値を自動更新
しない。

## 7. 段階的な導入計画

1. **監査版**: scanner、include graph、JSON 診断、`--check` を先に実装する。
2. **MVP**: section/frame、リスト、数式、listing、リンクを変換し、画像はコピーする。
3. **画像版**: PDF/EPS 変換、寸法、manifest/cache を追加する。
4. **例外対応**: columns、tabular、古い宣言型書式 (`\bf` 等) を実データ順に対応する。
5. **表示調整**: 共通 SCSS と visual regression を追加する。
6. **切替**: 全入口の strict/render が通り、人手確認後に Quarto を正本にする。

最初の受け入れ基準は `lecture-1-1.tex` について frame 数、コード、画像、数式が欠落せず、
`quarto render` が成功することとする。全資料を一括で書き換えるより、代表資料で診断契約と
IR を固めてから対象を広げる方が、原稿の静かな欠落を防げる。
