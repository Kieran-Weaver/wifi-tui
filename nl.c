#include "nl.h"

#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>

void nl_alloc( struct nl* nl ) {
	nl->sock = nl_socket_alloc();
	genl_connect( nl->sock );
	nl->id = genl_ctrl_resolve( nl->sock, "nl80211" );
}

void nl_free( struct nl* nl ) {
	nl_close( nl->sock );
	nl_socket_free( nl->sock );
}
