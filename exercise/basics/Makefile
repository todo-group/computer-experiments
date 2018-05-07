OBJ = bisection hello hello_cxx hello_f fibonacci fibonacci_cxx fibonacci_f differential differential_cxx differential_f

.SUFFIXES:
.SUFFIXES: .c .C .f90

F90 = gfortran

CFLAGS = -O3 -Wall
CXXFLAGS = $(CFLAGS)
FFLAGS = $(CFLAGS)

default: $(OBJ)

.f90:
	$(F90) $(FFLAGS) $(LDFLAGS) $< $(LDLIBS) -o $@

clean:
	rm -f a.out *.o $(OBJ)
