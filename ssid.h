#ifndef SSID_H
#define SSID_H

struct arena;
struct ssid_set;

int ssid_valid( const char* ssid, int len );

// Detect duplicate SSIDs
struct ssid_set* alloc_ssids( struct arena* a, int max_ssids );

// Returns 0 if ssid already seen, 1 otherwise
int ssid_insert( struct ssid_set* ssids, const char* ssid, int len );

#endif
