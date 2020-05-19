/* http://www.netlib.org/lapack/explore-html/d7/d7c/clange_8f.html */

#include <complex.h>
#ifdef I
# undef I
#endif

float clange_(char *NORM, int *M, int *N, float complex *A, int *LDA, float *WORK);
