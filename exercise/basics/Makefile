OBJ = hello hello_cxx hello_f fibonacci fibonacci_cxx fibonacci_f differential differential_cxx differential_f

.SUFFIXES:
.SUFFIXES: .c .C .f90

F90 = gfortran

CFLAGS = -O2
CXXFLAGS = $(CFLAGS)
FFLAGS = $(CFLAGS)

LDFLAGS =
LDLIBS =

default: $(OBJ)

.f90:
	$(F90) $(FFLAGS) $(LDFLAGS) $< $(LIBS) -o $@

clean:
	rm -f a.out *.o $(OBJ)
