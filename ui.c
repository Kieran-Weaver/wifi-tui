#include "ui.h"
#include "termbox2.h"
#include "scan.h"
#include "intf.h"

int ui_init( struct ui* ui ) {
	int ret;

	ret = tb_init();
	if (ret) return ret;

	tb_set_input_mode( TB_INPUT_ESC );

	return 0;
}

int ui_handle_key( struct ui* ui, int key, int mode ) {
	int max = 0;
	int code = EV_OK;

	switch ( mode ) {
	case MODE_INTF:
		max = ui->all_intfs->len;
		break;
	case MODE_SSID:
		max = ui->all_ssids->len;
		break;
	default:
		max = 0;
	}

	switch ( key ) {
	case TB_KEY_CTRL_C:
		code = EV_EXIT;
		break;
	case TB_KEY_ARROW_UP:
		ui->selection--;
		break;
	case TB_KEY_ARROW_DOWN:
		ui->selection++;
		break;
	case TB_KEY_ENTER:
		code = EV_SELECT;
		break;
	}

	if ( ui->selection > max ) ui->selection = max;
	else if ( ui->selection < 0 ) ui->selection = 0;

	return code;
}

int ui_draw( struct ui* ui, int mode ) {}
