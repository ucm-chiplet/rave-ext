

program example
  use rave_user_events
  integer, parameter :: N = 256*10+13 
  real(8) :: A(N)
  real(8) :: B(N)
  real(8) :: C(N)
  integer :: i

  call rave_begin_region("initialization")
  call initialize(N,A,B,C)
  call rave_end_region("initialization")

  call rave_begin_region("compute")
  call compute(N,A,B,C)
  call rave_end_region("compute")

  print'(a,f5.2)', "", C(1)  

contains
  subroutine initialize(N, A, B, C)
    implicit none
    integer, intent(in) :: N
    real(8), intent(out) :: A(N), B(N), C(N)
    integer :: i

    call rave_begin_region("ini_A")
    do i = 1, N
      A(i) = i-1
    end do
    call rave_end_region("ini_A")


    call rave_begin_region("ini_B")
    !$omp simd
    do i = 1, N
      B(i) = 2.5
    end do
    call rave_end_region("ini_B")

    call rave_disable();

    call rave_begin_region("ini_C")
    !$omp simd
    do i = 1, N
      C(i) = -(i-1)
    end do
    call rave_end_region("ini_C")

    call rave_enable();

  end subroutine initialize

  subroutine compute(N, A, B, C)
    implicit none
    integer, intent(in) :: N
    real(8), intent(inout) :: A(N), B(N), C(N)
    integer :: i

    call rave_begin_region("arith_vec") 
    !$omp simd
    do i = 1, N
      A(i) = A(i) - B(i)*0.2 + 0.5*C(i)
    end do
    call rave_end_region("arith_vec")

    call rave_begin_region("if_vec")
    !$omp simd
    do i = 1, N
      if ( A(i) > 0.5) then
        C(i) = C(i) + A(i)*0.2 
      end if
    end do
    call rave_end_region("if_vec")

  end subroutine compute 
end program example
