module rave_module
  use iso_c_binding
  implicit none
  interface
    subroutine rave_event(event, val) bind(c, name="rave_event_f")
       use iso_c_binding
       integer(kind=c_int), value :: event, val
    end subroutine rave_event 

    
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
  subroutine rave_name_event(event, nam)
    integer(kind=c_int), value :: event
    character(kind=c_char, len=*), target :: nam
    type(c_ptr) :: cstr_ptr
    cstr_ptr = c_loc(nam)
    call rave_name_event_2(event,cstr_ptr)
  end subroutine rave_name_event

  subroutine rave_name_value(event,val, nam)
    integer(kind=c_int), value :: event, val
    character(kind=c_char, len=*), target :: nam
    type(c_ptr) :: cstr_ptr
    cstr_ptr = c_loc(nam)
    call rave_name_value_2(event,val,cstr_ptr)
  end subroutine rave_name_value

end module rave_module
