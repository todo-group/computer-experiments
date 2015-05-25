#include "matrix_util.h"
#include <stdio.h>
#include <stdlib.h>

/* http://www.netlib.org/lapack/explore-html/dd/d4c/dsyev_8f.html */
void dsyev_(char *JOBZ, char *UPLO, int *N, double *A, int *LDA, double *W,
            double *WORK, int *LWORK, int *INFO);

double potential(double x, double v, double width) {
  if (x > (0.5 - 0.5 * width) && x < (0.5 + 0.5 * width)) {
    return v;
  } else {
    return 0.0;
  }
}
  
int main(int argc, char** argv) {
  int n; /* partition number */
  double v; /* height of potential between two wells */
  double width; /* width of wall between two wells */

  int num_states = 10; /* number of eigenstates to be printed out */
  int dim; /* dimension of Hamiltonian */
  double h;
  double diag;
  double offdiag;
  
  double **mat;
  double *ene; /* eigenenergies */
  double *work; /* working area */

  int lwork, info;
  char jobz = 'V'; /* Compute eigenvalues and eigenvectors */
  char uplo = 'U'; /* Upper triangle of A is stored */

  int i, j;
  double x;

  if (argc < 4) {
    fprintf(stderr, "Usage: %s n v width\n", argv[0]);
    exit(1);
  }
  n = atoi(argv[1]);
  v = atof(argv[2]);
  width = atof(argv[3]);

  dim = n - 1;
  h = 1.0 / n;
  diag = 2.0 / (h*h);
  offdiag = -1.0 / (h*h);
  if (num_states > dim) {
    num_states = dim;
  }
  
  /* preparation of Mat */
  mat = alloc_dmatrix(dim, dim);
  for (i = 0; i < dim; ++i) {
    x = 1.0 * (i+1) / n; /* x = 0 for i = -1 and x = 1 for i = dim */
    if (i > 0) {
      mat[i][i-1] = offdiag;
    }
    mat[i][i] = diag + potential(x, v, width);
    if (i < (dim - 1)) {
      mat[i][i+1] = offdiag;
    }
  }
  
  /* perform eigenvalue decomposition */
  ene = alloc_dvector(dim);
  lwork = 3*dim - 1;
  work = alloc_dvector(lwork);
  dsyev_(&jobz, &uplo, &dim, &mat[0][0], &dim, &ene[0], &work[0],
         &lwork, &info);
  if (info != 0) {
    fprintf(stderr, "Error: LAPACK::dsyev failed\n");
    exit(1);
  }

  printf("# n: %10d\n", n);
  printf("# v: %10.5f\n", v);
  printf("# width: %10.5f\n", width);
  printf("# %3d lowest eigenenergies:\n# ", num_states);
  for (int i = 0; i < num_states; ++i)
    printf("%10.5f ", ene[i]);
  printf("\n");

  printf("# %3d low-energy eigenstates:\n", num_states);
  printf("%10.5f ", 0.0);
  for (i = 0; i < num_states; ++i) {
    printf("%10.5f ", 0.0);
  }
  printf("\n");
  for (j = 0; j < dim; ++j) {
    printf("%10.5f ", 1.0 * (j+1) / n);
    for (i = 0; i < num_states; ++i) {
      printf("%10.5f ", mat[i][j]);
    }
    printf("\n");
  }
  printf("%10.5f ", 1.0);
  for (i = 0; i < num_states; ++i) {
    printf("%10.5f ", 0.0);
  }
  printf("\n");

  free_dmatrix(mat);
  free_dvector(ene);
  free_dvector(work);
}
