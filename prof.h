#define _GNU_SOURCE
#define __USE_GNU
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <dlfcn.h>
#include <string.h>

#define MAX_RECORDS 2000

typedef struct {
	void *func_address;
	uint64_t start_time;
} func_record_t;

static __thread int in_profiling = 0;
static __thread func_record_t records[MAX_RECORDS];
static __thread int record_index = 0;
static __thread int depth_level = 0;
static __thread int user_wants_return_printf = 0;

static inline uint64_t get_timestamp() {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (uint64_t)ts.tv_sec * 1000000000 + ts.tv_nsec;
}

const char* function_name(void *func_address) {
	Dl_info info;
	if (dladdr(func_address, &info) && info.dli_sname) {
		return info.dli_sname;
	} else {
		return NULL;
	}
}

void __cyg_profile_func_enter(void *func_address, void *call_site) __attribute__((no_instrument_function));
void __cyg_profile_func_enter(void *func_address, void *call_site) {
	if (in_profiling) return;
	in_profiling = 1;

	if (record_index >= MAX_RECORDS) {
		fprintf(stderr, "Too many functions for me to handle!\n");
		exit(EXIT_FAILURE);
	}
	records[record_index].func_address = func_address;
	records[record_index].start_time = get_timestamp();
	record_index++;

	printf("%*s--> %s %p\n",  depth_level, "", function_name(func_address), __builtin_frame_address(0));

	depth_level++;
	in_profiling = 0;
}

void __cyg_profile_func_exit(void *func_address, void *call_site) __attribute__((no_instrument_function));
void __cyg_profile_func_exit(void *func_address, void *call_site) {
	if (in_profiling) return;
	in_profiling = 1;


	int i;
	for (i = 0; i < record_index; i++) {
		if (records[i].func_address == func_address) {
			break;
		}
	}
	uint64_t end_time = get_timestamp();
	uint64_t duration = 0;

	duration = end_time - records[i].start_time;

	records[i] = records[record_index];
	record_index--;
	depth_level--;
		
	printf("%*s<-- %s %p %" PRIu64 "\n", depth_level, "", function_name(func_address), __builtin_frame_address(0), duration);
	in_profiling = 0;
}
