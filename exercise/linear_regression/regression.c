#include "cmatrix.h"
#include "dgetrf.h"
#include "dgetrs.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* Basis Functions f(i, x) = phi_i(x) */
double f(int i, double x)
{
    if (i == 0)
    {
        return 1.0;
    }
    else if (i == 1)
    {
        return x;
    }
    return 0;
}

void read_data(FILE* fp, int* n, double* x, double* y, double* yerror)
{
    *n = 0;
    double xin, yin, rin;
    while (fscanf(fp, "%lf %lf %lf", &xin, &yin, &rin) == 3)
    {
        x[*n] = xin;
        y[*n] = yin;
        yerror[*n] = rin;
        ++(*n);
    }
}

int main(int argc, char** argv)
{
    /* Maximum number of data points */
    const int maxdata = 100;
    /* Number of Basis Functions */
    const int nbase = 2;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s inputfile\n", argv[0]);
        exit(1);
    }

    /* read data from a file */
    FILE* fp = fopen(argv[1], "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Error: file can not open\n");
        exit(1);
    }
    int n;
    double x[maxdata];
    double y[maxdata];
    double yerror[maxdata];
    read_data(fp, &n, x, y, yerror);
    printf("x:\n");
    fprint_dvector(stdout, n, x);
    printf("y:\n");
    fprint_dvector(stdout, n, y);
    printf("yerror:\n");
    fprint_dvector(stdout, n, yerror);

    /* construct a = Phi^t Phi */
    double** a = alloc_dmatrix(nbase, nbase);
    for (int i = 0; i < nbase; ++i)
    {
        for (int j = 0; j < nbase; ++j)
        {
            mat_elem(a, i, j) = 0.0;
            for (int k = 0; k < n; ++k)
            {
                mat_elem(a, i, j) += f(i, x[k]) * f(j, x[k]);
            }
        }
    }
    printf("Phi^t Phi:\n");
    fprint_dmatrix(stdout, nbase, nbase, a);

    /* construct b = Phi^t y */
    double* b = alloc_dvector(nbase);
    for (int i = 0; i < nbase; ++i)
    {
        b[i] = 0.0;
        for (int k = 0; k < n; ++k)
        {
            b[i] += f(i, x[k]) * y[k];
        }
    }
    printf("Phi^t y:\n");
    fprint_dvector(stdout, nbase, b);

    /* perform LU decomposition */
    int* ipiv = alloc_ivector(nbase);
    int info;
    dgetrf_(&nbase, &nbase, mat_ptr(a), &nbase, vec_ptr(ipiv), &info);
    if (info != 0)
    {
        fprintf(stderr, "Error: LAPACK::dgetrf failed\n");
        exit(1);
    }
    printf("Result of LU decomposition:\n");
    fprint_dmatrix(stdout, nbase, nbase, a);
    printf("Pivot for LU decomposition:\n");
    fprint_ivector(stdout, nbase, ipiv);

    /* solve equations */
    const char trans = 'N';
    const int nrhs = 1;
    dgetrs_(&trans, &nbase, &nrhs, mat_ptr(a), &nbase, vec_ptr(ipiv), vec_ptr(b), &nbase, &info);
    if (info != 0)
    {
        fprintf(stderr, "Error: LAPACK::dgetrs failed\n");
        exit(1);
    }
    printf("Result of linear regression w:\n");
    fprint_dvector(stdout, nbase, b);

    free_dmatrix(a);
    free_dvector(b);
    return 0;
}
