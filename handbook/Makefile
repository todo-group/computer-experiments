default: main.pdf

main.pdf:
	latexmk main.tex

clean:
	rm -f *.aux *.dvi *.log *.toc *.xbb *.fdb_latexmk *.fls

distclean: clean
	rm -f main.pdf
