#include <pebble.h>
  
#define NUM_MENU_SECTIONS 1
#define NUM_MENU_ITEMS 5
  
#define NUM_MENU_ICONS 2


static Window *window;
static MenuLayer *menu_layer;

// Menu items can optionally have an icon drawn with them
static GBitmap *menu_icons[NUM_MENU_ICONS];

// A callback is used to specify the amount of sections of menu items
// With this, you can dynamically add and remove sections
static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_MENU_SECTIONS;
}

// Each section has a number of items;  we use a callback to specify this
// You can also dynamically add and remove items using this
static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return NUM_MENU_ITEMS;
}

// A callback is used to specify the height of the section header
static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  // This is a define provided in pebble.h that you may use for the default height
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

// Here we draw what each header is
static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  // Draw title text in the section header
  menu_cell_basic_header_draw(ctx, cell_layer, "Prehled produktu");
}

// This is the menu item draw callback where you specify what each item should look like
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  // Determine which section we're going to draw in
  switch (cell_index->section) {
    case 0:
      // Use the row to specify which item we'll draw
      switch (cell_index->row) {
        case 0:
          // Ikony: menu_icons[current_icon]
          menu_cell_basic_draw(ctx, cell_layer, "Osobni ucet CS", "Zustatek: 32 123,- Kc", menu_icons[0]);
          break;

        case 1:
          menu_cell_basic_draw(ctx, cell_layer, "Hypotecni uver", "Zustatek: XXXXX,- Kc", menu_icons[1]);
          break;

        case 2:
          menu_cell_basic_draw(ctx, cell_layer, "Kreditni karta", "Zustatek: XXXXXX,- Kc", menu_icons[1]);
          break;

        case 3:
          menu_cell_basic_draw(ctx, cell_layer, "Cizomenovy ucet", "Zustatek: XXXXXX,- Kc", menu_icons[0]);
          break;

        case 4:
          menu_cell_basic_draw(ctx, cell_layer, "Sporici ucet", "Zustatek: XXXXXX,- Kc", menu_icons[0]);
          break;

      }
      break;
  }
}

// Here we capture when a user selects a menu item
void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Click detected");
  APP_LOG(APP_LOG_LEVEL_DEBUG, "  section: %u", cell_index->section);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "  item: %u", cell_index->row);

  // After changing the icon, mark the layer to have it updated
  layer_mark_dirty(menu_layer_get_layer(menu_layer));
}



// This initializes the menu upon window load
void window_load(Window *window) {
  // Here we load the bitmap assets
  // resource_init_current_app must be called before all asset loading
  int num_menu_icons = 0;
  menu_icons[num_menu_icons++] = gbitmap_create_with_resource(RESOURCE_ID_ICON_ACCOUNT);
  menu_icons[num_menu_icons++] = gbitmap_create_with_resource(RESOURCE_ID_ICON_LOAN);

  
  // Now we prepare to initialize the menu layer
  // We need the bounds to specify the menu layer's viewport size
  // In this case, it'll be the same as the window's
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // Create the menu layer
  menu_layer = menu_layer_create(bounds);

  // Set all the callbacks for the menu layer
  menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
  });
  
  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(menu_layer, window);

  // Add it to the window for display
  layer_add_child(window_layer, menu_layer_get_layer(menu_layer));  
  
}

void window_unload(Window *window) {
  // Destroy the menu layer
  menu_layer_destroy(menu_layer);
  
  // Cleanup the menu icons
  for (int i = 0; i < NUM_MENU_ICONS; i++) {
    gbitmap_destroy(menu_icons[i]);
  }

  // And cleanup the background
  // gbitmap_destroy(menu_background);
}


int main(void) {
  window = window_create();

  // Setup the window handlers
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  window_stack_push(window, true /* Animated */);

  app_event_loop();

  window_destroy(window);
}
