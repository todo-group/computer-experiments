/* http://www.netlib.org/lapack/explore-html/d8/d2d/dgesvd_8f.html */
void dgesvd_(char *JOBU, char* JOBVT, int *M, int *N, double* A,
             int* LDA, double* S, double* U, int* LDU, double* VT, int* LDVT ,
             double* WORK, int* LWORK, int* INFO);
