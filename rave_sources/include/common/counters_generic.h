#include "counters.h"

void reset_counters(rave_counters * c);
//c1 = c2
void copy_counters(rave_counters * c1, rave_counters * c2);
//c1 += c2;
void add_counters(rave_counters * c1, rave_counters * c2);
//c1 -= c2;
void sub_counters(rave_counters * c1, rave_counters * c2);
#if 0
//c1 = moving_avg(c1,c2)
void avg_counters(rave_counters * c1, rave_counters * c2, int n);
#endif
// c1 = c2*mult
void mul_counters(rave_counters * c1, rave_counters * c2, double mult);
//c1 = c2-c1
void update_counters(rave_counters * c1, rave_counters * c2);
