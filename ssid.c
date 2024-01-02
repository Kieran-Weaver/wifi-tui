#include "ssid.h"
#include "arena.h"
#include <ctype.h>
#include <stdint.h>

// SSIDs are considered "valid" if they contain:
//   at least 1 printing character that is not a space
//   zero non-printing characters
int ssid_valid( const char* ssid, int len ) {
	for (int i = 0; i < len; i++) {
		if (isspace( ssid[i] )) continue;
		else if (isprint( ssid[i] )) return 1;
		else return 0;
	}
	
	return 0;
}

struct ssid_set {
	int len;
	int capacity;
	uint64_t* ssids;
};

struct ssid_set* alloc_ssids( struct arena* a, int max_ssids ) {
	struct ssid_set* ssids = NEW(a, struct ssid_set);

	ssids->len = 0;
	ssids->capacity = max_ssids;
	ssids->ssids = NEW(a, uint64_t, max_ssids);

	return ssids;
}

// FNV-1a
static uint64_t ssid_hash( const char* ssid, int len ) {
	uint64_t h = 0x100;
	for ( int i = 0; i < len; i++ ) {
		h ^= ssid[ i ] & 255;
		h *= 1111111111111111111;
	}

	return h ^ h>>32;
}

int ssid_insert( struct ssid_set* ssids, const char* ssid, int len ) {
	uint64_t shash = ssid_hash( ssid, len );

	for ( int i = 0; i < ssids->len; i++ ) {
		if ( ssids->ssids[ i ] == shash ) return 0;
	}

	ssids->ssids[ ssids->len ] = shash;

	return 1;
}
