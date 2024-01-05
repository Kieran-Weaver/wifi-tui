#ifndef INTF_H
#define INTF_H

struct arena;
struct nl;

struct intf {
	char* name;
	int idx; /* Internal libnl index */
	int len;
};

struct intfs {
	int len;
	int max_intfs;
	struct arena* a;
	struct intf *intfs;
};

int get_intfs( struct arena* a, struct nl* nl, struct intfs *intfs, int max_intfs );

#endif
