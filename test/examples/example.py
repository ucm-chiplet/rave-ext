import rave_user_events
def main():
    #1. Assign tuples of values and names to event 1000, and name it "code_region"
    rave_user_events.rave_name_event(1000,"code_region");
    rave_user_events.rave_name_value(1000,0,"End");
    rave_user_events.rave_name_value(1000,1,"ini_A");
    rave_user_events.rave_name_value(1000,2,"ini_B");
    rave_user_events.rave_name_value(1000,3,"ini_C");
    rave_user_events.rave_name_value(1000,4,"arith_vec");
    rave_user_events.rave_name_value(1000,5,"if_vec");

    #2. Initialize tracing
    rave_user_events.rave_restart_trace();

    N = 256*10 + 13
    A = [0]*N
    B = [0]*N
    C = [0]*N

    #3. Enclose first loop in value 1 ("ini_A")
    rave_user_events.rave_event_and_value(1000,1)
    for i in range(N):
        A[i] = i
    rave_user_events.rave_event_and_value(1000,0)

    #4. Enclose second loop in value 2 ("ini_B")
    rave_user_events.rave_event_and_value(1000,2)
    for i in range(N):
        B[i] = 2.5
    rave_user_events.rave_event_and_value(1000,0)

    #5. Disable tracing after this point. Events and values won't be recorded 
    rave_user_events.rave_stop_trace();

    rave_user_events.rave_event_and_value(1000,3)
    for i in range(N):
        C[i] = -i
    rave_user_events.rave_event_and_value(1000,0)

    #6. Re-enable tracing
    rave_user_events.rave_start_trace();

    #7. Enclose fourth loop in value 2 ("arith_vec")
    rave_user_events.rave_event_and_value(1000,4)
    for i in range(N):
        A[i] -= B[i]*0.2 + 0.5*C[i]
    rave_user_events.rave_event_and_value(1000,0)

    #8. Enclose fifth loop in value 5 ("if_vec")
    rave_user_events.rave_event_and_value(1000,5)
    for i in range(N):
        if (A[i] > 0.5):
            C[i] += A[i]*0.2
    rave_user_events.rave_event_and_value(1000,0)

if __name__ == "__main__":
    main()
