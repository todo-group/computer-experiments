#ifndef CLANGE_H
#define CLANGE_H

#include <complex.h>
#ifdef I
#undef I
#endif

/* http://www.netlib.org/lapack/explore-html/d7/d7c/clange_8f.html */
float clange_(const char* NORM, const int* M, const int* N, const float complex* A, const int* LDA, float* WORK);

#endif // CLANGE_H