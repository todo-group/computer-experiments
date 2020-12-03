ZIP = example-1-8.zip example-2-2.zip

EXAMPLE14 = \
	monte_carlo/mersenne_twister.h monte_carlo/random.c \
	basics/complex.c \
	matrix/cmatrix.h matrix/pointer.c matrix/pointer-matrix.c \
	matrix/dgemm.h matrix/multiply.c matrix/multiply_dgemm.c \
	matrix/matrix_example.c \
	matrix/dlange.h matrix/dlange.c \
	matrix/zlange.h matrix/zlange.c \
	linear_system/dgesv.h linear_system/dgesv.c

EXAMPLE15 = $(EXAMPLE14) \
	linear_system/gauss.c \
	linear_system/dgetrf.h linear_system/dgetrs.h linear_system/lu_decomp.c \
	linear_system/input1.dat linear_system/input2.dat

EXAMPLE16 = $(EXAMPLE15) \
	eigenvalue_problem/diag.c eigenvalue_problem/dsyev.h eigenvalue_problem/matrix1.dat

EXAMPLE18 = $(EXAMPLE16) \
	svd/dgesvd.h svd/svd.c svd/full_svd.c svd/matrix2.dat svd/matrix3.dat linear_regression/measurement1.dat linear_regression/measurement2.dat \
	../misc/convert2matrix.py

EXAMPLE21 = matrix/cmatrix.h \
	monte_carlo/mersenne_twister.h monte_carlo/random.c \
	monte_carlo/histogram.c
EXAMPLE22 = $(EXAMPLE21) \
	monte_carlo/square_lattice.c monte_carlo/harmonic.c
EXAMPLE27 = $(EXAMPLE22) \
	optimization/golden_section.c optimization/func_1d.h \
	optimization/measurement3.dat \
	linear_system/poisson.h linear_system/poisson_dense.c linear_system/poisson_sparse.c linear_system/poisson_lu.c linear_system/dgetrf.h linear_system/dgetrs.h \
	matrix/cmatrix.h

.SUFFIXES: .pdf .dvi .tex

default: $(PDF) $(ZIP)

.tex.dvi:
	TEXINPUTS=.:./style//: platex -shell-escape $<
	TEXINPUTS=.:./style//: platex $< > /dev/null
	TEXINPUTS=.:./style//: platex $< > /dev/null
.dvi.pdf:
	TEXINPUTS=.:./style//: dvipdfmx -I 24 $< > /dev/null

example-1-4.zip : $(EXAMPLE14)
	rm -rf example-1-4
	mkdir example-1-4
	cp -p $(EXAMPLE14) example-1-4
	echo "CFLAGS = -Wall -O3 -I." > example-1-4/Makefile
	echo "LDLIBS = -llapack -lblas" >> example-1-4/Makefile
	zip example-1-4.zip example-1-4/*

example-1-5.zip : $(EXAMPLE15)
	rm -rf example-1-5
	mkdir example-1-5
	cp -p $(EXAMPLE15) example-1-5
	echo "CFLAGS = -Wall -O3 -I." > example-1-5/Makefile
	echo "LDLIBS = -llapack -lblas" >> example-1-5/Makefile
	zip example-1-5.zip example-1-5/*

example-1-6.zip : $(EXAMPLE16)
	rm -rf example-1-6
	mkdir example-1-6
	cp -p $(EXAMPLE16) example-1-6
	echo "CFLAGS = -Wall -O3 -I." > example-1-6/Makefile
	echo "LDLIBS = -llapack -lblas" >> example-1-6/Makefile
	zip example-1-6.zip example-1-6/*

example-1-8.zip : $(EXAMPLE18)
	rm -rf example-1-8
	mkdir example-1-8
	cp -p $(EXAMPLE18) example-1-8
	echo "CFLAGS = -Wall -O3 -I." > example-1-8/Makefile
	echo "LDLIBS = -llapack -lblas" >> example-1-8/Makefile
	zip example-1-8.zip example-1-8/*

example-2-2.zip : $(EXAMPLE22)
	rm -rf example-2-2
	mkdir example-2-2
	cp -p $(EXAMPLE22) example-2-2
	echo "CFLAGS = -Wall -O3 -I." > example-2-2/Makefile
	echo "LDLIBS = -llapack -lblas" >> example-2-2/Makefile
	zip example-2-2.zip example-2-2/*

example-2-7.zip : $(EXAMPLE27)
	rm -rf example-2-7
	mkdir example-2-7
	cp -p $(EXAMPLE27) example-2-7
	echo "CFLAGS = -Wall -O3 -I." > example-2-7/Makefile
	echo "LDLIBS = -llapack -lblas" >> example-2-7/Makefile
	zip example-2-7.zip example-2-7/*

clean:
	rm -f *.aux *.dvi *.log *.nav *.out *.snm *.toc *.vrb
	rm -rf auto *.xbb ./image/*.xbb ./style/*.xbb
	rm -rf example-?-L?

distclean: clean
	rm -f $(PDF) $(ZIP)
