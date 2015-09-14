#include <malloc.h>
#include <string.h>
#include <libxml/xpath.h>

#include "logger.h"
#include "memory_mgmt.h"

#ifdef VAE_USE_GC
char *memory_mgmt_strdup(const char *s) {
  int len = strlen(s);
  char *buf = (char *) GC_malloc_atomic(len + 1);
  if (buf == NULL) {
    L(error) << "local_gc_strdup malloc fail";
    abort();
  }
  strncpy(buf, s, len);
  buf[len] = '\0';
  return buf;
}

// No-op free()
void memory_mgmt_free(void *ptr) {
  return;
}
#endif

void memory_mgmt_init(void) {
  //abuse of libxml internal pointers results in doublefree;
#ifdef M_CHECK_ACTION
  mallopt(M_CHECK_ACTION, 0);
#endif

#ifdef VAE_USE_GC
  // Setup GC for libxml2.
  GC_INIT();
  if (xmlGcMemSetup(memory_mgmt_free, GC_malloc, GC_malloc_atomic, GC_realloc, memory_mgmt_strdup) != 0) {
    L(error) << "Error setting up GC with xmlGcMemSetup";
    abort();
  }
#endif
}
