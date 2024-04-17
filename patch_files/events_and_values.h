
struct value_info{
	struct value_info * next;
	char name[64];
	int64_t ID;
};
typedef struct value_info value_info;

struct event_info{
		struct event_info * next;
		char name[64];
		int64_t ID;
		value_info * values;
};
typedef struct event_info event_info;
event_info * first_event_info = NULL;
event_info * last_event_info = NULL;


event_info * find_event(int id){
				event_info * curr = first_event_info;
				while (curr!=NULL){
								if (curr->ID == id) return curr;
								curr = curr->next;
				}
				return NULL;
}

value_info * find_value(event_info * event, int val){
				value_info * curr = event->values;
				while (curr!=NULL){
								if (curr->ID == val) return curr;
								curr = curr->next;
				}
				return NULL;
}

void add_value_to_event(int id, int val, char * name){
				event_info * event = find_event(id);
				if (event == NULL) return;
				value_info * value = find_value(event, val);
				if (value != NULL){
					strcpy(value->name, name);
					return;
				}
				//value not found: create it
				value_info * new_values = (value_info*)malloc(sizeof(value_info));
				new_values -> next = NULL;
				new_values -> ID = val;
				strcpy(new_values->name, name);

	
				value_info * last = event->values;
				if (last == NULL){
					event->values = new_values;
					return;
				}
				while (last != NULL){
					if (last->next == NULL){
						last->next = new_values;
						return;
					}
					last = last->next;
				}
}

event_info * add_event(int id, char *name){

				event_info * event = find_event(id);
				if (event != NULL){
					strcpy(event->name, name);
					return event;
				}
				//event not found: create it
				event_info * new_event = (event_info*)malloc(sizeof(event_info));
				new_event -> next = NULL;
				new_event -> ID = id;
				new_event -> values = NULL;
				strcpy(new_event->name, name);

				if (first_event_info==NULL) first_event_info = new_event;
				if (last_event_info!=NULL) last_event_info->next = new_event;
				last_event_info = new_event;
				
				return new_event;
}

char * get_event_value_name(event_info * event, int val){
				value_info * value = event->values; 
				while(value!=NULL){
					if (value->ID == val){
									return value->name;
					}
					value = value->next;
				}
				return NULL;
}
