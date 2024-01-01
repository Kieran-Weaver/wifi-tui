#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>
#include <stdalign.h>

// Arena design from https://nullprogram.com/blog/2023/09/27/

struct arena {
	char *beg;
	char *end;
};

#define NEW(...)            NEWX(__VA_ARGS__,NEW3,NEW2)(__VA_ARGS__)
#define NEWX(a,b,c,d,...) d
#define NEW2(a, t)          (t *)arena_alloc(a, sizeof(t), alignof(t), 1)
#define NEW3(a, t, n)       (t *)arena_alloc(a, sizeof(t), alignof(t), n)

#define sizeof(x) (ptrdiff_t)sizeof(x)
#define countof(a) (sizeof(a) / sizeof(*(a)))
#define lengthof(s) (countof(s) - 1)

__attribute((malloc, alloc_size(2, 4), alloc_align(3)))
void* arena_alloc( struct arena* a, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count );

struct arena new_arena( ptrdiff_t capacity );
void free_arena( struct arena* a );

#endif
