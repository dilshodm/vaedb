#ifdef VAE_USE_GC
#ifndef GC_THREADS
#define GC_THREADS
#endif

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <gc.h>

#define free memory_mgmt_free
#define malloc GC_malloc
#define strdup memory_mgmt_strdup
#endif // VAE_USE_GC

char *memory_mgmt_strdup(const char *s);
void memory_mgmt_free(void *ptr);
void memory_mgmt_init(void);
