#include "stdio.h"
#include "stdint.h"

#include <ompt.h>
#if 1
#define DEBUG_PRINT(...) ;
#else
#define DEBUG_PRINT printf
#endif
void tracer_cb_parallel_begin(
        ompt_data_t *encountering_task_data,
        const ompt_frame_t *encountering_task_frame,
        ompt_data_t *parallel_data,
        unsigned int requested_parallelism,
        int flags,
        const void *codeptr_ra) {
    DEBUG_PRINT("parallel_begin!\n");
		asm volatile(
				"xor x0, %0, x0\n"
				::"r"(requested_parallelism)
				);
}

void tracer_cb_parallel_end(
  ompt_data_t *parallel_data,
  ompt_data_t *encountering_task_data,
  int flags,
  const void *codeptr_ra) {
	asm volatile("li x0, -6\n");
}

void tracer_cb_sync_region(
        ompt_sync_region_t kind,
        ompt_scope_endpoint_t endpoint,
        ompt_data_t *parallel_data,
        ompt_data_t *task_data,
        const void *codeptr_ra) {
    DEBUG_PRINT("%d: sync_region kind=%d endpoint=%d. parallel_data.value=%d !!\n", omp_get_thread_num(), kind, endpoint,	(parallel_data!=NULL)?parallel_data->value:-1);
		if (kind==2 && endpoint==1) asm volatile("li x0, -5\n");
}

void tracer_cb_implicit_task(
        ompt_scope_endpoint_t endpoint,
        ompt_data_t *parallel_data,
        ompt_data_t *task_data,
        unsigned int actual_parallelism,
        unsigned int index,
        int flags) {
    DEBUG_PRINT("%d: implicit_task flags=%08x endpoint=%d!!\n", omp_get_thread_num(), flags, endpoint);
		if (flags==2 && endpoint==1) asm volatile("li x0, -5\n");
}

typedef struct {
    ompt_callback_parallel_begin_t    parallel_begin;
    ompt_callback_parallel_end_t      parallel_end;
    ompt_callback_sync_region_t       sync_region;
    ompt_callback_implicit_task_t     implicit_task;
		#if 0
    ompt_callback_thread_begin_t      thread_begin;
    ompt_callback_thread_end_t        thread_end;
    ompt_callback_work_t              work;
    ompt_callback_dispatch_t          dispatch;
    ompt_callback_task_create_t       task_create;
    ompt_callback_dependences_t       dependences;
    ompt_callback_task_dependence_t   task_dependence;
    ompt_callback_task_schedule_t     task_schedule;
    ompt_callback_masked_t            masked;
    ompt_callback_device_initialize_t device_initialize;
    ompt_callback_device_finalize_t   device_finalize;
    ompt_callback_device_load_t       device_load;
    ompt_callback_device_unload_t     device_unload;
    ompt_callback_buffer_request_t    buffer_request;
    ompt_callback_buffer_complete_t   buffer_complete;
		#endif
} callbacks_ompt_t;

callbacks_ompt_t generate_callback_table(void) {
    return (callbacks_ompt_t) {
        .parallel_begin      = tracer_cb_parallel_begin,
        .parallel_end        = tracer_cb_parallel_end,
        .sync_region         = tracer_cb_sync_region,
        .implicit_task       = tracer_cb_implicit_task,
				#if 0
        .thread_begin        = NULL, 
        .thread_end          = NULL, 
        .work                = NULL,
        .dispatch            = NULL,
        .task_create         = NULL,
        .dependences         = NULL, 
        .task_dependence     = NULL, 
        .task_schedule       = NULL,
        .masked              = NULL, 
        .device_initialize   = NULL, 
        .device_finalize     = NULL, 
        .device_load         = NULL, 
        .device_unload       = NULL, 
				#endif
    };
}

