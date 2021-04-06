ZIP = samples-1.zip samples-2.zip

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

samples-1.zip : $(EXAMPLE18)
	rm -rf samples-1
	mkdir samples-1
	cp -p $(EXAMPLE18) samples-1
	echo "CFLAGS = -Wall -O3 -I." > samples-1/Makefile
	echo "LDLIBS = -llapack -lblas" >> samples-1/Makefile
	zip samples-1.zip samples-1/*

samples-2-2.zip : $(EXAMPLE27)
	rm -rf samples-2
	mkdir samples-2
	cp -p $(EXAMPLE27) samples-2
	echo "CFLAGS = -Wall -O3 -I." > samples-2/Makefile
	echo "LDLIBS = -llapack -lblas" >> samples-2/Makefile
	zip samples-2.zip samples-2/*

clean:
	rm -f *.aux *.dvi *.log *.nav *.out *.snm *.toc *.vrb
	rm -rf auto *.xbb ./image/*.xbb ./style/*.xbb
	rm -rf example-?-L?

distclean: clean
	rm -f $(PDF) $(ZIP)
