#include "intf.h"
#include "arena.h"
#include "nl.h"
#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <linux/nl80211.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <string.h>

static void add_intf( struct intfs* intfs, int i, const char* name, int idx ) {
	int len = strlen( name );

	intfs->intfs[ i ].len = len;
	intfs->intfs[ i ].name = NEW( intfs->a, char, len + 1 );
	memcpy( intfs->intfs[ i ].name, name, len );
	intfs->intfs[ i ].name[ len ] = '\0';
	intfs->intfs[ i ].idx = idx;
}

static int intf_cb( struct nl_msg *msg, void* ctx ) {
	struct intfs *intfs = (struct intfs*) ctx;
	struct genlmsghdr *gnl_hdr = NLMSG_DATA( nlmsg_hdr( msg ) );
	struct nlattr *tb_msg[ NL80211_ATTR_MAX + 1 ];

	if ( intfs->len >= intfs->max_intfs ) return NL_SKIP;

	const char* name;
	unsigned int idx = -1;

	nla_parse( tb_msg, 
		   NL80211_ATTR_MAX,
		   genlmsg_attrdata( gnl_hdr, 0 ),
		   genlmsg_attrlen( gnl_hdr, 0 ),
		   NULL );

	if ( tb_msg[ NL80211_ATTR_IFNAME ] ) {
		name = nla_get_string( tb_msg[ NL80211_ATTR_IFNAME ] );
		if ( tb_msg[ NL80211_ATTR_IFINDEX ] ) {
			idx = nla_get_u32( tb_msg[ NL80211_ATTR_IFINDEX ] );
		}
		add_intf( intfs, intfs->len, name, idx );
		intfs->len++;
	}

	return NL_SKIP;
}

int get_intfs( struct arena *a, struct nl* nl, struct intfs *intfs, int max_intfs ) {
	int res;

	intfs->len = 0;
	intfs->a = a;
	intfs->intfs = NEW( intfs->a, struct intf, max_intfs );
	intfs->max_intfs = max_intfs;

	struct nl_cb* cb = nl_cb_alloc( NL_CB_DEFAULT );
	nl_cb_set( cb, NL_CB_VALID, NL_CB_CUSTOM, intf_cb, intfs );

	struct nl_msg* msg = nlmsg_alloc();
	genlmsg_put( msg, NL_AUTO_PORT, NL_AUTO_SEQ, nl->id, 0, NLM_F_DUMP, NL80211_CMD_GET_INTERFACE, 0 );

	res = nl_send_auto( nl->sock, msg );
	assert( res > 0 );

	res = nl_recvmsgs( nl->sock, cb );
	assert( !res );

	free( cb );
	nlmsg_free( msg );

	return intfs->len;
}
