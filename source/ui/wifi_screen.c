/*
 * wifi_screen.cpp
 *
 *  Created on: Feb 28, 2025
 *      Author: Masha
 */

#include "wifi_screen.h"

#include "FreeRTOS.h"
#include "task.h"

#include "fsl_device_registers.h"
#include "fsl_gpio.h"
#include "fsl_iomuxc.h"
#include "fsl_debug_console.h"
#include "fsl_lpi2c.h"
#include "lvgl_support.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "lvgl.h"
#include "src/common.h"
#include "tasks/wifi_tasks.h"
#include <stdio.h>

Screen_t WifiScreen = {WiFiScreenInit, WiFiScreenProcess, WiFiScreenDeinit, &wifi_screen};

lv_obj_t *wifi_screen;
lv_obj_t *ssid_input;
lv_obj_t *password_input;
lv_obj_t *connect_btn;
lv_obj_t *wifi_list;
lv_obj_t *status_label;

int wifi_count = 0;
int wifi_screen_k = 4;

char wifi_networks[MAX_WIFI_NETWORKS][32] = {
	    "Network_1", "Network_2", "Network_3", "Network_4", "Network_5"};
lv_obj_t *wifi_btns[MAX_WIFI_NETWORKS] = {0};
char connected_ssid[32] = "Network_2";
TickType_t last_update_time = 0;

