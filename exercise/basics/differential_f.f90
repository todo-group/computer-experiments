function f(x)
   implicit none
   double precision :: f
   double precision :: x
   f = sin(x)
end function

function df(x)
   implicit none
   double precision :: df
   double precision :: x
   df = cos(x)
end function

program main
   implicit none
   double precision :: x, h, exact, diff
   double precision :: f, df
   x = 0.3D0*4.0*atan(1.0D0)
   h = 0.1D0; 
   exact = df(x); 
   diff = (f(x + h) - f(x))/h; 
   print *, h, diff, abs(diff - exact)
end program
