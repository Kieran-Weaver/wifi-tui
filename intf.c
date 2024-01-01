#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <linux/nl80211.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdatomic.h>

struct intf {
	char* name;
	int len;
};

struct intfs {
	volatile int done; // callback finished?
	int len; // number of interfaces
	struct intf *intfs;
};

static int intf_cb( struct nl_msg *msg, void* ctx ) {
	struct intfs *intfs = (struct intfs*) ctx;
	if ( intfs->done ) {
		return NL_OK;
	}

	atomic_store( &intfs->done, true );

	return NL_OK;
}

int main() {
	struct nl_sock *sock = nl_socket_alloc();
	struct intfs *intfs = calloc( 1, sizeof( struct intfs ) );

	nl_connect( sock, NETLINK_ROUTE );

	struct rtgenmsg msg = {};
	msg.rtgen_family = AF_PACKET;

	int ret = nl_send_simple( sock, RTM_GETLINK, NLM_F_REQUEST | NLM_F_DUMP, &msg, sizeof( msg ) );
	assert( ret > 0 );

	nl_socket_modify_cb( sock, NL_CB_VALID, NL_CB_CUSTOM, intf_cb, intfs );
	nl_recvmsgs_default( sock );
	assert( intfs->done );

	free( intfs );
	nl_socket_free( sock );

	return 0;
}
