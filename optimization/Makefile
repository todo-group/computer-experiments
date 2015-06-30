OBJ = random golden_section nelder_mead_1d nelder_mead_2d
default: $(OBJ)

CPPFLAGS = -I$(BOOST_ROOT)/include
CFLAGS = -O3
LDFLAGS = -llapack -lm

clean:
	rm -rf $(OBJ)
