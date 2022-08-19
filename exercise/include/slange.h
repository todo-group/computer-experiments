#ifndef SLANGE_H
#define SLANGE_H

/* http://www.netlib.org/lapack/explore-html/dd/da4/slange_8f.html */
float slange_(const char* NORM, const int* M, const int* N, const float* A, const int* LDA, float* WORK);

#endif // SLANGE_H