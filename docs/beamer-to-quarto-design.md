# Beamer から Quarto + Reveal.js への変換

`lecture/` の Beamer 原稿を、`tools/beamer_to_quarto.py` で編集可能な
Quarto スライドへ変換する。この文書は現在の実装と運用をまとめる。

## 使い方

変換器は Python 3.10 以降と標準ライブラリを使用する。
PDF 図の変換には Poppler の `pdftocairo`、HTML の生成には Quarto が必要。

```sh
# 一つの講義を変換して HTML を生成
python3 tools/beamer_to_quarto.py lecture/lecture-2-1.tex \
  --output-dir quarto --report quarto/conversion-report.json --strict
quarto render quarto/lecture-2-1.qmd

# 全入口を検査（QMD・画像は書き出さず、レポートのみ保存）
python3 tools/beamer_to_quarto.py lecture --all-masters --check \
  --report conversion-report.json --strict
```

ディレクトリ入力には `--all-masters` が必要で、`\documentclass` を含む `.tex` を
入口として検出する。出力先の既定値は `quarto`。再変換は既存の `.qmd` を上書きする。
共通の表示設定は `quarto/_quarto.yml` に置き、変換器では生成・上書きしない。

## 変換規則

`\input` を再帰展開し、セクションとフレームを抽出して本文を Markdown に変換する。
入力の探索順は入口の親ディレクトリ、参照元の親ディレクトリ。拡張子なしは `.tex` を補う。
コメント中の入力・セクションを除外し、入力の欠落や循環は診断する。

| 原稿 | 変換結果 |
|---|---|
| `\title`・`\author`・`\date` | YAML メタデータ |
| `\section{題}` | `# 題`。コメントアウトされたセクションは除外 |
| `frame` | `## 題`。タイトルがない通常フレームは「無題」 |
| `\titlepage` と `\tableofcontents` だけのフレーム | 省略し、Quarto のタイトル・目次スライドを使用 |
| `\tableofcontents` | `toc: true`、`toc-depth: 1`、`toc-title: "目次"` |
| `itemize`・`enumerate` | 入れ子を保持した Markdown リスト |
| `$...$`・`\(...\)` | インライン数式 |
| `\[...\]`・`$$...$$`・数式環境 | `$$` で囲んだ数式。単独の `align` / `align*` は内部を `aligned` に変換 |
| `lstlisting` | 言語付きコードブロック。言語指定がなければ C |
| `verbatim`・`verbatim*` | `.verbatim` クラス付きコードブロック |
| `\textbf`・`\texttt`・`\emph` | 太字・等幅・強調 |
| `\href`・`\url` | Markdown リンク |
| `{\color{gray} 内容}`・`{\color{red} 内容}` | `[内容]{.gray}`・`[内容]{.red}`。入れ子の書式も保持 |
| 単純な `tabular`（`l`・`c`・`r` 列） | 第1行を見出しにした Markdown 表。列の配置とセル内の数式を保持 |
| `\includegraphics` | `assets/` 内の画像を参照する Markdown 画像 |
| 画像だけを包む `\resizebox` | 包装を除去。幅の `\textwidth` 比を %、高さの `\textheight` 比を 700px 基準の値に変換 |
| 本文の `\\`・`\\[2em]` 等 | 改行に変換し、余白指定を削除 |
| `\vspace`・`\hspace`・`\setlength`・`\noindent`・`\hfill`、`center`・`small` の環境タグ | レイアウト指定を削除 |

数式を本文処理前に保護して、`cases` や `split` に余分な `$$` が付くことを防ぐ。
コードは最後に復元し、字下げ・空行・`%`・`$`・LaTeX に似た文字列を保持する。
通常本文の原稿の字下げは除去し、意図しないコードブロック化を防ぐ。

表があり、末尾に独立した図が一つある本文は、表・説明を左 60%、図を右 40% の
`.columns` に配置する。Beamer の負の余白による配置を置き換え、図のはみ出しを防ぐ。

## 画像と出力

PDF は第1ページを `pdftocairo -svg` で SVG に変換する。
ほかの画像はコピーし、`--assets symlink` ならシンボリックリンクにする。
PDF はこのオプションでも SVG を生成する。EPS は自動変換せず、ブラウザー非対応の警告を出す。

画像の元パスと元ファイルの SHA-256 は `asset-manifest.json` に保存する。
QMD は一時ディレクトリから出力先へ移動するが、画像は直接出力先へ書き込む。
`--strict` は error 診断があると非 0 終了する。エラー時にも生成物は書き出される。

`--report` の JSON は、最上位に `schema_version`・`tool_version`・`decks`・
`diagnostics` を持つ。各 deck の `counts` は入力フレーム数 `frames_in` と出力本文スライド数
`slides_out`。タイトルフレームを省略すると両者は一致しない。
Quarto が追加するタイトル・目次・セクションスライドは `slides_out` に含めない。
診断は重要度・コード・メッセージ・パス・行番号を持ち、元原稿への厳密な位置追跡は未実装。

## 共通の表示設定

`quarto/_quarto.yml` で以下を指定する。

| 項目 | 現在の設定 |
|---|---|
| 本文・見出し | Noto Sans JP |
| 本文サイズ | `fontsize: 24px` |
| 数式 | MathJax 4 の STIX Two Math（`mathjax-stix2`） |
| `verbatim` のサイズ | `pre.verbatim` 等の CSS で `18px` |
| 色 | `.gray` は灰色、`.red` は赤色 |
| 目次 | 第1レベルのセクションのみ |

数式は Quarto の `html-math-method: mathml` と、直接読み込む MathJax 4 で表示する。
MathML に変換できなかった TeX も MathJax で処理する。
フォントと MathJax は外部サービスから読み込むため、表示にはインターネット接続が必要。

## 検証と制約

```sh
python3 -m pip install -r requirements.txt
python3 -m pytest tests/test_beamer_to_quarto.py -q
```

回帰テストは、目次・色・リスト・数式・表・PDF 図・コードの字下げと文字保持などを確認する。
変更後は代表資料 `lecture-2-1.tex` を変換・レンダリングし、図の読み込み、数式、表、
コード例、スライド内への収まりをブラウザーで確認する。変換器自体は Quarto やブラウザーを実行しない。

現状は正規表現と括弧・環境の対応付けによる限定的な変換で、汎用の TeX パーサーではない。
未対応の LaTeX は原文が残ることがあり、未対応構文を網羅する診断や raw-LaTeX ブロック化は未実装。
複雑な表、元原稿の `columns`、アニメーション、画像以外の `\resizebox`、絶対配置は手動調整が必要。
`\input` 展開はコード・数式内部を区別しない。`--check` は PDF 変換や HTML 描画を検証しない。
複数ページ PDF の全ページ変換、画像の変換キャッシュ、厳密な出典追跡は今後の対応範囲とする。
