#include <iostream>
#include <math.h>   // M_PI
#include <iomanip>  //
#include <mpi.h>    // MPI

int main(int argc, char** argv)
{
  double my_pi = 0;
  int nsteps;
  int myrank;
  const int MASTER = 0;
  int total_proc_num;
  
  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &total_proc_num); // get total process number
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank); //  get own rank number

  if(myrank == MASTER) {
    std::cout << "myrank: " << myrank << std::endl;
    std::cout << "input: ";
    std::cin >> nsteps;
  }

  // broadcast nsteps to each processes
  MPI_Bcast(&nsteps, 1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
  
  // parallelized for-loop
  double step_width = 1.0/nsteps;
  for(int i = myrank; i<nsteps; i+=total_proc_num) {
    double x = (i + 0.5)*step_width;
    my_pi += 4.0/(1.0 + x*x);
  }
  my_pi = my_pi / nsteps;

  // All gether results of each processes
  double pi=0;
  MPI_Reduce(&my_pi, &pi, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);

  double diff = pi - M_PI;

  if(myrank == MASTER) {
    std::cout << "rank:   " << myrank << std::endl
	      << "nsteps: " << nsteps << std::endl
	      << "my_pi= " << std::setprecision(10) << pi   << std::endl
	      << "diff = " << std::setprecision(10) << diff << std::endl;
  }

  MPI_Finalize();
  return 0;
}
