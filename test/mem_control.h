#ifndef MEM_CONTROL_H_INCLUDED
#define MEM_CONTROL_H_INCLUDED

#include <stdlib.h>
#include <stdbool.h>

bool MEM_CONTROL_FAIL_NEXT = false;

void *control_malloc(size_t size) {
	if (MEM_CONTROL_FAIL_NEXT) {
		MEM_CONTROL_FAIL_NEXT = false;
		return NULL;
	}

	return malloc(size);
}

void *control_calloc(size_t nmemb, size_t size) {
	if (MEM_CONTROL_FAIL_NEXT) {
		MEM_CONTROL_FAIL_NEXT = false;
		return NULL;
	}

	return calloc(nmemb, size);
}

void *control_realloc(void *ptr, size_t size) {
	if (MEM_CONTROL_FAIL_NEXT) {
		MEM_CONTROL_FAIL_NEXT = false;
		return NULL;
	}

	return realloc(ptr, size);
}

#define malloc(s) control_malloc(s)
#define calloc(n, s) control_calloc(n, s)
#define realloc(p, s) control_realloc(p, s)

#endif
