#include "scan.h"
#include "arena.h"
#include "nl.h"

#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <linux/nl80211.h>

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

static int scan_finished_cb( struct nl_msg* msg, void* arg ) {
	struct genlmsghdr* gnlh = (struct genlmsghdr*)nlmsg_data( nlmsg_hdr( msg ) );
	int* result = (int*)arg;

	switch ( gnlh->cmd ) {
	case NL80211_CMD_NEW_SCAN_RESULTS:
		*result = 1;
		break;
	case NL80211_CMD_SCAN_ABORTED:
		*result = -1;
		break;
	default:
		break;
	}

	return NL_SKIP;
}

static int nl_skip( struct nl_msg* msg, void* arg ) {
	return NL_SKIP;
}

static int nl_stop( struct nl_msg* msg, void* arg ) {
	int* ack = (int*)arg;
	*ack = 0;
	return NL_STOP;
}

static int nl_ok( struct nl_msg* msg, void* arg ) {
	return NL_OK;
}

static int trigger_scan( struct nl_sock* sock, int if_index, int nl80211_id ) {
	struct nl_msg* msg = NULL;
	struct nl_msg* ssids = NULL;
	struct nl_cb* cb = NULL;
	int err;
	int ack_got = 1;
	int mcid = -1;
	int done = 0;

	// Find multicast group for scanning
	mcid = genl_ctrl_resolve_grp( sock, "nl80211", "scan" );
	if ( mcid < 0 ) goto fail;

	// Subscribe to said multicast group
	err = nl_socket_add_membership( sock, mcid );
	if ( err < 0 ) goto fail;

	msg = nlmsg_alloc();
	ssids = nlmsg_alloc();
	cb = nl_cb_alloc( NL_CB_DEFAULT );

	if ( !msg || !ssids || !cb ) goto fail;

	// Build nl80211 message
	genlmsg_put( msg, NL_AUTO_PORT, NL_AUTO_SEQ, nl80211_id, 0, 0, NL80211_CMD_TRIGGER_SCAN, 0 );
	nla_put_u32( msg, NL80211_ATTR_IFINDEX, if_index );
	nla_put( ssids, 1, 0, "" );
	nla_put_nested( msg, NL80211_ATTR_SCAN_SSIDS, ssids );
	nlmsg_free( ssids );
	ssids = NULL;

	// Build callback set
	err = nl_cb_set( cb, NL_CB_VALID, NL_CB_CUSTOM, scan_finished_cb, &done );
	if ( err < 0 ) goto fail;
	err = nl_cb_set( cb, NL_CB_FINISH, NL_CB_CUSTOM, nl_skip, NULL );
	if ( err < 0 ) goto fail;
	err = nl_cb_set( cb, NL_CB_ACK, NL_CB_CUSTOM, nl_stop, &ack_got );
	if ( err < 0 ) goto fail;
	err = nl_cb_set( cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, nl_ok, NULL );
	if ( err < 0 ) goto fail;

	// Trigger the scan
	err = nl_send_auto( sock, msg );
	if ( err < 0 ) goto fail;

	// Wait for scan to complete
	while ( ack_got ) {
		err = nl_recvmsgs( sock, cb );
		if ( err < 0 ) goto fail;
	}

	while ( done == 0 ) {
		nl_recvmsgs( sock, cb );
	}

	if ( done < 0 ) goto fail;

	nlmsg_free( msg );
	nl_cb_put( cb );
	nl_socket_drop_membership( sock, mcid );

	return 0;
fail:
	if ( msg ) nlmsg_free( msg );
	if ( ssids ) nlmsg_free( ssids );
	if ( cb ) nl_cb_put( cb );
	if ( mcid >= 0 ) nl_socket_drop_membership( sock, mcid );
	return 1;
}


int get_ssids( struct ssids* ssids, struct nl* nl, int if_index ) {
	// Not implemented
	trigger_scan( nl->sock, if_index, nl->id );
	return -1;
}
