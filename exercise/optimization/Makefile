OBJ = golden_section gradient_descent_2d nelder_mead_1d nelder_mead_2d mc_steepest_descent_1d mc_steepest_descent_2d simulated_annealing_1d simulated_annealing_2d
default: $(OBJ)

CPPFLAGS = -I.
CFLAGS = -O3
LDFLAGS = -llapack -lm

clean:
	rm -rf *.o $(OBJ)
