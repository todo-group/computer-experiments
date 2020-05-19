/* http://www.netlib.org/lapack/explore-html/d7/d2b/dgemm_8f.html */

void dgemm_(char *TRANSA, char *TRANSB, int *M, int *N, int *K,
            double *ALPHA, double *A, int *LDA, double *B, int *LDB,
            double *BETA, double *C, int *LDC);
