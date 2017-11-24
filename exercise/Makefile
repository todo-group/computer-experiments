PDF = exercise-0.pdf exercise-1.pdf exercise-2.pdf exercise-3.pdf exercise-4.pdf \
	exercise-2-1.pdf exercise-2-2.pdf
ZIP = example-2-L1.zip example-2-L2.zip example-2-L3.zip

EXAMPLE2L1 = basics/bisection.c eigenvalue_problem/double_well.c linear_system/determinant.c linear_system/lu_decomp.c linear_system/matrix_util.h linear_system/input1.dat linear_system/input2.dat

EXAMPLE2L2 = monte_carlo/random.c monte_carlo/histogram.c \
	monte_carlo/harmonic.c monte_carlo/square_lattice.c \
	linear_system/matrix_util.h monte_carlo/mersenne_twister.h

EXAMPLE2L3 = monte_carlo/bond_table.c monte_carlo/exact_counting.c linear_system/matrix_util.h linear_system/mersenne_twister.h linear_system/multiply.c linear_system/multiply_dgemm.c

.SUFFIXES: .pdf .dvi .tex

default: $(PDF) $(ZIP)

.tex.dvi:
	TEXINPUTS=.:./style//: platex -shell-escape $< > /dev/null
	TEXINPUTS=.:./style//: platex $< > /dev/null
	TEXINPUTS=.:./style//: platex $< > /dev/null
.dvi.pdf:
	TEXINPUTS=.:./style//: dvipdfmx -I 24 $< > /dev/null

example-2-L1.zip : $(EXAMPLE2L1)
	rm -rf example-2-L1
	mkdir example-2-L1
	cp -p $(EXAMPLE2L1) example-2-L1
	zip example-2-L1.zip example-2-L1/*
	rm -rf example-2-L1

example-2-L2.zip : $(EXAMPLE2L2)
	rm -rf example-2-L2
	mkdir example-2-L2
	cp -p $(EXAMPLE2L2) example-2-L2
	zip example-2-L2.zip example-2-L2/*
	rm -rf example-2-L2

example-2-L3.zip : $(EXAMPLE2L3)
	rm -rf example-2-L3
	mkdir example-2-L3
	cp -p $(EXAMPLE2L3) example-2-L3
	zip example-2-L3.zip example-2-L3/*
	rm -rf example-2-L3

clean:
	rm -f *.aux *.dvi *.log *.nav *.out *.snm *.toc *.vrb
	rm -rf auto *.xbb ./image/*.xbb ./style/*.xbb
	rm -rf example-?-L?

distclean: clean
	rm -f $(PDF) $(ZIP)
