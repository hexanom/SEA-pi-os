#include "kalloc.h"
#include "vmem.h"

#define KPAGES 1024 // ~4MO

struct fl {
  struct fl  *next;
  uint32  size;
} *freelist = (struct fl *)0;

static uint8* top;
static uint8* heap_limit;

bool kalloc_init(uint8* heap_start, uint32 heap_size) {
  top = heap_start;
  heap_limit = heap_start + heap_size;
  return true;
}

bool kalloc_setup() {
  uint8* heap_start = vmem_page_alloc(KPAGES);
  uint32 heap_size = KPAGES * PAGE_SIZE;
  return kalloc_init(heap_start, heap_size);
}

uint8* kalloc_alloc(uint32 size) {
  register struct fl *f = freelist, **prev;

  prev = &freelist;
  while (f && f->size < size) {
    prev = &f->next;
    f = f->next;
  }
  if (f == (struct fl *)0) {
    f = (struct fl *)top;
    top += (size + 3) & ~3;
    /* No space available anymore */
    if (top > (uint8*)heap_limit)
      return 0;
  } else
    *prev = f->next;

  return ((uint8 *)f);
}

bool kalloc_free(uint8* ptr, uint32 size) {
  register struct fl *f = (struct fl *)ptr;

  f->size = (size + 3) & ~3;
  f->next = freelist;
  freelist = f;
  return true;
}

