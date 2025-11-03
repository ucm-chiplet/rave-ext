import rave_user_events

def initialize(N, A, B, C):
    rave_user_events.rave_begin_region("ini_A")
    for i in range(N):
        A[i] = i
    rave_user_events.rave_end_region("ini_A")

    rave_user_events.rave_begin_region("ini_B")
    for i in range(N):
        B[i] = 2.5
    rave_user_events.rave_end_region("ini_B")

    rave_user_events.rave_stop_trace();

    rave_user_events.rave_begin_region("ini_c")
    for i in range(N):
        C[i] = -i
    rave_user_events.rave_end_region("ini_C")

    rave_user_events.rave_start_trace();


def compute(N, A, B, C):

    rave_user_events.rave_begin_region("arith_vec")
    for i in range(N):
        A[i] -= B[i]*0.2 + 0.5*C[i]
    rave_user_events.rave_end_region("arith_vec")

    rave_user_events.rave_begin_region("if_vec")
    for i in range(N):
        if (A[i] > 0.5):
            C[i] += A[i]*0.2
    rave_user_events.rave_end_region("if_vec")

def main():

    rave_user_events.rave_restart_trace();

    N = 256*10 + 13
    A = [0]*N
    B = [0]*N
    C = [0]*N

    rave_user_events.rave_begin_region("initialization")
    initialize(N, A, B, C)
    rave_user_events.rave_end_region("initialization")

    rave_user_events.rave_begin_region("compute")
    compute(N, A, B, C)
    rave_user_events.rave_end_region("compute")

if __name__ == "__main__":
    main()
