PDF = main.pdf vcs.pdf

default: $(PDF)

main.pdf: main.tex
	latexmk main.tex

vcs.pdf: vcs.tex
	latexmk vcs.tex

clean:
	rm -f *.aux *.dvi *.log *.toc *.xbb *.fdb_latexmk *.fls

distclean: clean
	rm -f $(PDF)
