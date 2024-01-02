#ifndef NL_H
#define NL_H

struct nl {
	struct nl_sock* sock;
	int id;
};

void nl_alloc( struct nl* nl );
void nl_free( struct nl* nl );

#endif
