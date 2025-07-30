struct value_info{
	struct value_info * next;
	char * name;
	int64_t ID;
};
typedef struct value_info value_info;

struct event_info{
	struct event_info * next;
	char * name; 
	int64_t ID;
	value_info * values;
	value_info * last_value;
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

value_info *  add_new_value(event_info * event, int val, char * name){
	value_info * new_values = (value_info*)malloc(sizeof(value_info));
	new_values -> next = NULL;
	new_values -> ID = val;
	my_strcpy(new_values->name, name);

	//Add it to the value stack
	if (event->last_value == NULL){
		event->values = new_values;
		event->last_value = new_values;
		return new_values;
	}
	event->last_value->next = new_values;
	event->last_value = new_values;
	return new_values;
}

void add_value_to_event(int id, int val, char * name){
	event_info * event = find_event(id);
	if (event == NULL){
		//printf("Event %d not found\n",id);
	 	return;
	}

	//If value already exists, just update its name
	value_info * value = find_value(event, val);
	if (value != NULL){
		free(value->name);
		//printf("%d %d found, update name from %s to %s\n", id,val,value->name, name);
		my_strcpy(value->name, name);
		return;
	}

	//value not found: create it
	//printf("%d %d not found, named it %s\n",id,val,name);
	add_new_value(event,val,name);
}

//For symbols
int add_value_name_to_event(int id, char * name){
	event_info * event = find_event(id);
	if (event == NULL) return -1;

	value_info * curr = event->values;
	int val = 1;
	while (curr!=NULL){
		if (!strcmp(curr->name, name)){
		 	return curr->ID;
		}
		curr = curr->next;
		++val;
	}
	//value not found: create it
	add_new_value(event,val,name);
	return val;
}

event_info * add_event(int id, char *name){

	event_info * event = find_event(id);
	if (event != NULL){
		free(event->name);
		my_strcpy(event->name, name);
		return event;
	}
	//event not found: create it
	event_info * new_event = (event_info*)malloc(sizeof(event_info));
	new_event -> next = NULL;
	new_event -> ID = id;
	new_event -> values = NULL;
	new_event -> last_value = NULL;
	my_strcpy(new_event->name, name);

	if (first_event_info==NULL) first_event_info = new_event;
	if (last_event_info!=NULL) last_event_info->next = new_event;
	last_event_info = new_event;

	return new_event;
}

char * get_event_value_name(event_info * event, int val){
	if (event==NULL) return "Value name not found";
	value_info * value = event->values; 
	while(value!=NULL){
		if (value->ID == val){
			return value->name;
		}
		value = value->next;
	}
	return "Value name not found";
}
