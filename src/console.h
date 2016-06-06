#pragma once
#include <pebble.h>
#define CONSOLE_LAYER_BUFFER_SIZE 500      // # of Bytes to preallocate to hold the text of each console layer

#define WordWrapFalse   false
#define WordWrapTrue    true
#define WordWrapInherit 2

enum {GTextAlignmentInherit = 3};
#define GColorInherit ((GColor8){.argb=GColorClearARGB8})
#define GFontInherit NULL

//Maybe inject an image?
void   console_layer_write_image          (Layer *console_layer, GBitmap *image);

//void log_buffer(Layer *console_layer);

// ------------------------------------------------------------------------------------------------------------ //
// Create and Destroy Layer
// ------------------------------------------------------------------------------------------------------------ //
Layer* console_layer_create(GRect frame);
#define console_layer_destroy(console_layer) layer_destroy(console_layer);
#define console_layer_safe_destroy(console_layer) if (console_layer) { layer_destroy(console_layer); console_layer = NULL; }

// ------------------------------------------------------------------------------------------------------------ //
// Gets
// ------------------------------------------------------------------------------------------------------------ //
int            console_layer_get_border_thickness             (Layer *console_layer);
bool           console_layer_get_border_enabled               (Layer *console_layer);
GColor         console_layer_get_border_color                 (Layer *console_layer);

GColor         console_layer_get_header_background_color      (Layer *console_layer);
GTextAlignment console_layer_get_header_text_alignment        (Layer *console_layer);
GColor         console_layer_get_header_text_color            (Layer *console_layer);
bool           console_layer_get_header_enabled               (Layer *console_layer);
GFont          console_layer_get_header_font                  (Layer *console_layer);
char*          console_layer_get_header_text                  (Layer *console_layer);

bool           console_layer_get_dirty_automatically          (Layer *console_layer);
GColor         console_layer_get_background_color             (Layer *console_layer);

GColor         console_layer_get_default_text_background_color(Layer *console_layer);
GTextAlignment console_layer_get_default_text_alignment       (Layer *console_layer);
GColor         console_layer_get_default_text_color           (Layer *console_layer);
bool           console_layer_get_default_word_wrap            (Layer *console_layer);
GFont          console_layer_get_default_font                 (Layer *console_layer);

GColor         console_layer_get_text_background_color        (Layer *console_layer);
GTextAlignment console_layer_get_text_alignment               (Layer *console_layer);
GColor         console_layer_get_text_color                   (Layer *console_layer);
int            console_layer_get_word_wrap                    (Layer *console_layer);
GFont          console_layer_get_font                         (Layer *console_layer);


// ------------------------------------------------------------------------------------------------------------ //
// Sets
// ------------------------------------------------------------------------------------------------------------ //
void console_layer_set_border_thickness             (Layer *console_layer, int    border_thickness);
void console_layer_set_border_enabled               (Layer *console_layer, bool   border_enabled);
void console_layer_set_border_color                 (Layer *console_layer, GColor border_color);

void console_layer_set_header_background_color      (Layer *console_layer, GColor         header_background_color);
void console_layer_set_header_text_alignment        (Layer *console_layer, GTextAlignment header_text_alignment);
void console_layer_set_header_text_color            (Layer *console_layer, GColor         header_text_color);
void console_layer_set_header_enabled               (Layer *console_layer, bool           header_enabled);
void console_layer_set_header_font                  (Layer *console_layer, GFont          header_font);
void console_layer_set_header_text                  (Layer *console_layer, char          *header_text);

void console_layer_set_dirty_automatically          (Layer *console_layer, bool           dirty_layer_after_writing);
void console_layer_set_background_color             (Layer *console_layer, GColor         background_color);

void console_layer_set_default_text_background_color(Layer *console_layer, GColor         default_text_background_color);
void console_layer_set_default_text_alignment       (Layer *console_layer, GTextAlignment default_text_alignment);
void console_layer_set_default_text_color           (Layer *console_layer, GColor         default_text_color);
void console_layer_set_default_word_wrap            (Layer *console_layer, bool           default_word_wrap);
void console_layer_set_default_font                 (Layer *console_layer, GFont          default_font);

void console_layer_set_text_background_color        (Layer *console_layer, GColor         text_background_color);
void console_layer_set_text_alignment               (Layer *console_layer, GTextAlignment text_alignment);
void console_layer_set_text_color                   (Layer *console_layer, GColor         text_color);
void console_layer_set_word_wrap                    (Layer *console_layer, int            word_wrap);
void console_layer_set_font                         (Layer *console_layer, GFont          font);

// ------------------------------------------------------------------------------------------------------------ //
// Group Sets
// ------------------------------------------------------------------------------------------------------------ //
void console_layer_set_text_colors(Layer *console_layer,
                                   GColor text_color,
                                   GColor text_background_color);

void console_layer_set_defaults(Layer *console_layer,
                                GFont font,
                                GColor default_text_color,
                                GColor default_text_background_color,
                                GColor layer_background_color,
                                GTextAlignment text_alignment,
                                bool word_wrap,
                                bool dirty_layer_after_writing);

void console_layer_set_border(Layer *console_layer,
                              bool border_enabled,
                              GColor border_color,
                              int border_thickness);

void console_layer_set_header(Layer *console_layer,
                              bool header_enabled,
                              GColor header_text_color,
                              GColor header_background_color,
                              GFont header_font,
                              GTextAlignment header_text_alignment);
// ------------------------------------------------------------------------------------------------------------ //
// Write Text
// ------------------------------------------------------------------------------------------------------------ //
void console_layer_write_text_attributes(Layer *console_layer,
                                         char *text,
                                         GFont font,
                                         GColor text_color,
                                         GColor text_background_color,
                                         GTextAlignment text_alignment,
                                         int word_wrap,
                                         bool advance,
                                         bool dirty_layer_after_writing);

void console_layer_write_text(Layer *console_layer, char *text);
void console_layer_writeln_text(Layer *console_layer, char *text);
void console_layer_clear(Layer *console_layer);
// ------------------------------------------------------------------------------------------------------------ //