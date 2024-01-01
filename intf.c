#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <linux/nl80211.h>
#include <assert.h>

struct intf {
	char* name;
	int len;
};

struct intfs {
	int done; // callback finished?
	int len; // number of interfaces
	struct intf *intfs;
};

int main() {
	struct nl_sock *sock = nl_socket_alloc();
	nl_connect( sock, NETLINK_ROUTE );

	struct rtgenmsg msg = {};
	msg.rtgen_family = AF_PACKET;

	int ret = nl_send_simple( sock, RTM_GETLINK, NLM_F_REQUEST | NLM_F_DUMP, &msg, sizeof( msg ) );
	assert( ret > 0 );

	nl_socket_free( sock );

	return 0;
}
