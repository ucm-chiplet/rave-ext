module rave_user_events
  use iso_c_binding
  implicit none
  interface
    subroutine rave_event_and_value(event, val) bind(c, name="rave_event_and_value_f")
       use iso_c_binding
       integer(kind=c_int), value :: event, val
    end subroutine rave_event_and_value

    subroutine rave_restart_trace() bind(c, name="rave_restart_trace_f")
       use iso_c_binding
    end subroutine rave_restart_trace

    subroutine rave_start_trace() bind(c, name="rave_start_trace_f")
       use iso_c_binding
    end subroutine rave_start_trace

    subroutine rave_stop_trace() bind(c, name="rave_stop_trace_f")
       use iso_c_binding
    end subroutine rave_stop_trace

    subroutine rave_begin_region_internal(name) &
                 bind(c, name="rave_begin_region_f")
       use iso_c_binding
       type(c_ptr), value :: name
    end subroutine rave_begin_region_internal 

    subroutine rave_end_region_internal(name) &
                 bind(c, name="rave_end_region_f")
       use iso_c_binding
       type(c_ptr), value :: name
    end subroutine rave_end_region_internal 
    
    subroutine rave_name_event_2(event, nam) bind(c, name="rave_name_event_f")
       use iso_c_binding
       integer(kind=c_int), value :: event
       type(c_ptr), value :: nam
    end subroutine rave_name_event_2 

    subroutine rave_name_value_2(event, val, nam) bind(c, name="rave_name_value_f")
       use iso_c_binding
       integer(kind=c_int), value :: event,val
       type(c_ptr), value :: nam
    end subroutine rave_name_value_2

  end interface


contains
  subroutine rave_begin_region(name)
    character(kind=c_char, len=*), target :: name
    character(kind=c_char, len=:), target, allocatable :: name_bigger
    type(c_ptr) :: e_cstr_ptr
    allocate(character(len=len(name)+1, kind=c_char) :: name_bigger)
    name_bigger(1:len(name)) = name
    name_bigger(len(name)+1 : len(name)+1)=C_NULL_CHAR
    e_cstr_ptr = c_loc(name_bigger)
    call rave_begin_region_internal(e_cstr_ptr)
  end subroutine rave_begin_region

  subroutine rave_end_region(name)
    character(kind=c_char, len=*), target :: name
    character(kind=c_char, len=:), target, allocatable :: name_bigger
    type(c_ptr) :: e_cstr_ptr
    allocate(character(len=len(name)+1, kind=c_char) :: name_bigger)
    name_bigger(1:len(name)) = name
    name_bigger(len(name)+1 : len(name)+1)=C_NULL_CHAR
    e_cstr_ptr = c_loc(name_bigger)
    call rave_end_region_internal(e_cstr_ptr)
  end subroutine rave_end_region

  subroutine rave_name_event(event, nam)
    integer(kind=c_int), value :: event
    character(kind=c_char, len=*), target :: nam
    type(c_ptr) :: cstr_ptr
    nam(len(nam)+1:len(nam)+1)=C_NULL_CHAR
    cstr_ptr = c_loc(nam)
    call rave_name_event_2(event,cstr_ptr)
  end subroutine rave_name_event

  subroutine rave_name_value(event,val, nam)
    integer(kind=c_int), value :: event, val
    character(kind=c_char, len=*), target :: nam
    type(c_ptr) :: cstr_ptr
    nam(len(nam)+1:len(nam)+1)=C_NULL_CHAR
    cstr_ptr = c_loc(nam)
    call rave_name_value_2(event,val,cstr_ptr)
  end subroutine rave_name_value

end module rave_user_events
