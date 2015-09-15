#ifndef __MEMORY_MGMT_H__
#define __MEMORY_MGMT_H__

#ifdef VAE_USE_GC
#ifndef GC_THREADS
#define GC_THREADS
#endif

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <gc.h>
#include <gc_cpp.h>

void *memory_mgmt_malloc(size_t size);
void *memory_mgmt_malloc_atomic(size_t size);
void *memory_mgmt_realloc(void *mem, size_t size);
char *memory_mgmt_strdup(const char *s);
void memory_mgmt_free(void *ptr);

#else // VAE_USE_GC

#define memory_mgmt_malloc malloc
#define memory_mgmt_malloc_atomic malloc
#define memory_mgmt_realloc realloc
#define memory_mgmt_strdup strdup
#define memory_mgmt_free free

#endif // VAE_USE_GC

void memory_mgmt_init(void);

#endif // __MEMORY_MGMT_H__
