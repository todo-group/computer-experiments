#include "cmatrix.h"
#include "dgemm.h"
#include <math.h>
#include <stdio.h>

/* v = D^{1/2} v */
void product_D(int M, double temperature, double* v)
{
    const int dim = 1 << M; /* dim = 2^M */
    const double w[2] = { 1.0, exp(-1.0 / temperature) };
    for (int s = 0; s < dim; ++s)
    {
        double elem = v[s];
        for (int i = 0; i < M; ++i)
        {
            const int j = (i + 1) % M;
            elem *= w[((s >> i) & 1) ^ ((s >> j) & 1)];
        }
        v[s] = elem;
    }
}

/* v = U v */
void product_U(int M, double temperature, double* v, double* vtmp)
{
    const int dim = 1 << M; /* dim = 2^M */
    const double w[2] = { 1.0, exp(-2.0 / temperature) };
    for (int i = 0; i < M; ++i)
    {
        for (int s = 0; s < dim; ++s)
        {
            vtmp[s] = w[0] * v[s] + w[1] * v[s ^ (1 << i)];
        }
        for (int s = 0; s < dim; ++s)
        {
            v[s] = vtmp[s];
        }
    }
}

/* v = D^{1/2} U D^{1/2} v */
void product(int M, double temperature, double* v, double* vtmp)
{
    product_D(M, temperature, v);
    product_U(M, temperature, v, vtmp);
    product_D(M, temperature, v);
}

void fill_T(int M, double temperature, double** T)
{
    const int dim = 1 << M; /* dim = 2^M */
    double* v0 = alloc_dvector(dim);
    double* v1 = alloc_dvector(dim);
    for (int i = 0; i < dim; ++i)
    {
        for (int j = 0; j < dim; ++j)
        {
            v0[j] = 0.0;
        }
        v0[i] = 1.0;
        product(M, temperature, v0, v1);
        for (int j = 0; j < dim; ++j)
        {
            T[i][j] = v0[j];
        }
    }
    free_dvector(v0);
    free_dvector(v1);
}

int main(void)
{
    const double temperature = 2.0;
    const int L = 4;
    const int M = L;

    /* prepare transfer matrix T */
    const int dim = 1 << M;
    double** T = alloc_dmatrix(dim, dim);
    fill_T(M, temperature, T);

    /* calculate T^L */
    double** T0 = alloc_dmatrix(dim, dim);
    double** T1 = alloc_dmatrix(dim, dim);
    /* T0 = identity matrix */
    for (int i = 0; i < dim; ++i)
    {
        for (int j = 0; j < dim; ++j)
        {
            mat_elem(T0, i, j) = 0.0;
        }
        mat_elem(T0, i, i) = 1.0;
    }
    const char trans = 'N';
    const double alpha = 1.0;
    const double beta = 0.0;
    for (int k = 0; k < L; ++k)
    {
        /* T1 = T * T0 */
        dgemm_(&trans, &trans, &dim, &dim, &dim, &alpha, mat_ptr(T),
            &dim, mat_ptr(T0), &dim, &beta, mat_ptr(T1), &dim);
        /* T0 = T1 */
        for (int j = 0; j < dim; ++j)
        {
            for (int i = 0; i < dim; ++i)
            {
                mat_elem(T0, i, j) = mat_elem(T1, i, j);
            }
        }
    }

    /* take trace of T^L */
    double sum = 0.0;
    for (int i = 0; i < dim; ++i)
    {
        sum += mat_elem(T0, i, i);
    }

    const int num_sites = L * M;
    printf("temperature = %15.10f\n", temperature);
    printf("L = %d\n", L);
    printf("Z = %15.10f * exp(%14.10f)\n", sum, 2 * num_sites / temperature);
    printf("free energy = %15.10f\n", -2 * num_sites - temperature * log(sum));
    printf("free energy density = %15.10f\n", -2 - temperature * log(sum) / num_sites);

    free_dmatrix(T);
    free_dmatrix(T0);
    free_dmatrix(T1);
    return 0;
}
