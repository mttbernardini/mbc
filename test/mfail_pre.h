#ifndef MFAIL_PRE_H_INCLUDED
#define MFAIL_PRE_H_INCLUDED

#include <stdlib.h>

void *control_malloc(size_t size) {
	return NULL;
}

void *control_calloc(size_t nmemb, size_t size) {
	return NULL;
}

void *control_realloc(void *ptr, size_t size) {
	return NULL;
}

#define MFAIL_H_REPLACE

#ifdef MFAIL_H_REPLACE
#define malloc(s) control_malloc(s)
#define calloc(n,s) control_calloc(n,s)
#define realloc(p,s) control_realloc(p,s)
#endif

#endif
