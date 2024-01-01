#include "arena.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

struct arena new_arena( ptrdiff_t capacity ) {
	struct arena a;
	a.beg = malloc( capacity );
	a.end = a.beg ? a.beg + capacity : 0;
	return a;
}

void free_arena( struct arena* a ) {
	free( a->beg );
}

void *arena_alloc(struct arena *a, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count) {
	ptrdiff_t padding = -(uintptr_t)a->beg & (align - 1);
	ptrdiff_t available = a->end - a->beg - padding;
	if (available < 0 || count > available/size) {
		abort();
	}
	void *p = a->beg + padding;
	a->beg += padding + count*size;
	return memset(p, 0, count*size);
}
