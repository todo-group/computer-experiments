OBJ = matrix_example multiply multiply_dgemm pointer pointer-matrix random_matrix
default: $(OBJ)

CFLAGS = -O3 -Wall -I../monte_carlo
LDLIBS = -llapack -lblas

clean:
	rm -rf $(OBJ)
