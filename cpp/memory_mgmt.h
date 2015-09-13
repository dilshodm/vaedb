#ifdef VAE_USE_GC
#ifndef GC_THREADS
#define GC_THREADS
#endif

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <gc.h>

#define malloc GC_malloc
#define free GC_free
#endif // VAE_USE_GC

void memory_mgmt_init(void);
