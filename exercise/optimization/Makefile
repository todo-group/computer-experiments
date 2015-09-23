OBJ = golden_section nelder_mead_1d nelder_mead_2d steepest_decent simulated_annealing
default: $(OBJ)

CPPFLAGS = -I.
CFLAGS = -O3
LDFLAGS = -llapack -lm

clean:
	rm -rf *.o $(OBJ)
