#include "/apps/x86/rave/include/rave_user_events.h"

void rave_event_f(int type, int value) {
	 rave_event_and_value(type, value)
}

void rave_name_event_f(int event, const char * nam){
	rave_name_event(event,nam);
}

void rave_name_value_f(int event, int value, const char * nam){
	rave_name_value(event,value,nam);
}
