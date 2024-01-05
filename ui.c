#include "ui.h"
#include "termbox2.h"
#include "scan.h"
#include "intf.h"
#include <stdio.h>

int ui_init( struct ui* ui ) {
	int ret;

	ret = tb_init();
	if (ret) return ret;

	tb_set_input_mode( TB_INPUT_ESC );
	tb_clear();

	return 0;
}

int ui_handle_key( struct ui* ui, struct tb_event* ev, int mode ) {
	int max = 0;
	int keycode = -1;
	int code = EV_OK;

	switch ( mode ) {
	case MODE_INTF:
		max = ui->all_intfs->len - 1;
		break;
	case MODE_SSID:
		max = ui->all_ssids->len - 1;
		break;
	default:
		max = 0;
	}

	switch ( ev->key ) {
	case TB_KEY_CTRL_C:
		printf(" EXIT \n");
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
	case TB_KEY_BACKSPACE:
		if ( ui->chosen_passwd.len ) {
			ui->chosen_passwd.len--;
			ui->chosen_passwd.data[ ui->chosen_passwd.len ] = '\0';
		}
		break;
	default:
		if ( ( mode == MODE_PASS ) && ( ev->ch ) ) {
			keycode = ev->ch;
		}
		break;
	}

	if ( ui->selection >= max ) ui->selection = max;
	else if ( ui->selection < 0 ) ui->selection = 0;

	// WPA2 passkeys are ASCII only
	if ( ( mode == MODE_PASS ) && ( keycode >= 32 ) && ( keycode <= 126 ) ) {
		if ( ui->chosen_passwd.len < PASSWD_MAX ) {
			ui->chosen_passwd.data[ ui->chosen_passwd.len ] = keycode;
			ui->chosen_passwd.len++;
		}
	}

	return code;
}

#define BOX_TL_CORNER  0x250C
#define BOX_TR_CORNER  0x2510
#define BOX_BL_CORNER  0x2514
#define BOX_BR_CORNER  0x2518
#define BOX_VERTICAL   0x2502
#define BOX_HORIZONTAL 0x2500

static void ui_draw_box( int x1, int y1, int x2, int y2, uint16_t fg, uint16_t bg ) {
	tb_set_cell( x1, y1, BOX_TL_CORNER, fg, bg );
	tb_set_cell( x1, y2, BOX_BL_CORNER, fg, bg );
	tb_set_cell( x2, y1, BOX_TR_CORNER, fg, bg );
	tb_set_cell( x2, y2, BOX_BR_CORNER, fg, bg );
	for ( int x = x1 + 1; x < x2; x++ ) {
		tb_set_cell( x, y1, BOX_HORIZONTAL, fg, bg );
		tb_set_cell( x, y2, BOX_HORIZONTAL, fg, bg );
	}
	for ( int y = y1 + 1; y < y2; y++ ) {
		tb_set_cell( x1, y, BOX_VERTICAL, fg, bg );
		tb_set_cell( x2, y, BOX_VERTICAL, fg, bg );
	}
}

#define DRAW_LIST( ITEM ) \
	for ( int i = 0; i < ui->all_##ITEM##s->len; i++ ) { \
		const char* str = ui->all_##ITEM##s->ITEM##s[ i ].name; \
		if ( i == ui->selection ) { \
			fg = TB_BLACK; \
			bg = TB_WHITE; \
		} else { \
			fg = TB_WHITE; \
			bg = TB_BLACK; \
		} \
		tb_printf( sidebar_width + 2, line++, fg, bg, "%s", str ); \
	}

int ui_draw( struct ui* ui, int mode ) {
	int width = tb_width() - 1;
	int height = tb_height() - 1;
	uint16_t fg, bg;

	int sidebar_width = width / 3;
	if ( sidebar_width < 32 ) sidebar_width = 32;

	tb_clear();
	// sidebar
	ui_draw_box( 0, 0, sidebar_width, height, TB_WHITE, TB_BLACK );

	int line = 3;
	if ( ( mode == MODE_SSID ) || ( mode == MODE_PASS ) ) {
		// Interface selected
		tb_print( 2, line++, TB_WHITE, TB_DEFAULT, "Interface: " );
		tb_printf( 2, line++, TB_WHITE, TB_DEFAULT, "%s", ui->chosen_intf.name );
	}

	if ( mode == MODE_PASS ) {
		// SSID Selected
		tb_print( 2, line++, TB_WHITE, TB_DEFAULT, "SSID: " );
		tb_printf( 2, line++, TB_WHITE, TB_DEFAULT, "%s", ui->chosen_ssid.name );
	}

	// main box
	ui_draw_box( sidebar_width, 0, width, height, TB_WHITE, TB_BLACK );

	line = 3;
	switch ( mode ) {
	case MODE_INTF:
		tb_print( sidebar_width + 2, line++, TB_WHITE, TB_BLACK, "Choose Interface: " );
		DRAW_LIST( intf );
		break;
	case MODE_SSID:
		tb_print( sidebar_width + 2, line++, TB_WHITE, TB_BLACK, "Choose SSID: " );
		DRAW_LIST( ssid );
		break;
	case MODE_PASS:
		tb_print( sidebar_width + 2, line++, TB_WHITE, TB_BLACK, "Password: " );
		break;
	}

	tb_present();
}
