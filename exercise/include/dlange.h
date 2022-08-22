#ifndef DLANGE_H
#define DLANGE_H

/* http://www.netlib.org/lapack/explore-html/dc/d09/dlange_8f.html */
double dlange_(const char* NORM, const int* M, const int* N, const double* A, const int* LDA, double* WORK);

#endif // DLANGE_H