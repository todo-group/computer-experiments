OBJ = determinant gauss lu_decomp poisson_dense poisson_lu poisson_sparse steepest_descent
default: $(OBJ)

CFLAGS = -O3 -I../matrix
LDLIBS = -llapack -lblas

clean:
	rm -rf $(OBJ)
