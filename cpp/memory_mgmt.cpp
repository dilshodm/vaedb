#include <malloc.h>
#include <string.h>
#include <libxml/xpath.h>

#include "logger.h"
#include "memory_mgmt.h"

#ifdef VAE_USE_GC
void *memory_mgmt_malloc(size_t size) {
  return GC_MALLOC(size);
}

void *memory_mgmt_malloc_atomic(size_t size) {
  return GC_MALLOC_ATOMIC(size);
}

void *memory_mgmt_realloc(void *mem, size_t size) {
  return GC_REALLOC(mem, size);
}

char *memory_mgmt_strdup(const char *s) {
  int len = strlen(s);
  char *buf = (char *) GC_MALLOC_ATOMIC(len + 1);
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
  GC_FREE(ptr);
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
  GC_expand_hp(64 << 20); // crashes without this; who knows why?
  if (xmlGcMemSetup(memory_mgmt_free, memory_mgmt_malloc, memory_mgmt_malloc_atomic, memory_mgmt_realloc, memory_mgmt_strdup) != 0) {
    L(error) << "Error setting up GC with xmlGcMemSetup";
    abort();
  }
#endif
}
