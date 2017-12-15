PDF = lecture-1-1.pdf lecture-1-2.pdf lecture-1-3.pdf lecture-1-4.pdf \
	lecture-2-1.pdf lecture-2-2.pdf lecture-2-3.pdf lecture-2-4.pdf \
	appendix.pdf \
	0_about.pdf 1_basics.pdf 2_ode.pdf 3_linear_system.pdf \
	4_eigenvalue_problem.pdf 5_linear_regression.pdf 6_monte_carlo.pdf 7_optimization.pdf 8_hpc.pdf 9_vcs.pdf

.SUFFIXES: .pdf .dvi .tex

default: Makefile.depend $(PDF)

.tex.dvi:
	TEXINPUTS=.:./style//: platex -shell-escape $< > /dev/null
	TEXINPUTS=.:./style//: platex $< > /dev/null
	TEXINPUTS=.:./style//: platex $< > /dev/null
.dvi.pdf:
	TEXINPUTS=.:./style//: dvipdfmx -I 24 $< > /dev/null

clean:
	rm -f *.aux *.dvi *.log *.nav *.out *.snm *.toc *.vrb
	rm -rf auto *.xbb ./image/*.xbb ./style/*.xbb

distclean: clean
	rm -f $(PDF)

Makefile.depend: *.tex
	sh makedepend.sh > Makefile.depend

-include Makefile.depend
