SRC = style/*.sty

.SUFFIXES: .pdf .dvi .tex

PDF = 1_basics.pdf 2_ode.pdf 3_linear_system.pdf 4_eigenvalue_problem.pdf 5_linear_regression.pdf 6_monte_carlo.pdf 7_optimization.pdf 8_hpc.pdf 9_vcs.pdf

default: $(PDF)

.tex.dvi: $(SRC)
	TEXINPUTS=.:./style//: platex -shell-escape $<
	TEXINPUTS=.:./style//: platex $<
	TEXINPUTS=.:./style//: platex $<
.dvi.pdf:
	TEXINPUTS=.:./style//: dvipdfmx $<

clean:
	rm -f *.aux *.dvi *.log *.nav *.out *.snm *.toc *.vrb
	rm -rf auto *.xbb ./image/*.xbb ./style/*.xbb

distclean: clean
	rm -f $(PDF)
