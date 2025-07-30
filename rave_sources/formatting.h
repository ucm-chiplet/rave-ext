
char * format_end = "\033[0m";
char * format_bold_num = "\033[1;34m";

int PLAIN_TEXT=0;
#define CLEAR_FORMAT "\033[0m"
#define BOLD_BLUE "\033[1;34m"
#define BOLD_GREEN "\033[1;32m"
#define BOLD_PINK "\033[1;35m"
#define BOLD_CYAN "\033[1;36m"
#define BOLD_WHITE "\033[1m"
#define BOLD_YELLOW "\033[1;33m"

#define P_GENERIC(fd,format,x,color)\
	if(!PLAIN_TEXT) fprintf(fd,color); \
	fprintf(fd,format,x);\
	if(!PLAIN_TEXT) fprintf(fd,CLEAR_FORMAT);


#define P_COUNTERS(fd,format,name) P_GENERIC(fd,format,name,BOLD_YELLOW)
#define P_NAME(fd,format,name) P_GENERIC(fd,format,name,BOLD_CYAN)
#define P_NUMBER(fd,format,x) P_GENERIC(fd,format,x,BOLD_BLUE)
#define P_VL(fd,format,x) P_GENERIC(fd,format,x,BOLD_GREEN)
#define P_PERCENTAGE(fd,format,x) P_GENERIC(fd,format,x,BOLD_PINK)


#define BOLD_R(x) "\033[1;31m"x"\033[0m"
#define BOLD_PER(x) "\033[1;35m"x"\033[0m"
#define BOLD_NAME(x) "\033[1;36m"x"\033[0m"
#define BOLD_NUM(x) "\033[1;34m"x"\033[0m"
#define BOLD_G(x) "\033[1;33m"x"\033[0m"
#define BOLD_VL(x) "\033[1;32m"x"\033[0m"
#define BOLD(x) "\033[1m"x"\033[0m"

char sym_pipe[]={"│"};
char sym_cross[]={"├"};
char sym_line[]={"─"};
char sym_ele[]={"└"};

struct indent_control_t{
	char buffer[1024];
	int offset;
	char offsets[128]; //offsets point to where each nest symbol is
	int nesting;
	int clear_previous;
	int prev_nest;
	int spaces;
};
typedef struct indent_control_t indent_control_t;
indent_control_t ic = {0};

void reset_indent(){
	ic.buffer[0]='\0';
	ic.offset=0;
	ic.nesting=0;
	ic.clear_previous=0;
	ic.prev_nest=0;
	ic.spaces=0;
}

char * indent_add(int is_last){
	//If previous was last, we need to change last segment to spaces:
	if (ic.nesting>0){
		if (ic.clear_previous){
			//I need to put a space and shift 2 positions, because the special characters are 3 chars wide.
			ic.buffer[ic.offsets[ic.nesting-1]] = ' ';
			char * rewrite = &ic.buffer[ic.offsets[ic.nesting-1]+1];
			while (*rewrite != '\0'){
				*(rewrite) = *(rewrite+2);
				++rewrite;
			}
			ic.offset -= 2;
		}else{ 
			//If not, change last indent character to a pipe
			strcpy(&ic.buffer[ic.offsets[ic.nesting-1]], sym_pipe); 
		}
	}
	ic.clear_previous=0;
	//Increase nesting
	++ic.nesting;

	//Append some spaces
	for(int i=0; i<ic.spaces; ++i) ic.buffer[ic.offset++] = ' ';

	ic.offsets[ic.nesting-1] = ic.offset;

	//Append the special character
	if (is_last) strcpy(&ic.buffer[ic.offset], sym_ele);
	else strcpy(&ic.buffer[ic.offset], sym_cross);
	ic.offset+=3;
	ic.buffer[ic.offset] = '\0';

	if (is_last) ic.clear_previous=1;
	

	return ic.buffer;
}

char * indent_sub(int howmany, int is_last){
	//I don't need to clear previous, as I'm already going back.	
	ic.clear_previous=0;
	//Remove nesting:
	ic.nesting -= howmany;
	if(ic.nesting<0) ic.nesting=0;

	//Append the special character
	if (ic.nesting>0){
		ic.offset = ic.offsets[ic.nesting-1]; 
		if (is_last){
		 	strcpy(&ic.buffer[ic.offset], sym_ele);
			ic.clear_previous=1;
		}else strcpy(&ic.buffer[ic.offset], sym_cross);
		ic.offset+=3;
		ic.buffer[ic.offset] = '\0';
	}
	return ic.buffer;
}

char * indent_none(int is_last){
	//I don't need to clear previous, because previous will never be last (if i'm not indenting...)
	ic.clear_previous=0;
	//If is last, change last indent character to an L
	if (is_last){
		strcpy(&ic.buffer[ic.offsets[ic.nesting-1]], sym_ele);
		ic.clear_previous=1;
	}
	return ic.buffer;
}

void indent(FILE * fd, int level, int is_last){
	int inc = level-(ic.prev_nest);
	ic.prev_nest=level;
	if (inc==0) indent_none(is_last);
	else if (inc>0) indent_add(is_last);
	else indent_sub(-inc,is_last);
	fprintf(fd,"%s─ ",ic.buffer);
}
