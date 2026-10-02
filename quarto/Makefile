QUARTO ?= quarto
NODE ?= node

CHROME ?= /Applications/Google Chrome.app/Contents/MacOS/Google Chrome
PDF_TIMEOUT_SECONDS ?= 120

QMD := $(wildcard *.qmd)
HTML := $(QMD:%.qmd=%.html)
PDF := $(QMD:%.qmd=%.pdf)
ASSETS := $(wildcard assets/image/*)

.PHONY: all html pdf clean preview
.DELETE_ON_ERROR:

all: html pdf

# --------------------------------
# HTML / reveal.js
# --------------------------------

html: $(HTML)

%.html: %.qmd _quarto.yml $(ASSETS) Makefile
	$(QUARTO) render "$<" --to revealjs

# --------------------------------
# PDF
# --------------------------------

pdf: $(PDF)

%.pdf: %.html ../tools/print_reveal_pdf.mjs Makefile
	@echo "Printing $@"
	@"$(NODE)" ../tools/print_reveal_pdf.mjs "$(CHROME)" "$<" "$@" "$(PDF_TIMEOUT_SECONDS)"

# --------------------------------
# Preview
# --------------------------------

preview:
	$(QUARTO) preview

# --------------------------------
# Clean
# --------------------------------

clean:
	rm -rf $(HTML) $(PDF) _quarto.yml.cache _quarto.yml.lock *_files
