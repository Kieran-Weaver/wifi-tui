#include "intf.h"
#include "arena.h"
#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <linux/nl80211.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <string.h>

static void add_intf( struct intfs* intfs, int i, const char* name ) {
	int len = strlen( name );

	intfs->intfs[ i ].len = len;
	intfs->intfs[ i ].name = NEW( intfs->a, char, len + 1 );
	memcpy( intfs->intfs[ i ].name, name, len );
	intfs->intfs[ i ].name[ len ] = '\0';
}

static int intf_cb( struct nl_msg *msg, void* ctx ) {
	struct intfs *intfs = (struct intfs*) ctx;

	struct nlmsghdr *nl_hdr = nlmsg_hdr( msg );
	struct ifinfomsg *intf = NLMSG_DATA( nl_hdr );
	struct rtattr *rta = IFLA_RTA( intf );
	int bytes_left = nl_hdr->nlmsg_len - NLMSG_LENGTH(sizeof(*intf));

	for ( ; RTA_OK( rta, bytes_left ) ; rta = RTA_NEXT( rta, bytes_left ) ) {
		if ( rta->rta_type == IFLA_IFNAME ) {
			const char* name = RTA_DATA( rta );
			add_intf( intfs, intfs->len, name );
			intfs->len++;
		}
	}

	return NL_OK;
}

int get_intfs( struct arena *a, struct intfs *intfs, int max_intfs ) {
	struct nl_sock *sock = nl_socket_alloc();

	intfs->len = 0;
	intfs->a = a;
	intfs->intfs = NEW( intfs->a, struct intf, max_intfs );

	nl_connect( sock, NETLINK_ROUTE );

	struct rtgenmsg msg = {};
	msg.rtgen_family = AF_PACKET;

	int ret = nl_send_simple( sock, RTM_GETLINK, NLM_F_REQUEST | NLM_F_DUMP, &msg, sizeof( msg ) );
	assert( ret > 0 );

	nl_socket_modify_cb( sock, NL_CB_VALID, NL_CB_CUSTOM, intf_cb, intfs );
	nl_recvmsgs_default( sock );

	nl_socket_free( sock );

	return intfs->len;
}
