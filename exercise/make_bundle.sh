#!/bin/bash

mkdir samples-1
mkdir samples-2

# compile tex files
FNAME_TEX=$(find . -regex ".*.tex")

latexmk -C &>/dev/null
for fname in ${FNAME_TEX}; do
    latexmk ${fname} &>/dev/null
done
latexmk -c &>/dev/null
rm *.dvi *.synctex.gz

# zip sample codes
SRC1=(monte_carlo/random.c basics/complex.c matrix/pointer.c
    matrix/pointer-matrix.c matrix/multiply.c matrix/multiply_dgemm.c
    matrix/matrix_example.c matrix/dlange.c matrix/zlange.c
    linear_system/dgesv.c linear_system/gauss.c linear_system/lu_decomp.c
    eigenvalue_problem/diag.c svd/svd.c svd/full_svd.c)
AUX1=(linear_system/input1.dat linear_system/input2.dat eigenvalue_problem/matrix1.dat
    svd/matrix2.dat svd/matrix3.dat linear_regression/measurement1.dat linear_regression/measurement2.dat
    ../misc/convert2matrix.py)

cp ${SRC1[*]} samples-1
cp ${AUX1[*]} samples-1
for fname in ${SRC1[*]}; do
    INCLUDE=($(gcc -MM -I include ${fname} | cut -d ' ' -f 2- | sed -r 's/\\//g' | sed -r 's/ /\n/g' | grep -E '\.h'))
    for hname in ${INCLUDE[*]}; do
        cp ${hname} samples-1
    done
done
cp Makefile samples-1

SRC2=(monte_carlo/random.c monte_carlo/histogram.c monte_carlo/square_lattice.c
    monte_carlo/harmonic.c eigenvalue_problem/tridiagonal.c eigenvalue_problem/sparse.c
    eigenvalue_problem/matfree.c optimization/golden_section.c optimization/steepest_descent_2d.c
    optimization/nelder_mead_2d.c optimization/simulated_annealing_2d.c)
AUX2=(optimization/measurement3.dat)
INCLUDE2=($(cat ${SRC2[*]} | grep -E "#include \"" | sed -r "s/#include \"(.*)\"/\1/g" | sort -u))

cp ${SRC2[*]} samples-2
cp ${AUX2[*]} samples-2
for fname in ${SRC2[*]}; do
    INCLUDE=($(gcc -MM -I include ${fname} | cut -d ' ' -f 2- | sed -r 's/\\//g' | sed -r 's/ /\n/g' | grep -E '\.h'))
    for hname in ${INCLUDE[*]}; do
        cp ${hname} samples-2
    done
done
cp Makefile samples-2

rm samples-1.zip
rm samples-2.zip
zip samples-1.zip samples-1/*
zip samples-2.zip samples-2/*

rm -r samples-1
rm -r samples-2
