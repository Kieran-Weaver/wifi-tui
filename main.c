#include "arena.h"
#include "intf.h"
#include "nl.h"
#include "ui.h"
#define TB_IMPL
#include <termbox2.h>

#define ARENA_SIZE 65536
#define MAX_INTFS 256

int main( int argc, char *argv[]) {
	struct intfs intfs;
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
	ui.all_ssids = NULL;

	tb_init();

	while (tb_poll_event(&ev) == TB_OK) {
		if ( ev.type == TB_EVENT_KEY ) {
			code = ui_handle_key( &ui, ev.key, mode );

			if ( code == EV_EXIT ) break;

			if ( code == EV_SELECT ) {
				switch (mode) {
				case MODE_INTF:
					curr = intfs.intfs[ ui.selection ];
					printf( "Intf: %s\n", curr.name );
					break;
				default:
					break;
				}
			}

		}
		tb_clear();
		ui_draw( &ui, mode );
		tb_present();
	}

	tb_shutdown();
	nl_free( &nl );
	free_arena( &arena );

	return 0;
}
