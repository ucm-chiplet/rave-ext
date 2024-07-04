#include "/apps/x86/rave/include/rave_user_events.h"

void rave_event_and_value_f(int type, int value) {
	 rave_event_and_value(type, value)
}

void rave_name_event_f(int event, char * nam){
	rave_name_event(event,nam);
}

void rave_name_value_f(int event, int value, char * nam){
	rave_name_value(event,value,nam);
}

void rave_restart_trace_f(){
 	rave_restart_trace();
}
void rave_start_trace_f(){
	rave_start_trace();
}
void rave_stop_trace_f(){
	rave_stop_trace();
}
