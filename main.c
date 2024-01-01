#include "arena.h"
#include "intf.h"
#include <stdio.h>

#define ARENA_SIZE 65536
#define MAX_INTFS 256

int main( int argc, char *argv[]) {
	struct intfs intfs;
	struct arena arena = new_arena( ARENA_SIZE );

	struct arena a = arena;
	int len = get_intfs( &a, &intfs, MAX_INTFS );

	for ( int i = 0; i < len; i++ ) {
		printf( "%s\n", intfs.intfs[ i ].name );
	}

	free_arena( &arena );

	return 0;
}
