#include "arena.h"
#include "scan.h"
#include "intf.h"
#include "nl.h"
#include "ui.h"
#define TB_IMPL
#include <termbox2.h>

#define ARENA_SIZE 65536
#define MAX_INTFS 256

static char passwd[ PASSWD_MAX ];

int main( int argc, char *argv[]) {
	struct intfs intfs;
	struct ssids ssids;
	struct ui ui;
	struct intf curr;
	struct nl nl;
	struct arena arena = new_arena( ARENA_SIZE );

	struct arena a = arena;

	nl_alloc( &nl );
	int len = get_intfs( &a, &nl, &intfs, MAX_INTFS );

	struct tb_event ev;
	int mode = MODE_INTF;
	int code;

	ui.changed = false;
	ui.selection = 0;
	ui.all_intfs = &intfs;
	ui.all_ssids = &ssids;
	ui.chosen_passwd.data = passwd;
	ui.chosen_passwd.len = 0;

	ui_init( &ui );
	ui_draw( &ui, mode );

	while (1) {
		if ( tb_poll_event(&ev) != TB_OK ) continue;
		if ( ev.type == TB_EVENT_KEY ) {
			code = ui_handle_key( &ui, &ev, mode );
			if ( code == EV_SELECT ) {
				switch (mode) {
				case MODE_INTF:
					ui.chosen_intf = intfs.intfs[ ui.selection ];
					mode = MODE_SSID;
					break;
				case MODE_SSID:
					ui.chosen_ssid = ssids.ssids[ ui.selection ];
					mode = MODE_PASS;
				case MODE_PASS:
					code = EV_EXIT;
					break;
				default:
					break;
				}
			}

			if ( code == EV_EXIT ) break;
		}
		ui_draw( &ui, mode );
	}

	tb_shutdown();
	nl_free( &nl );
	free_arena( &arena );

	return 0;
}
