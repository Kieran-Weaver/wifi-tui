#include "scan.h"
#include "arena.h"
#include "nl.h"

/*
struct ssid {
        char* name;
        int len;
};

struct ssids {
        int len;
        struct arena* a;
        struct ssid * ssids;
};
*/

struct ssids* make_ssids( struct arena* a, int max_ssids ) {
	struct ssids* ssids = NEW( a, struct ssids );

	ssids->len = 0;
	ssids->a = a;
	ssids->ssids = NEW( a, struct ssid, max_ssids );

	return ssids;
}

int get_ssids( struct ssids* ssids, struct nl* nl, int if_index ) {
	// Not implemented
	return -1;
}