// UI settings
// ------------------------------------------------------------------------------
void WiFiScreenInit(void) {
    wifi_screen = lv_obj_create(NULL);
	lv_obj_set_style_pad_all(wifi_screen, 5, 0); // Add padding around the screen

	last_update_time = 0;
	wifi_count = 0;
	memset(connected_ssid, 0, sizeof connected_ssid);
	memset(wifi_btns, 0, sizeof wifi_btns);
	memset(wifi_networks, 0, sizeof wifi_networks);


	// Create a parent container with a grid layout
	static lv_coord_t grid_template_rows[] = {LV_GRID_FR(1), LV_GRID_FR(3), LV_GRID_TEMPLATE_LAST};
	static lv_coord_t grid_template_cols[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
	lv_obj_t *main_container = lv_obj_create(wifi_screen);
	lv_obj_set_size(main_container, lv_pct(100), lv_pct(100));
	lv_obj_set_layout(main_container, LV_LAYOUT_GRID);
	lv_obj_set_grid_dsc_array(main_container, grid_template_cols, grid_template_rows);

	// Set a border and padding for visualization
	lv_obj_set_style_border_width(main_container, 1, 0);
	lv_obj_set_style_border_color(main_container, lv_color_black(), 0);
	lv_obj_set_style_pad_all(main_container, 5, 0);

	// Create the common info section (row 0, span all columns)
	lv_obj_t *header_area = lv_obj_create(main_container);
	lv_obj_set_grid_cell(header_area, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
	lv_obj_set_style_bg_color(header_area, lv_color_make(0xEE, 0xEE, 0xEE), 0); // Background color
	lv_obj_set_style_pad_all(header_area, 10, 0);

    lv_obj_t *wifi_label = lv_label_create(header_area);
    lv_label_set_text(wifi_label, "WiFi Settings");
    lv_obj_align(wifi_label, LV_ALIGN_CENTER, 0, 0);
	status_label = lv_label_create(header_area);
	lv_label_set_text(status_label, "LOADING...");
	lv_obj_align(status_label, LV_ALIGN_RIGHT_MID, 0, 0);

	static lv_coord_t plant_grid_rows[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
	static lv_coord_t plant_grid_cols[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
	lv_obj_t *wifi_grid = lv_obj_create(main_container);
	lv_obj_set_grid_cell(wifi_grid, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
	lv_obj_set_grid_dsc_array(wifi_grid, plant_grid_cols, plant_grid_rows);
	lv_obj_set_style_pad_all(wifi_grid, 5, 0);

    wifi_list = lv_list_create(wifi_grid);
	lv_obj_set_grid_cell(wifi_list, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
	lv_obj_set_scrollbar_mode(wifi_list, LV_SCROLLBAR_MODE_AUTO);

	lv_obj_t *input_area = lv_obj_create(wifi_grid);
	lv_obj_set_grid_cell(input_area, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);

    ssid_input = lv_textarea_create(input_area);
	lv_obj_set_size(ssid_input, lv_pct(80), lv_pct(30));
    lv_textarea_set_placeholder_text(ssid_input, "WiFi Name");
    lv_obj_align(ssid_input, LV_ALIGN_TOP_LEFT, 0, 0);

    password_input = lv_textarea_create(input_area);
	lv_obj_set_size(password_input, lv_pct(80), lv_pct(30));
    lv_textarea_set_placeholder_text(password_input, "Password");
    lv_obj_align(password_input, LV_ALIGN_LEFT_MID, 0, 0);
    lv_textarea_set_password_mode(password_input, true);

    connect_btn = lv_btn_create(input_area);
    lv_obj_align(connect_btn, LV_ALIGN_BOTTOM_LEFT, 0, 0);
	lv_obj_set_size(connect_btn, lv_pct(50), lv_pct(30));
    lv_obj_t *btn_label = lv_label_create(connect_btn);
    lv_label_set_text(btn_label, "Connect");

    char AT_command3[] = "AT+CWLAPOPT=1,6,-90\r\n";
    xQueueSend(xQueueWifiCommand, (void*)&AT_command3, (TickType_t )0);
}

void WifiScreen_UpdateSelectionHighlight() {
	// Default highlight.
	lv_obj_set_style_bg_color(ssid_input, lv_color_white(), 0);
	lv_obj_set_style_bg_color(password_input, lv_color_white(), 0);
	lv_obj_set_style_bg_color(connect_btn, lv_color_make(0x66, 0x99, 0xCC), 0);
	for (int i = 0; i < wifi_count; i++) {
		if (wifi_btns[i] != NULL) {
			lv_obj_set_style_bg_color(wifi_btns[i], lv_color_white(), 0);
		}
	}

	// Focused element highlight.
	if ((focus >= 1) && (focus <= wifi_count)) {
		lv_obj_set_style_bg_color(wifi_btns[focus - 1], lv_color_make(0xc0, 0xc0, 0xc0), 0);
		lv_obj_scroll_to_view(wifi_btns[focus - 1], LV_ANIM_ON);
		if (strcmp(wifi_networks[focus - 1], connected_ssid) == 0) {
			lv_obj_set_style_text_color(wifi_btns[focus - 1], Green(), 0);
		}
	} else if (focus == wifi_count + 1) {
		lv_obj_set_style_bg_color(ssid_input, lv_color_make(0xb3, 0xce, 0xe5), 0);
	} else if (focus == wifi_count + 2) {
		lv_obj_set_style_bg_color(password_input, lv_color_make(0xb3, 0xce, 0xe5), 0);
	} else if (focus == wifi_count + 3) {
		lv_obj_set_style_bg_color(connect_btn, lv_color_make(0x00, 0x88, 0x88), 0);
	}
}

// Wifi requests
// ------------------------------------------------------------------------------
void ConnectToWifi() {
    char command1[] = "AT+CWQAP\r\n";
    xQueueSend(xQueueWifiCommand, (void*)&command1, (TickType_t )0);

	char command2[64];
	snprintf(command2, sizeof(command2), "AT+CWJAP=\"%s\",\"%s\"\r\n",
			lv_textarea_get_text(ssid_input),
			lv_textarea_get_text(password_input));
	xQueueSend(xQueueWifiCommand, (void*)&command2, (TickType_t )0);
	PRINTF("Connecting to wifi...\r\n");
}

void CheckConnection(void) {
    char command[] = "AT+CWSTATE?\r\n";
    xQueueSend(xQueueWifiCommand, (void*)&command, (TickType_t )0);
}

void UpdateWifiNetworks() {
    TickType_t current_time = xTaskGetTickCount();
    if (last_update_time == 0 || current_time - last_update_time > pdMS_TO_TICKS(10000)) {
    	CheckConnection();
        char command[] = "AT+CWLAP\r\n";
        xQueueSend(xQueueWifiCommand, (void*)&command, (TickType_t )0);
        last_update_time = current_time;

    	lv_obj_t *loading = lv_list_add_btn(wifi_list, NULL, "loading...");
    	lv_obj_move_foreground(loading);
    }
}

void ResetWifi() {
    char command1[] = "AT+CWMODE=1\r\n";
    xQueueSend(xQueueWifiCommand, (void*)&command1, (TickType_t )0);
    char command2[] = "AT+RST\r\n";
    xQueueSend(xQueueWifiCommand, (void*)&command2, (TickType_t )0);
}

// UI updates logic
// ------------------------------------------------------------------------------
void WifiScreen_ProcessKeyboardInput(Key_t key) {
	int ssid_input_focus = wifi_count + 1;
	int password_input_focus = wifi_count + 2;
	int connect_wifi_focus = wifi_count + 3;

	if (text_input_mode) {
		if (focus == ssid_input_focus) {
			EnterText(key, ssid_input);
		} else if (focus == password_input_focus) {
			EnterText(key, password_input);
		}
		return;
	}
    switch (KeyFunc(key.key)) {
        case KEY_UP:
        	focus = (focus - 1 + wifi_screen_k) % wifi_screen_k;
            break;
        case KEY_DOWN:
        	focus = (focus + 1) % wifi_screen_k;
            break;
        case KEY_SELECT:
        	if (focus >= 1 && focus <= wifi_count) {
        	    lv_textarea_set_text(ssid_input, wifi_networks[focus - 1]);
        	    focus = wifi_count + 2;
        		StartTextInput(password_input);
        	} else if (focus == ssid_input_focus || focus == password_input_focus) {
        		StartTextInput(focus == ssid_input_focus ? ssid_input : password_input);
        	} else if (focus == connect_wifi_focus) {
        		ConnectToWifi();
        	}
            break;
        case KEY_BACK:
        	LoadScreen(&MainScreen);
            break;
    }
}

void UpdateWifiNetworksList(char* response) {
	lv_obj_clean(wifi_list);
	int count = 0;
    const char *ptr = response;
    while ((ptr = strstr(ptr, "+CWLAP:(")) != NULL && count < MAX_WIFI_NETWORKS) {
        int rssi;
        char ssid[32];
        if (sscanf(ptr, "+CWLAP:(\"%31[^\"]\")(%d)", ssid, &rssi) > 0) {
			wifi_btns[count] = lv_list_add_btn(wifi_list, NULL, ssid);
    		strcpy(wifi_networks[count], ssid);

			if (strcmp(ssid, connected_ssid) == 0) {
				lv_obj_set_style_text_color(wifi_btns[count], Green(), 0);
			}
			count++;
        }
        ptr++;
    }
    wifi_count = count;
}

void UpdateConnectedNetwork(char* response) {
	int state;
    sscanf(response, "AT+CWSTATE?\r\n+CWSTATE:%d,\"%31[^\"]\"", &state, connected_ssid);
    switch (state) {
    case 0:
    	lv_label_set_text(status_label, "NO CONNECTION");
		lv_obj_set_style_text_color(status_label, Red(), 0);
    	break;
    case 1:
    case 2:
		lv_label_set_text(status_label, "CONNECTED");
		lv_obj_set_style_text_color(status_label, Green(), 0);
    	break;
    case 3:
    	lv_label_set_text(status_label, "CONNECTING...");
		lv_obj_set_style_text_color(status_label, Yellow(), 0);
    	break;
    case 4:
    	lv_label_set_text(status_label, "DISCONNECTED");
		lv_obj_set_style_text_color(status_label, Red(), 0);
    	break;
    }
}

void ReadWifiResponse() {
	char response[1024] = {0};
	if (xQueueReceive(xQueueWifiResponse, &response, ( TickType_t ) 10)) {
		if (strstr(response, "+CWLAP:(") != NULL) {
			UpdateWifiNetworksList(response);
		} else if (strstr(response, "+CWSTATE:") != NULL) {
			UpdateConnectedNetwork(response);
		} else if (strstr(response, "AT+CWLAP\r\n\r\nOK\r\n") != NULL) {
	    	ResetWifi();
		}
	}
}

// Main process
// ------------------------------------------------------------------------------
void WiFiScreenProcess(void) {
	UpdateWifiNetworks();
	ReadWifiResponse();

	wifi_screen_k = wifi_count + 4;
	WifiScreen_UpdateSelectionHighlight();

	Key_t key = ListenToKeyboard();
	if (key.key != '\0') {
		WifiScreen_ProcessKeyboardInput(key);
	}
}

void WiFiScreenDeinit() {}
