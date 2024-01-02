#ifndef SCAN_H
#define SCAN_H

struct arena;
struct nl;

struct ssid {
	char* name;
	int len;
};

struct ssids {
	int len;
	struct arena* a;
	struct ssid * ssids;
};

struct ssids* make_ssids( struct arena* a, int max_ssids );
int get_ssids( struct ssids* ssids, struct nl* nl, int if_index );

#endif
