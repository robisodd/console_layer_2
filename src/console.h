#pragma once
#include <pebble.h>

#define DirtyAfterWritingFalse   false
#define DirtyAfterWritingTrue    true

#define WordWrapFalse   false
#define WordWrapTrue    true
#define WordWrapInherit 2

enum {GTextAlignmentInherit = 3};
#define GColorInherit ((GColor8){.argb=GColorClearARGB8})
#define GFontInherit NULL

// ------------------------------------------------------------------------------------------------------------ //
// Create and Destroy Layer
// ------------------------------------------------------------------------------------------------------------ //
Layer* console_layer_create_with_buffer_size(GRect frame, int buffer_size);
Layer* console_layer_create(GRect frame);      // Creates layer with 500 byte buffer

// You can just use the standard layer_destroy to destroy a console_layer
// Adding these for consistency
#define console_layer_destroy(console_layer) layer_destroy(console_layer);
#define console_layer_safe_destroy(console_layer) if (console_layer) { layer_destroy(console_layer); console_layer = NULL; }

// ------------------------------------------------------------------------------------------------------------ //
// Gets
// ------------------------------------------------------------------------------------------------------------ //
int            console_layer_get_border_thickness       (Layer *console_layer);
bool           console_layer_get_border_enabled         (Layer *console_layer);
GColor         console_layer_get_border_color           (Layer *console_layer);

GColor         console_layer_get_header_background_color(Layer *console_layer);
GTextAlignment console_layer_get_header_text_alignment  (Layer *console_layer);
GColor         console_layer_get_header_text_color      (Layer *console_layer);
bool           console_layer_get_header_enabled         (Layer *console_layer);
GFont          console_layer_get_header_font            (Layer *console_layer);
char*          console_layer_get_header_text            (Layer *console_layer);

bool           console_layer_get_dirty_automatically    (Layer *console_layer);
GColor         console_layer_get_background_color       (Layer *console_layer);

GColor         console_layer_get_layer_background_color (Layer *console_layer);
GColor         console_layer_get_layer_text_color       (Layer *console_layer);
GTextAlignment console_layer_get_layer_alignment        (Layer *console_layer);
bool           console_layer_get_layer_word_wrap        (Layer *console_layer);
GFont          console_layer_get_layer_font             (Layer *console_layer);

GColor         console_layer_get_background_color       (Layer *console_layer);
GColor         console_layer_get_text_color             (Layer *console_layer);
GTextAlignment console_layer_get_alignment              (Layer *console_layer);
int            console_layer_get_word_wrap              (Layer *console_layer);
GFont          console_layer_get_font                   (Layer *console_layer);


// ------------------------------------------------------------------------------------------------------------ //
// Sets
// ------------------------------------------------------------------------------------------------------------ //
void console_layer_set_header_background_color(Layer *console_layer, GColor         header_background_color);
void console_layer_set_header_text_alignment  (Layer *console_layer, GTextAlignment header_text_alignment);
void console_layer_set_header_text_color      (Layer *console_layer, GColor         header_text_color);
void console_layer_set_header_enabled         (Layer *console_layer, bool           header_enabled);
void console_layer_set_header_font            (Layer *console_layer, GFont          header_font);
void console_layer_set_header_text            (Layer *console_layer, char          *header_text);

void console_layer_set_border_thickness       (Layer *console_layer, int            border_thickness);
void console_layer_set_border_enabled         (Layer *console_layer, bool           border_enabled);
void console_layer_set_border_color           (Layer *console_layer, GColor         border_color);

void console_layer_set_layer_background_color (Layer *console_layer, GColor         layer_background_color);
void console_layer_set_layer_text_color       (Layer *console_layer, GColor         layer_text_color);
void console_layer_set_layer_alignment        (Layer *console_layer, GTextAlignment layer_alignment);
void console_layer_set_layer_word_wrap        (Layer *console_layer, bool           layer_word_wrap);
void console_layer_set_layer_font             (Layer *console_layer, GFont          layer_font);

void console_layer_set_background_color       (Layer *console_layer, GColor         background_color);
void console_layer_set_text_color             (Layer *console_layer, GColor         text_color);
void console_layer_set_alignment              (Layer *console_layer, GTextAlignment alignment);
void console_layer_set_word_wrap              (Layer *console_layer, int            word_wrap);
void console_layer_set_font                   (Layer *console_layer, GFont          font);

void console_layer_set_dirty_automatically    (Layer *console_layer, bool           dirty_layer_after_writing);

// ------------------------------------------------------------------------------------------------------------ //
// Group Sets
// ------------------------------------------------------------------------------------------------------------ //
void console_layer_set_colors      (Layer         *console_layer,
                                    GColor         text_color,
                                    GColor         background_color);

void console_layer_set_layer_style (Layer         *console_layer,
                                    GColor         layer_text_color,
                                    GColor         layer_background_color,
                                    GFont          layer_font,
                                    GTextAlignment layer_alignment,
                                    bool           layer_word_wrap,
                                    bool           dirty_layer_after_writing);

void console_layer_set_border_style(Layer         *console_layer,
                                    bool           border_enabled,
                                    GColor         border_color,
                                    int            border_thickness);

void console_layer_set_header_style(Layer         *console_layer,
                                    bool           header_enabled,
                                    GColor         header_text_color,
                                    GColor         header_background_color,
                                    GFont          header_font,
                                    GTextAlignment header_text_alignment);

// ------------------------------------------------------------------------------------------------------------ //
// Write Text
// ------------------------------------------------------------------------------------------------------------ //
// Note: The source text is copied to the console_layer's buffer inside the function so it can be from a temporary source
// I have not tested emoji yet, but it *should* work
//
// Images are NOT copied to the buffer (only a pointer) so you gotta keep the image in memory if it's still displayed on screen
// Also, btw, header text isn't stored in the layer either, just a pointer.
// ------------------------------------------------------------------------------------------------------------ //
void console_layer_write_text_and_image_styled  (Layer *console_layer, GBitmap *image, char *text, GColor text_color, GColor background_color, GFont font, GTextAlignment alignment, bool advance);
void console_layer_write_text_and_image         (Layer *console_layer, GBitmap *image, char *text);
void console_layer_writeln_text_and_image       (Layer *console_layer, GBitmap *image, char *text);

void console_layer_write_image_styled           (Layer *console_layer, GBitmap *image, GColor background_color, GTextAlignment alignment, bool advance);
void console_layer_write_image                  (Layer *console_layer, GBitmap *image);
void console_layer_writeln_image                (Layer *console_layer, GBitmap *image);

void console_layer_write_text_styled            (Layer *console_layer, char *text, GColor text_color, GColor background_color, GFont font, GTextAlignment alignment, int word_wrap, bool advance);
void console_layer_write_text                   (Layer *console_layer, char *text);
void console_layer_writeln_text                 (Layer *console_layer, char *text);

void console_layer_clear        (Layer *console_layer);


// ------------------------------------------------------------------------------------------------------------ //

// Internal use only:
void log_buffer(Layer *console_layer);
