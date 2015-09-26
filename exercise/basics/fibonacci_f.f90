program main
  implicit none
  integer,parameter :: n_max =60
  integer :: a(n_max) ! a(1) ... a(n_max)
  integer :: i
  a(1) = 1
  a(2) = 1
  do i = 3, n_max, 1
     a(i) = a(i-1) + a(i-2)
  end do
  do i = 10, n_max, 10
     print *, i, a(i)
  end do
end program main
