#ifndef INTF_H
#define INTF_H

struct arena;

struct intf {
	char* name;
	int len;
};

struct intfs {
	int len;
	struct arena* a;
	struct intf *intfs;
};

int get_intfs( struct arena* a, struct intfs *intfs, int max_intfs );

#endif
