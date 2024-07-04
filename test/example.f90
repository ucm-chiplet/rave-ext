program example
  use rave_module
  integer, parameter :: N = 256*10+13 
  real(8) :: A(N)
  real(8) :: B(N)
  real(8) :: C(N)
  integer :: i

  call rave_name_event(1000,"code_region");
  call rave_name_value(1000,0_8,"End");
  call rave_name_value(1000,1_8,"ini_A");
  call rave_name_value(1000,2_8,"ini_B");
  call rave_name_value(1000,3_8,"ini_C");
  call rave_name_value(1000,4_8,"arith_vec");
  call rave_name_value(1000,5_8,"if_vec");

  call rave_event_and_value(1000,1_8)
  do i = 1, N
      A(i) = i-1
  end do
  call rave_event_and_value(1000,0_8)


  call rave_event_and_value(1000,2_8)
  !$omp simd
  do i = 1, N
      B(i) = 2.5
  end do
  call rave_event_and_value(1000,0_8)

  call rave_stop_trace();

  call rave_event_and_value(1000,3_8)
  !$omp simd
  do i = 1, N
      C(i) = -(i-1)
  end do
  call rave_event_and_value(1000,0_8)

  call rave_start_trace();

  call rave_event_and_value(1000,4_8)
  !$omp simd
  do i = 1, N
      A(i) = A(i) - B(i)*0.2 + 0.5*C(i)
  end do
  call rave_event_and_value(1000,0_8)

  call rave_event_and_value(1000,5_8)
  !$omp simd
  do i = 1, N
     if ( A(i) > 0.5) then
         C(i) = C(i) + A(i)*0.2 
     end if
  end do
  call rave_event_and_value(1000,0_8)
  print'(a,f5.2)', "", C(1)  

end program example
