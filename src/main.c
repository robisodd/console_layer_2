#include <pebble.h>
#include "main.h"
#include "console.h"
//#pragma GCC diagnostic ignored "-Wsign-compare"
//#pragma GCC diagnostic ignored "-Wswitch"`
// Console Layer positioning
#define BOTTOM_CONSOLE_HEIGHT 34
#define CONSOLE_LAYER_SEPARATION 4

static Window *main_window;
static Layer *top_console_layer;
static Layer *bottom_console_layer;
static GBitmap *smile;
static bool emulator;
static GRect outer_rect;


static void error_msg(char *msg) {
  printf("Displaying Error: %s", msg);
  console_layer_write_text_styled(top_console_layer, "Dictation Error", GColorBlack, GColorRed, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), GTextAlignmentCenter, true, true);
  console_layer_writeln_text(bottom_console_layer, msg);
}


//#pragma GCC diagnostic ignored "-Wswitch"
static char* watch_type() {
  switch(watch_info_get_model()) {
    case WATCH_INFO_MODEL_UNKNOWN:           return "Emulator";
    case WATCH_INFO_MODEL_PEBBLE_ORIGINAL:   return "Original Pebble";
    case WATCH_INFO_MODEL_PEBBLE_STEEL:      return "Pebble Steel";
    case WATCH_INFO_MODEL_PEBBLE_TIME:       return "Pebble Time";
    case WATCH_INFO_MODEL_PEBBLE_TIME_STEEL: return "Pebble Time Steel";
    case WATCH_INFO_MODEL_PEBBLE_TIME_ROUND_14:
    case WATCH_INFO_MODEL_PEBBLE_TIME_ROUND_20:
    return "Pebble Time Round";
    default: return "Unknown Pebble";
  }
}


// ------------------------------------------------------------------------ //
//  Dictation API Functions
// ------------------------------------------------------------------------ //
static DictationSession *dictation_session = NULL;
static char dictation_text[512];
static const char* DictationSessionStatusError[] = {
  "Transcription successful, with a valid result.",
  "User rejected transcription and exited UI.",
  "User exited UI after transcription error.",
  "Too many errors occurred during transcription and the UI exited.",
  "No speech was detected and UI exited.",
  "No BT or internet connection.",
  "Voice transcription disabled for this user.",
  "Voice transcription failed due to internal error.",
  "Cloud recognizer failed to transcribe speech."
};

static void dictation_session_callback(DictationSession *session, DictationSessionStatus status, char *transcription, void *context) {
  if(status == DictationSessionStatusSuccess) {
    snprintf(dictation_text, sizeof(dictation_text), "%s", transcription);
    printf("Dictation Text: %s", dictation_text);
    console_layer_writeln_text(top_console_layer, dictation_text);
    console_layer_writeln_text(bottom_console_layer, "Dictation Successful");
  } else {
    printf("Dictation Error: %s", DictationSessionStatusError[status]);
    snprintf(dictation_text, sizeof(dictation_text), "Dictation Error: %s", DictationSessionStatusError[status]);
    error_msg(dictation_text);
  }
}


static void init_dictation() {
  dictation_session = dictation_session_create(sizeof(dictation_text), dictation_session_callback, NULL);
}

static void destroy_dictation() {
  if(dictation_session)
    dictation_session_destroy(dictation_session);
}


// ------------------------------------------------------------------------ //
//  Button Functions
// ------------------------------------------------------------------------ //
static void up_click_handler(ClickRecognizerRef recognizer, void *context) { //   UP   button
  static uint8_t prevchat = 3;
  
  if(rand()%2) {
    if(prevchat!=1) {
      console_layer_write_text_styled(top_console_layer, "player 1:", GColorBlue, console_layer_get_background_color(top_console_layer), fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), GTextAlignmentLeft, true, false);
      console_layer_set_alignment(top_console_layer, GTextAlignmentRight);
      prevchat = 1;
    }
    console_layer_write_text(bottom_console_layer, "player 1 sent");
  } else {
    if(prevchat!=0) {
      console_layer_write_text_styled(top_console_layer, ":player 2", GColorBlue, console_layer_get_background_color(top_console_layer), fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), GTextAlignmentRight, true, false);
      console_layer_set_alignment(top_console_layer, GTextAlignmentLeft);
      prevchat = 0;
    }
    console_layer_write_text(bottom_console_layer, "player 2 sent");
  }
  
  switch(rand()%6) {
    case 0:
      console_layer_writeln_text(top_console_layer, "Hello there!\nHow are the kids?\nThat's good to hear.");
      console_layer_writeln_text(bottom_console_layer, "                           some messages");
    break;
    case 1:
    case 2:
      console_layer_writeln_text(top_console_layer, "This is weird.\n");
      console_layer_writeln_text(bottom_console_layer, "                           a message");
    break;
    case 3:
      console_layer_writeln_text(top_console_layer, "Hi \U0001F4A9 face.\n");
      console_layer_writeln_text(bottom_console_layer, "                           emoji message");
    break;
    case 4:
      console_layer_writeln_text(top_console_layer, "Guess What?\nThis is a really long message which won't fit on the screen since wordwrap is off.");
      console_layer_writeln_text(bottom_console_layer, "                           a long message");
    break;
    case 5:
      console_layer_writeln_image(top_console_layer, smile);
      console_layer_writeln_text(bottom_console_layer, "                           a picture");
    break;
  }
}

