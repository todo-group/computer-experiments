#include <stdio.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char** argv) {
  double mypi, pi;
  int nsteps;
  int myrank;
  int total_proc_num;
  double step_width;
  int i;
  double x;
  
  nsteps = 100000;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &total_proc_num); /* get total process number */
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank); /* get own rank number */

  if (myrank == 0) {
    printf("total_proc_size: %d\n", total_proc_num);
    printf("nsteps: %d\n", nsteps);
  }

  /* broadcast nsteps to each processes */
  MPI_Bcast(&nsteps, 1, MPI_INT, 0, MPI_COMM_WORLD);
  
  /* parallelized for-loop */
  mypi = 0;
  step_width = 1.0/nsteps;
  for (i = myrank; i < nsteps; i += total_proc_num) {
    x = (i + 0.5)*step_width;
    mypi += 4.0/(1.0 + x*x);
  }
  mypi = mypi / nsteps;

  // All gether results of each processes
  MPI_Reduce(&mypi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  if (myrank == 0) {
    printf("pi:     %17.15f\n", pi);
    printf("diff:   %17.15f\n", pi - M_PI);
  }

  MPI_Finalize();
  return 0;
}
