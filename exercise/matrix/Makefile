OBJ = matrix_example multiply multiply_dgemm pointer random_matrix
default: $(OBJ)

CFLAGS = -O3 -I../monte_carlo
LDLIBS = -llapack -lblas

clean:
	rm -rf $(OBJ)
