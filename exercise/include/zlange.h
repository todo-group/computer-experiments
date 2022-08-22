#ifndef ZLANGE_H
#define ZLANGE_H

#include <complex.h>
#ifdef I
#undef I
#endif

/* http://www.netlib.org/lapack/explore-html/d5/d8f/zlange_8f.html */
double zlange_(const char* NORM, const int* M, const int* N, const double complex* A, const int* LDA, double* WORK);

#endif // ZLANGE_H