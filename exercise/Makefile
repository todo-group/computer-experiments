PDF = exercise-0.pdf exercise-1.pdf exercise-2.pdf exercise-3.pdf exercise-4.pdf exercise-2-1.pdf
ZIP = example-2-L1.zip

EXAMPLE2L1 = basics/bisection.c eigenvalue_problem/double_well.c linear_system/determinant.c linear_system/lu_decomp.c linear_system/matrix_util.h linear_system/input1.dat linear_system/input2.dat

.SUFFIXES: .pdf .dvi .tex

default: $(PDF) $(ZIP)

.tex.dvi:
	TEXINPUTS=.:./style//: platex -shell-escape $<
	TEXINPUTS=.:./style//: platex $<
	TEXINPUTS=.:./style//: platex $<
.dvi.pdf:
	TEXINPUTS=.:./style//: dvipdfmx -I 24 $<

example-2-L1.zip : $(EXAMPLE2L1)
	rm -rf example-2-L1
	mkdir example-2-L1
	cp -p $(EXAMPLE2L1) example-2-L1
	zip example-2-L1.zip example-2-L1/*

clean:
	rm -f *.aux *.dvi *.log *.nav *.out *.snm *.toc *.vrb
	rm -rf auto *.xbb ./image/*.xbb ./style/*.xbb
	rm -rf example-?-L?

distclean: clean
	rm -f $(PDF) $(ZIP)
