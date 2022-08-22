#ifndef DGESV_H
#define DGESV_H

/* http://www.netlib.org/lapack/explore-html/d8/d72/dgesv_8f.html */
void dgesv_(const int* N, const int* NRHS, double* A, const int* LDA, int* IPIV, double* B, const int* LDB, int* INFO);

#endif // DGESV_H