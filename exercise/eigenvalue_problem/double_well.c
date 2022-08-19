#include "cmatrix.h"
#include "dsyev.h"
#include <stdio.h>
#include <stdlib.h>

double potential(double x, double v, double width)
{
    if (x > (0.5 - 0.5 * width) && x < (0.5 + 0.5 * width))
    {
        return v;
    }
    else
    {
        return 0.0;
    }
}

int main(int argc, char** argv)
{
    int n;        /* partition number */
    double v;     /* height of potential between two wells */
    double width; /* width of wall between two wells */

    int num_states = 10; /* number of eigenstates to be printed out */

    const char jobz = 'V'; /* Compute eigenvalues and eigenvectors */
    const char uplo = 'U'; /* Upper triangle of A is stored */

    if (argc < 4)
    {
        fprintf(stderr, "Usage: %s n v width\n", argv[0]);
        exit(1);
    }
    n = atoi(argv[1]);
    v = atof(argv[2]);
    width = atof(argv[3]);

    int dim = n - 1; /* dimension of Hamiltonian */
    const double h = 1.0 / n;
    const double diag = 2.0 / (h * h);
    const double offdiag = -1.0 / (h * h);
    if (num_states > dim)
    {
        num_states = dim;
    }

    /* preparation of Mat */
    double** mat = alloc_dmatrix(dim, dim);
    for (int j = 0; j < dim; ++j)
    {
        for (int i = 0; i < dim; ++i)
        {
            mat_elem(mat, i, j) = 0.0;
        }
    }
    for (int i = 0; i < dim; ++i)
    {
        const double x = 1.0 * (i + 1) / n; /* x = 0 for i = -1 and x = 1 for i = dim */
        if (i > 0)
        {
            mat_elem(mat, i, i - 1) = offdiag;
        }
        mat_elem(mat, i, i) = diag + potential(x, v, width);
        if (i < (dim - 1))
        {
            mat_elem(mat, i, i + 1) = offdiag;
        }
    }

    /* perform eigenvalue decomposition */
    double* ene = alloc_dvector(dim); /* eigenenergies */
    const int lwork = 3 * dim - 1;
    double* work = alloc_dvector(lwork); /* working area */
    int info;
    dsyev_(&jobz, &uplo, &dim, mat_ptr(mat), &dim, vec_ptr(ene), vec_ptr(work),
        &lwork, &info);
    if (info != 0)
    {
        fprintf(stderr, "Error: LAPACK::dsyev failed\n");
        exit(1);
    }

    printf("# n: %10d\n", n);
    printf("# v: %10.5f\n", v);
    printf("# width: %10.5f\n", width);
    printf("# %3d lowest eigenenergies:\n# ", num_states);
    for (int i = 0; i < num_states; ++i)
    {
        printf("%10.5f ", ene[i]);
    }
    printf("\n");

    printf("# %3d low-energy eigenstates:\n", num_states);
    printf("%10.5f ", 0.0);
    for (int i = 0; i < num_states; ++i)
    {
        printf("%10.5f ", 0.0);
    }
    printf("\n");
    for (int j = 0; j < dim; ++j)
    {
        printf("%10.5f ", 1.0 * (j + 1) / n);
        for (int i = 0; i < num_states; ++i)
        {
            printf("%10.5f ", mat_elem(mat, j, i));
        }
        printf("\n");
    }
    printf("%10.5f ", 1.0);
    for (int i = 0; i < num_states; ++i)
    {
        printf("%10.5f ", 0.0);
    }
    printf("\n");

    free_dmatrix(mat);
    free_dvector(ene);
    free_dvector(work);
    return 0;
}
