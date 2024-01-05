#ifndef UI_H
#define UI_H

#include <stdbool.h>
#include "intf.h"
#include "scan.h"
#define PASSWD_MAX 64

struct tb_event;

struct str {
	char* data;
	int len;
};

enum mode {
	MODE_INTF = 0,
	MODE_SSID,
	MODE_PASS,
};

enum event {
	EV_EXIT = 0,
	EV_OK,
	EV_SELECT,
};

struct ui {
	bool changed;
	int selection;
	struct intf chosen_intf;
	struct ssid chosen_ssid;
	struct str chosen_passwd;
	struct intfs * all_intfs;
	struct ssids * all_ssids;
};

int ui_init( struct ui* ui );
int ui_handle_key( struct ui* ui, struct tb_event* key, int mode );
int ui_draw( struct ui* ui, int mode );
int ui_shut( struct ui* ui );

#endif
