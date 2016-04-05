default: exercise-0.pdf

exercise-0.pdf: exercise-0.tex
	platex -shell-escape exercise-0.tex
	dvipdfmx exercise-0.dvi

exercise-1.pdf: exercise-1.tex
	platex -shell-escape exercise-1.tex
	dvipdfmx exercise-1.dvi

exercise-2.pdf: exercise-2.tex
	platex -shell-escape exercise-2.tex
	dvipdfmx exercise-2.dvi

exercise-3.pdf: exercise-3.tex
	platex -shell-escape exercise-3.tex
	dvipdfmx exercise-3.dvi

exercise-4.pdf: exercise-4.tex
	platex -shell-escape exercise-4.tex
	dvipdfmx exercise-4.dvi

exercise-5.pdf: exercise-5.tex
	platex -shell-escape exercise-5.tex
	dvipdfmx exercise-5.dvi

exercise-6.pdf: exercise-6.tex
	platex -shell-escape exercise-6.tex
	dvipdfmx exercise-6.dvi

clean:
	rm -f *.aux *.dvi *.log *.toc *.fdb_latexmk *.fls

distclean: clean
	rm -f exercise-?.pdf
