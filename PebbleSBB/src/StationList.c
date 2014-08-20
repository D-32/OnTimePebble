#include <pebble.h>

static Window* window;
static MenuLayer* menu_layer;

static bool enableButton;
static AppSync sync;
static uint8_t sync_buffer[640];

static uint8_t count;
static char rows[8][100];

void showConnections(MenuIndex* index);

static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
}

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
    if (key == 0) {
    	count = new_tuple->value->uint8;
    } else {
		const char* tmp = new_tuple->value->cstring;
		if (strlen(tmp) > 1) {
			enableButton = true;
			strcpy(rows[key - 1], tmp); // this is hex, but as were only using 8 it's okay	
		}
    }
	menu_layer_reload_data(menu_layer);
}

void stations_menu_select_click(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
	if (enableButton) {
		showConnections(cell_index);	
	}
}

void stations_menu_draw_row(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context) { 
	graphics_context_set_text_color(ctx, GColorBlack);
	graphics_draw_text(ctx, rows[cell_index->row], fonts_get_system_font(FONT_KEY_GOTHIC_14), GRect(0,2,layer_get_frame(cell_layer).size.w,layer_get_frame(cell_layer).size.h), GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
}

int16_t stations_menu_get_cell_height(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
	return 24;
}

uint16_t stations_menu_get_num_rows_in_section(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
	return count;
}

MenuLayerCallbacks cbacks;
static void window_load(Window *window) {
    Layer* window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    menu_layer = menu_layer_create(GRect(0,0,bounds.size.w,bounds.size.h));
	menu_layer_set_click_config_onto_window(menu_layer, window);
	cbacks.get_num_rows = &stations_menu_get_num_rows_in_section;
	cbacks.get_cell_height = &stations_menu_get_cell_height; 
	cbacks.select_click = &stations_menu_select_click; 
	cbacks.draw_row = &stations_menu_draw_row;
	menu_layer_set_callbacks(menu_layer, NULL, cbacks);
	layer_add_child(window_get_root_layer(window), menu_layer_get_layer(menu_layer));
    
    Tuplet initial_values[] = {
        TupletInteger(0x0, (uint8_t) 0),
      	TupletCString(0x1, ""),
      	TupletCString(0x2, ""),
      	TupletCString(0x3, ""),
      	TupletCString(0x4, ""),
      	TupletCString(0x5, ""),
      	TupletCString(0x6, ""),
      	TupletCString(0x7, ""),
      	TupletCString(0x8, ""),
    };
    app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values, ARRAY_LENGTH(initial_values),
                  sync_tuple_changed_callback, sync_error_callback, NULL);
}

static void window_unload(Window *window) {
    app_sync_deinit(&sync);
    menu_layer_destroy(menu_layer);
}

static void init(void) {
    window = window_create();
    window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
    window_stack_push(window, true);
}

static void deinit(void) {
    window_destroy(window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