static void sl_click_handler(ClickRecognizerRef recognizer, void *context) { // SELECT button
  if(emulator)
    error_msg("No Emulator Microphone");
  else
    PBL_IF_MICROPHONE_ELSE(dictation_session_start(dictation_session), error_msg("No Microphone"));
}

static void dn_click_handler(ClickRecognizerRef recognizer, void *context) { //  DOWN  button pressed briefly
  if(layer_get_hidden(bottom_console_layer)) {
    layer_set_hidden(bottom_console_layer, false);
    console_layer_writeln_text(bottom_console_layer, "Log Visible");
    layer_set_frame(top_console_layer, GRect(outer_rect.origin.x, outer_rect.origin.y, outer_rect.size.w, outer_rect.size.h - BOTTOM_CONSOLE_HEIGHT - CONSOLE_LAYER_SEPARATION));
  } else {
    layer_set_hidden(bottom_console_layer, true);
    console_layer_writeln_text(bottom_console_layer, "Hiding log layer");
    layer_set_frame(top_console_layer, outer_rect);
  }
}


static void dn_long_click_handler(ClickRecognizerRef recognizer, void *context) { //  DOWN  button held for 500ms
  console_layer_clear(top_console_layer);
  console_layer_writeln_text(bottom_console_layer, "Chat Window Cleared.");
}


static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, sl_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, dn_click_handler);
  window_long_click_subscribe(BUTTON_ID_DOWN, 0, dn_long_click_handler, NULL);
}


// ------------------------------------------------------------------------ //
//  Main Functions
// ------------------------------------------------------------------------ //
static void main_window_load(Window *window) {
  window_set_click_config_provider(window, click_config_provider);
  
  // Create Console Layers
  Layer *root_layer = window_get_root_layer(window);
  
  outer_rect = grect_inset(layer_get_frame(window_get_root_layer(window)), GEdgeInsets(PBL_IF_ROUND_ELSE(26, 10)));
  
  top_console_layer = console_layer_create(GRect(outer_rect.origin.x, outer_rect.origin.y, outer_rect.size.w, outer_rect.size.h - BOTTOM_CONSOLE_HEIGHT - CONSOLE_LAYER_SEPARATION));
  bottom_console_layer = console_layer_create(GRect(outer_rect.origin.x, outer_rect.origin.y + outer_rect.size.h - BOTTOM_CONSOLE_HEIGHT, outer_rect.size.w, BOTTOM_CONSOLE_HEIGHT));
  
  layer_add_child(root_layer, top_console_layer);
  layer_add_child(root_layer, bottom_console_layer);

  // Configure Console Layers
  console_layer_set_layer_background_color(top_console_layer, GColorWhite);  // default is clear background
  console_layer_set_layer_style(bottom_console_layer, GColorWhite, GColorBlack, fonts_get_system_font(FONT_KEY_GOTHIC_09), GTextAlignmentLeft, true, true);

  console_layer_set_header_enabled(top_console_layer, true);
  console_layer_set_border_enabled(top_console_layer, true);
  console_layer_set_header_background_color(top_console_layer, PBL_IF_COLOR_ELSE(GColorVividCerulean, GColorBlack));
  console_layer_set_header_text_color(top_console_layer, PBL_IF_COLOR_ELSE(GColorBlack, GColorWhite));
  console_layer_set_header_text(top_console_layer, "Chat");
  
  
  // Add some text to Console Layers
  console_layer_write_text_styled(top_console_layer, "Welcome to\nConsole Chat", GColorInherit, GColorInherit, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), GTextAlignmentCenter, true, true);
  console_layer_writeln_text(bottom_console_layer, "Program Started.");
  
  // Detect and log watch type
  console_layer_write_text(bottom_console_layer, "Detected:");
  console_layer_write_text_styled(bottom_console_layer, watch_type(), GColorYellow, GColorInherit, GFontInherit, GTextAlignmentRight, WordWrapInherit, true);
}


static void main_window_unload(Window *window) {
  layer_destroy(top_console_layer);
  layer_destroy(bottom_console_layer);
}


static void init() {
  // Create main Window
  main_window = window_create();
  window_set_window_handlers(main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_set_background_color(main_window, GColorDarkGray);
  window_stack_push(main_window, true);
  
  // Create a new dictation session
  init_dictation();

  // Load an image
  smile = gbitmap_create_with_resource(RESOURCE_ID_SMILE);

  emulator = watch_info_get_model()==WATCH_INFO_MODEL_UNKNOWN;
  if(emulator) {
    light_enable(true);  // Good colors on emulator
    LOG("Emulator Detected: Turning Backlight On");
  }
}


static void deinit() {
  destroy_dictation();
  window_destroy(main_window);  // Destroy main Window
}


int main(void) {
  init();
  app_event_loop();
  deinit();
}


// #include <pebble.h>
// #include "console.h"

// int main(void) {
//   Layer *console_layer;
//   Window *main_window = window_create();
//   window_stack_push(main_window, true);
//   layer_add_child(window_get_root_layer(main_window), console_layer = console_layer_create(layer_get_frame(window_get_root_layer(main_window))));
//   console_layer_set_background_color(console_layer, console_layer_get_background_color(console_layer));
//   console_layer_set_text_color(console_layer,console_layer_get_text_color(console_layer));
//   console_layer_set_alignment(console_layer, console_layer_get_alignment(console_layer));
//   console_layer_set_word_wrap(console_layer, console_layer_get_word_wrap(console_layer));
//   console_layer_set_font(console_layer, console_layer_get_font(console_layer));
//   console_layer_clear(console_layer);
//   console_layer_write_text(console_layer, "Hello");
//   app_event_loop();
// }