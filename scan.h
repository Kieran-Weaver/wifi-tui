#ifndef SCAN_H
#define SCAN_H

struct arena;
struct nl;
struct ssid_set;

struct ssid {
	char* name;
	int len;
	int strength;
};

struct ssids {
	int len;
	int max_ssids;
	struct arena* a;
	struct ssid * ssids;

	struct ssid_set * internal;
};

struct ssids* make_ssids( struct arena* a, int max_ssids );
int get_ssids( struct ssids* ssids, struct nl* nl, int if_index );

#endif
