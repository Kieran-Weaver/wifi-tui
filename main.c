#include "arena.h"
#include "intf.h"
#include "nl.h"
#include <stdio.h>

#define ARENA_SIZE 65536
#define MAX_INTFS 256

int main( int argc, char *argv[]) {
	struct intfs intfs;
	struct nl nl;
	struct arena arena = new_arena( ARENA_SIZE );

	struct arena a = arena;

	nl_alloc( &nl );
	int len = get_intfs( &a, &nl, &intfs, MAX_INTFS );

	for ( int i = 0; i < len; i++ ) {
		printf( "Intf: %s, Idx: %d\n", intfs.intfs[ i ].name, intfs.intfs[ i ].idx );
	}

	nl_free( &nl );
	free_arena( &arena );

	return 0;
}
