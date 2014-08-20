#include <pebble.h>

static Window* window;
static TextLayer* platform_layer;
static TextLayer* delay_layer;
static BitmapLayer* icon_layer;
static GBitmap* icon_bitmap = NULL;

static AppSync sync;
static uint8_t sync_buffer[640];

static uint32_t ICONS[] = {
	RESOURCE_ID_IMAGE_BUS,
	RESOURCE_ID_IMAGE_CABLE,
	RESOURCE_ID_IMAGE_SHIP,
	RESOURCE_ID_IMAGE_TRAIN,
	RESOURCE_ID_IMAGE_TRAM
};


static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
}

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
    switch (key) {
    	case 0x0:
			if (new_tuple->value->uint8 < 5) { // why i'm getting 103 here I have no idea
				icon_bitmap = gbitmap_create_with_resource(ICONS[new_tuple->value->uint8]);
				bitmap_layer_set_bitmap(icon_layer, icon_bitmap);	
			}
    		break;
    	case 0x1:
    		text_layer_set_text(platform_layer, new_tuple->value->cstring);
    		break;
    	case 0x2:
    		text_layer_set_text(delay_layer, new_tuple->value->cstring);
    		break;
    }
}

static void window_load(Window *window) {
    Layer* window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);
    
    icon_layer = bitmap_layer_create(GRect(bounds.size.w / 2 - 40, 10, 80, 80));
	layer_add_child(window_layer, bitmap_layer_get_layer(icon_layer));
    
    platform_layer = text_layer_create(GRect(0, 95, bounds.size.w, 50));
	text_layer_set_text_color(platform_layer, GColorBlack);
	text_layer_set_background_color(platform_layer, GColorClear);
	text_layer_set_font(platform_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text_alignment(platform_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(platform_layer));

    delay_layer = text_layer_create(GRect(0, 115, bounds.size.w, 50));
	text_layer_set_text_color(delay_layer, GColorBlack);
	text_layer_set_background_color(delay_layer, GColorClear);
	text_layer_set_font(delay_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text_alignment(delay_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(delay_layer));
}

static void window_unload(Window *window) {
	text_layer_destroy(platform_layer);
	text_layer_destroy(delay_layer);
	if (icon_bitmap) {
		gbitmap_destroy(icon_bitmap);
	}
	bitmap_layer_destroy(icon_layer);
}

void showConnectionDetail(MenuIndex* index) {
	char buf[10];
	char buf2[10];
	snprintf(buf, sizeof(buf), "gd;%i", index->row);
	Tuplet value = TupletCString(0x0, strcpy(buf2, buf));
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    if (iter == NULL) {
        return;
    }
    dict_write_tuplet(iter, &value);
    dict_write_end(iter);
    app_message_outbox_send();
    
    
    
    window = window_create();
    window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
    window_stack_push(window, true);
    
    
    Tuplet initial_values[] = {
        TupletInteger(0x0, (uint8_t) 999),
      	TupletCString(0x1, ""),
      	TupletCString(0x2, ""),
    };
    app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values, ARRAY_LENGTH(initial_values),
                  sync_tuple_changed_callback, sync_error_callback, NULL);
}