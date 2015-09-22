default: main.pdf

main.pdf: *.tex
	platex -shell-escape main.tex
	platex main.tex
	platex main.tex
	dvipdfmx main.dvi

clean:
	rm -f *.aux *.dvi *.log *.toc *.xbb *.fdb_latexmk *.fls

distclean: clean
	rm -f main.pdf