static ompt_set_callback_t set_callback_fn = NULL;
static inline int set_ompt_callback(ompt_callbacks_t event, ompt_callback_t callback) {
    int error = 1;
    switch (set_callback_fn(event, callback)) {
        case ompt_set_error:
            DEBUG_PRINT("OMPT set callback %d failed.\n", event);
            break;
        case ompt_set_never:
            DEBUG_PRINT("OMPT set callback %d returned 'never'. The event was "
                    "registered but it will never occur or the callback will never "
                    "be invoked at runtime.\n", event);
            break;
        case ompt_set_impossible:
            DEBUG_PRINT("OMPT set callback %d returned 'impossible'. The event "
                    "may occur but the tracing of it is not possible.\n", event);
            break;
        case ompt_set_sometimes:
            DEBUG_PRINT("OMPT set callback %d returned 'sometimes'. The event "
                    "may occur and the callback will be invoked at runtime, but "
                    "only for an implementation-defined subset of associated event "
                    "occurrences.\n", event);
            error = 0;
            break;
        case ompt_set_sometimes_paired:
            DEBUG_PRINT("OMPT set callback %d returned 'sometimes paired'. "
                    "The event may occur and the callback will be invoked at "
                    "runtime, but only for an implementation-defined subset of "
                    "associated event occurrences. If any callback is invoked "
                    "with a begin_scope endpoint, it will be invoked also later "
                    "with and end_scope endpoint.\n", event);
            error = 0;
            break;
        case ompt_set_always:
            error = 0;
            break;
        default:
            fprintf(stderr, "Unsupported return code at set_ompt_callback, "
                    "please file a bug report.\n");
    }
    return error;
}

#define register_callback(x)\
        if (callbacks.x) {\
            int error = set_ompt_callback(\
                    ompt_callback_##x,\
                    (ompt_callback_t)callbacks.x);\
            if (error) fprintf(stderr, "Warning: Could not register callback: " #x "\n");\
            global_error += error;\
        }

static callbacks_ompt_t callbacks = {0};
static int register_ompt_callbacks(ompt_function_lookup_t lookup) {

    /* Populate global struct */
		callbacks = generate_callback_table();

    int global_error = 0;
    /* Register callbacks */
    set_callback_fn = (ompt_set_callback_t)lookup("ompt_set_callback");
    if (set_callback_fn) {
				register_callback(parallel_begin)
				register_callback(parallel_end)
				register_callback(sync_region)
				register_callback(implicit_task)
    } else {
        global_error = 1;
        fprintf(stderr, "Could not look up function \"ompt_set_callback\"\n");
    }
    return global_error;
}

#define unregister_callback(x)\
        if (callbacks.x) {\
            set_callback_fn(ompt_callback_##x, NULL);\
        }

static void unregister_ompt_callbacks(void) {
    if (set_callback_fn) {
				unregister_callback(parallel_begin)
				unregister_callback(parallel_end)
				unregister_callback(sync_region)
				unregister_callback(implicit_task)
    }
}

static const char *omp_runtime_version;
static int ompt_initialize(ompt_function_lookup_t lookup, int initial_device_num,
        ompt_data_t *tool_data) {

    /* Print OMPT version */
    DEBUG_PRINT("Detected OpenMP runtime: %s\n", omp_runtime_version);
    DEBUG_PRINT("Initializing OMPT module\n");

    /* Register OMPT callbacks */
    int error = register_ompt_callbacks(lookup);

    /* If callbacks are successfully registered,
     * return a non-zero value to activate the tool */
    if (!error) {
        DEBUG_PRINT("OMPT callbacks succesfully registered\n");
        return 1;
    }
    fprintf(stderr, "Unable to register all OpenMP tool callbacks\n");
    return 1;
}

static void ompt_finalize(ompt_data_t *tool_data) {
    DEBUG_PRINT("Finalizing OMPT module\n");
}

ompt_start_tool_result_t* ompt_start_tool(unsigned int omp_version, const char *runtime_version) {
    omp_runtime_version = runtime_version;
    static ompt_start_tool_result_t ompt_start_tool_result = {
        .initialize = ompt_initialize,
        .finalize   = ompt_finalize,
        .tool_data  = {0}
    };
    return &ompt_start_tool_result;
}

