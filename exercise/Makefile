ZIP = example-1-L2.zip example-1-L3.zip example-1-EX4.zip \
	example-2-L1.zip example-2-L2.zip example-2-L3.zip example-2-EX3.zip example-2-L4.zip example-2-EX4.zip

EXAMPLE1L2 = matrix/cmatrix.h matrix/pointer.c matrix/pointer-matrix.c

EXAMPLE1L3 = linear_system/gauss.c linear_system/input1.dat linear_system/input2.dat linear_system/input3.dat \
	matrix/cmatrix.h matrix/pointer-matrix.c matrix/matrix_example.c matrix/vector.dat matrix/matrix.dat \
	matrix/dgemm.h matrix/multiply.c matrix/multiply_dgemm.c \
	linear_system/lu_decomp.c \
	monte_carlo/mersenne_twister.h

EXAMPLE1L4 = matrix/cmatrix.h \
	eigenvalue_problem/diag.c eigenvalue_problem/dsyev.h eigenvalue_problem/matrix1.dat \
	svd/svd.c svd/full_svd.c svd/dgesvd.h svd/matrix2.dat svd/matrix3.dat svd/matrix4.dat \
	linear_regression/measurement1.dat linear_regression/measurement2.dat

EXAMPLE2L1 = basics/bisection.c eigenvalue_problem/double_well.c eigenvalue_problem/dsyev.h eigenvalue_problem/diag.c matrix/cmatrix.h matrix/matrix_example.c matrix/matrix.dat matrix/vector.dat eigenvalue_problem/matrix1.dat linear_system/determinant.c linear_system/lu_decomp.c linear_system/input1.dat linear_system/input2.dat

EXAMPLE2L2 = monte_carlo/random.c monte_carlo/histogram.c \
	monte_carlo/harmonic.c monte_carlo/square_lattice.c \
	matrix/cmatrix.h monte_carlo/mersenne_twister.h

EXAMPLE2L3 = monte_carlo/bond_table.h monte_carlo/mersenne_twister.h monte_carlo/exact_counting.c matrix/cmatrix.h matrix/multiply.c matrix/multiply_dgemm.c matrix/dgemm.h eigenvalue_problem/dsyev.h eigenvalue_problem/diag.c eigenvalue_problem/matrix1.dat monte_carlo/transfer_matrix.c

EXAMPLE2EX3 = monte_carlo/random.c monte_carlo/histogram.c monte_carlo/bond_table.c monte_carlo/exact_counting.c matrix/cmatrix.h monte_carlo/mersenne_twister.h matrix/multiply.c matrix/multiply_dgemm.c monte_carlo/transfer_matrix.c

EXAMPLE2L4 = optimization/func_1d.h optimization/func_2d.h optimization/golden_section.c optimization/mc_steepest_descent_1d.c optimization/mc_steepest_descent_2d.c optimization/simulated_annealing_1d.c optimization/simulated_annealing_2d.c monte_carlo/mersenne_twister.h optimization/nelder_mead_2d.c linear_system/steepest_descent.c matrix/cmatrix.h linear_system/input3.dat optimization/measurement-3.dat linear_system/poisson.h linear_system/poisson_dense.c linear_system/poisson_lu.c linear_system/poisson_sparse.c linear_system/lu_decomp.c

.SUFFIXES: .pdf .dvi .tex

default: $(PDF) $(ZIP)

.tex.dvi:
	TEXINPUTS=.:./style//: platex -shell-escape $<
	TEXINPUTS=.:./style//: platex $< > /dev/null
	TEXINPUTS=.:./style//: platex $< > /dev/null
.dvi.pdf:
	TEXINPUTS=.:./style//: dvipdfmx -I 24 $< > /dev/null

example-1-L2.zip : $(EXAMPLE1L2)
	rm -rf example-1-L2
	mkdir example-1-L2
	cp -p $(EXAMPLE1L2) example-1-L2
	echo "default: pointer pointer-matrix" > example-1-L2/Makefile
	echo "CFLAGS = -O3 -I." >> example-1-L2/Makefile
	zip example-1-L2.zip example-1-L2/*
	rm -rf example-1-L2

example-1-L3.zip : $(EXAMPLE1L3)
	rm -rf example-1-L3
	mkdir example-1-L3
	cp -p $(EXAMPLE1L3) example-1-L3
	echo "CFLAGS = -O3" > example-1-L3/Makefile
	echo "LDLIBS = -llapack -lblas" >> example-1-L3/Makefile
	zip example-1-L3.zip example-1-L3/*
	rm -rf example-1-L3

example-1-L4.zip : $(EXAMPLE1L4)
	rm -rf example-1-L4
	mkdir example-1-L4
	cp -p $(EXAMPLE1L4) example-1-L4
	echo "CFLAGS = -O3" > example-1-L4/Makefile
	echo "LDLIBS = -llapack -lblas" >> example-1-L4/Makefile
	zip example-1-L4.zip example-1-L4/*
	rm -rf example-1-L4

example-2-L1.zip : $(EXAMPLE2L1)
	rm -rf example-2-L1
	mkdir example-2-L1
	cp -p $(EXAMPLE2L1) example-2-L1
	echo "CFLAGS = -O3" > example-2-L1/Makefile
	echo "LDLIBS = -llapack -lblas" >> example-2-L1/Makefile
	zip example-2-L1.zip example-2-L1/*
	rm -rf example-2-L1

example-2-L2.zip : $(EXAMPLE2L2)
	rm -rf example-2-L2
	mkdir example-2-L2
	cp -p $(EXAMPLE2L2) example-2-L2
	echo "CFLAGS = -O3" > example-2-L2/Makefile
	echo "LDLIBS =" >> example-2-L2/Makefile
	zip example-2-L2.zip example-2-L2/*
	rm -rf example-2-L2

example-2-L3.zip : $(EXAMPLE2L3)
	rm -rf example-2-L3
	mkdir example-2-L3
	cp -p $(EXAMPLE2L3) example-2-L3
	zip example-2-L3.zip example-2-L3/*
	rm -rf example-2-L3

example-2-EX3.zip : $(EXAMPLE2EX3)
	rm -rf example-2-EX3
	mkdir example-2-EX3
	cp -p $(EXAMPLE2EX3) example-2-EX3
	zip example-2-EX3.zip example-2-EX3/*
	rm -rf example-2-EX3

example-2-L4.zip : $(EXAMPLE2L4)
	rm -rf example-2-L4
	mkdir example-2-L4
	cp -p $(EXAMPLE2L4) example-2-L4
	zip example-2-L4.zip example-2-L4/*
	rm -rf example-2-L4

example-2-EX4.zip : $(EXAMPLE2EX4)
	rm -rf example-2-EX4
	mkdir example-2-EX4
	cp -p $(EXAMPLE2EX4) example-2-EX4
	zip example-2-EX4.zip example-2-EX4/*
	rm -rf example-2-EX4

clean:
	rm -f *.aux *.dvi *.log *.nav *.out *.snm *.toc *.vrb
	rm -rf auto *.xbb ./image/*.xbb ./style/*.xbb
	rm -rf example-?-L?

distclean: clean
	rm -f $(PDF) $(ZIP)
