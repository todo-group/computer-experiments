/* http://www.netlib.org/lapack/explore-html/d5/d8f/zlange_8f.html */

#include <complex.h>
#ifdef I
# undef I
#endif

double zlange_(char *NORM, int *M, int *N, double complex *A, int *LDA, double *WORK);
