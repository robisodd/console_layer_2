#include "console.h"
// ------------------------------------------------------------------------------------------------------------ //
//  Data Structure
// ------------------------------------------------------------------------------------------------------------ //
// APLITE APP MEMORY USAGE: Total footprint in RAM:         3353 bytes / 24KB
// BASALT APP MEMORY USAGE: Total footprint in RAM:         3353 bytes / 64KB
// CHALK  APP MEMORY USAGE: Total footprint in RAM:         3353 bytes / 64KB
// ------------------------------------------------------- 
/*
------------------------------------------------------------------------------------------------------------------------------------------------------
Buffer holds:
       0 = Head/Tail EOF terminator
       0 = End of string
       Text is written from last char to first char forwards in memory.
       This is instead of it being written (from last char to first char) forwards causing it to be stored and read backwards
       It writes last char to first char in case the string written is larger than the buffer.  So the head eats the tail.
       
------------------------------------------------------------------------------------------------------------------------------------------------------
 Buffer Description
--------------------------------------
              v=pos points to EOF            |       Second Chunk        |        Third Chunk          |
 Data Layout: 0|SBCFONTIMAGstring...string/n0|SBCFONTIMAGstring...string0|SBCFONTIMAGstring...string/n0|0000000---til end of buffer
           EOF^ ^                         ^ ^0 terminated string
        Settings|                         | optional newline (10) at end of string if writeln
       0 = 1 byte:  Circular Buffer Begin/End "EOF" split point (must = 0)
    IMAG = 4 bytes: Image Pointer (optional, if settings)
    FONT = 4 bytes: Font Pointer (optional, if settings bit b=1)
       C = 1 byte:  Text Color (optional, if settings bit c=1)
       B = 1 byte:  Text Background Color (optional, if settings bit d=1)
       S = 1 byte:  Settings Byte
       0babcdefgh = Settings Byte
         a        1 bit:  Image Included?             [1 = yes (text too), 0 = no (just text)]
          b       1 bit:  Background Color Specified? [0 = no (inherit from console_layer), 1 = yes]
           c      1 bit:  Text Color Specified?       [0 = no (inherit from console_layer), 1 = yes]
            d     1 bit:  Font Specified?             [0 = no (inherit from console_layer), 1 = yes]
             ef   2 bits: Alignment                   [00=left, 01=center, 10=right,   11=inherit]
               gh 2 bits: Word Wrap                   [00=no,   01=yes,    10=inherit, 11=inherit]
               g  1 bit:  Inherit Word Wrap?          [0 = no (change), 1 = yes (inherit)]
                h 1 bit:  bit g = 1: Unused. bit g = 0: Word Wrap? (0 = no, 1 = yes)
                          "Word Wrap no" means one line of text displayed only (ends in "..." if too long)
                          "Word Wrap yes" means wrap long (and \n inside string) text to multiple lines

------------------------------------------------------------------------------------------------------------------------------------------------------
 Note that the Buffer can wrap around
--------------------------------------

         Third |         Fourth Chunk        | |        First Chunk          |       Second Chunk        | Third Chunk (wraps around end of buffer)
Buffer: "ing\n0|SBCFONTIMAGstring...strSBCFON|0|SBCFONTIMAGstring...string/n0|SBCFONTIMAGstring...string0|SBCFONTIMAGstring...str"
                                              ^ = EOF: This 0 is where pos points, signifing the beginning/end "EOF" point
                                              
Notice that in this example that the fourth chunk is invalid since it's been partially overwritten
  
------------------------------------------------------------------------------------------------------------------------------------------------------  
*/

typedef struct console_data_struct {
  bool               dirty_layer_automatically;
  bool               border_enabled;
  bool               header_enabled;
  bool               layer_word_wrap;
  
  GColor             border_color;
  uint8_t            border_thickness;
  
  GColor             header_background_color;
  GColor             header_text_color;
  GFont              header_font;
  GTextAlignment     header_text_alignment;
  char              *header_text;
  
  GColor             layer_background_color;
  GColor             layer_text_color;
  GFont              layer_font;
  GTextAlignment     layer_alignment;

  uint8_t            word_wrap;
  GColor             background_color;
  GColor             text_color;
  GFont              font;
  GTextAlignment     alignment;

  size_t             buffer_size;
  uintptr_t          pos;
  char              *buffer;
} console_data_struct;

#define DEFAULT_BUFFER_SIZE 500  // Size (in bytes) of text buffer per layer

                                          // 0bABCDEFGH = Settings Byte
#define             IMAGE_BIT  0b10000000 //   A        1 bit:  Image Included? (1=yes, 0=no)
#define  BACKGROUND_COLOR_BIT  0b01000000 //    B       1 bit:  Background Color Specified? (0 = no (inherit from console_layer), 1 = yes)
#define        TEXT_COLOR_BIT  0b00100000 //     C      1 bit:  Text Color Specified?       (0 = no (inherit from console_layer), 1 = yes)
#define              FONT_BIT  0b00010000 //      D     1 bit:  Font Specified?             (0 = no (inherit from console_layer), 1 = yes)
#define         ALIGNMENT_BITS 0b00001100 //       EF   2 bits: Alignment                   [00=left, 01=center, 10=right,   11=inherit]
#define         WORD_WRAP_BITS 0b00000011 //         GH 2 bits: Word Wrap                   [00=no,   01=yes,    10=inherit, 11=inherit]
#define WORD_WRAP_INHERIT_BIT  0b00000010 //         G  1 bit:  Inherit Word Wrap?          (0 = no:change, 1 = yes:inherit)
#define         WORD_WRAP_BIT  0b00000001 //          H 1 bit:  WORD_WRAP_INHERIT_BIT = 1: Unused. WORD_WRAP_INHERIT_BIT = 0: Word Wrap
// Word Wrap: 0 = One line of text displayed only (ends in "..." if too long), 1 = Wrap Long (and \n) Text to multiple lines

#define NULL_IMAGE NULL

// ------------------------------------------------------------------------------------------------------------ //
// Gets
// ------------------------------------------------------------------------------------------------------------ //
int            console_layer_get_border_thickness       (Layer *console_layer) {return ((console_data_struct*)layer_get_data(console_layer))->border_thickness;}
bool           console_layer_get_border_enabled         (Layer *console_layer) {return ((console_data_struct*)layer_get_data(console_layer))->border_enabled;}
GColor         console_layer_get_border_color           (Layer *console_layer) {return ((console_data_struct*)layer_get_data(console_layer))->border_color;}

GColor         console_layer_get_header_background_color(Layer *console_layer) {return ((console_data_struct*)layer_get_data(console_layer))->header_background_color;}
GTextAlignment console_layer_get_header_text_alignment  (Layer *console_layer) {return ((console_data_struct*)layer_get_data(console_layer))->header_text_alignment;}
GColor         console_layer_get_header_text_color      (Layer *console_layer) {return ((console_data_struct*)layer_get_data(console_layer))->header_text_color;}
bool           console_layer_get_header_enabled         (Layer *console_layer) {return ((console_data_struct*)layer_get_data(console_layer))->header_enabled;}
GFont          console_layer_get_header_font            (Layer *console_layer) {return ((console_data_struct*)layer_get_data(console_layer))->header_font;}
char*          console_layer_get_header_text            (Layer *console_layer) {return ((console_data_struct*)layer_get_data(console_layer))->header_text;}

GColor         console_layer_get_layer_background_color (Layer *console_layer) {return ((console_data_struct*)layer_get_data(console_layer))->layer_background_color;}
GColor         console_layer_get_layer_text_color       (Layer *console_layer) {return ((console_data_struct*)layer_get_data(console_layer))->layer_text_color;}
GTextAlignment console_layer_get_layer_alignment        (Layer *console_layer) {return ((console_data_struct*)layer_get_data(console_layer))->layer_alignment;}
bool           console_layer_get_layer_word_wrap        (Layer *console_layer) {return ((console_data_struct*)layer_get_data(console_layer))->layer_word_wrap;}
GFont          console_layer_get_layer_font             (Layer *console_layer) {return ((console_data_struct*)layer_get_data(console_layer))->layer_font;}

GColor         console_layer_get_background_color       (Layer *console_layer) {return ((console_data_struct*)layer_get_data(console_layer))->background_color;}
GColor         console_layer_get_text_color             (Layer *console_layer) {return ((console_data_struct*)layer_get_data(console_layer))->text_color;}
GTextAlignment console_layer_get_alignment              (Layer *console_layer) {return ((console_data_struct*)layer_get_data(console_layer))->alignment;}
int            console_layer_get_word_wrap              (Layer *console_layer) {return ((console_data_struct*)layer_get_data(console_layer))->word_wrap;}
GFont          console_layer_get_font                   (Layer *console_layer) {return ((console_data_struct*)layer_get_data(console_layer))->font;}

bool           console_layer_get_dirty_automatically    (Layer *console_layer) {return ((console_data_struct*)layer_get_data(console_layer))->dirty_layer_automatically;}


// ------------------------------------------------------------------------------------------------------------ //
// Sets
// ------------------------------------------------------------------------------------------------------------ //
void console_layer_set_border_thickness       (Layer *console_layer, int    border_thickness)                 {((console_data_struct*)layer_get_data(console_layer))->border_thickness = border_thickness;}
void console_layer_set_border_enabled         (Layer *console_layer, bool   border_enabled)                   {((console_data_struct*)layer_get_data(console_layer))->border_enabled   = border_enabled;}
void console_layer_set_border_color           (Layer *console_layer, GColor border_color)                     {((console_data_struct*)layer_get_data(console_layer))->border_color     = border_color;}

void console_layer_set_header_background_color(Layer *console_layer, GColor         header_background_color)  {((console_data_struct*)layer_get_data(console_layer))->header_background_color = header_background_color;}
void console_layer_set_header_text_alignment  (Layer *console_layer, GTextAlignment header_text_alignment)    {((console_data_struct*)layer_get_data(console_layer))->header_text_alignment   = header_text_alignment;}
void console_layer_set_header_text_color      (Layer *console_layer, GColor         header_text_color)        {((console_data_struct*)layer_get_data(console_layer))->header_text_color       = header_text_color;}
void console_layer_set_header_enabled         (Layer *console_layer, bool           header_enabled)           {((console_data_struct*)layer_get_data(console_layer))->header_enabled          = header_enabled;}
void console_layer_set_header_font            (Layer *console_layer, GFont          header_font)              {((console_data_struct*)layer_get_data(console_layer))->header_font             = header_font;}
void console_layer_set_header_text            (Layer *console_layer, char          *header_text)              {((console_data_struct*)layer_get_data(console_layer))->header_text             = header_text;}

void console_layer_set_layer_background_color (Layer *console_layer, GColor         layer_background_color)   {((console_data_struct*)layer_get_data(console_layer))->layer_background_color = layer_background_color;}
void console_layer_set_layer_text_color       (Layer *console_layer, GColor         layer_text_color)         {((console_data_struct*)layer_get_data(console_layer))->layer_text_color       = layer_text_color;}
void console_layer_set_layer_alignment        (Layer *console_layer, GTextAlignment layer_alignment)          {((console_data_struct*)layer_get_data(console_layer))->layer_alignment        = layer_alignment;}
void console_layer_set_layer_word_wrap        (Layer *console_layer, bool           layer_word_wrap)          {((console_data_struct*)layer_get_data(console_layer))->layer_word_wrap        = layer_word_wrap;}
void console_layer_set_layer_font             (Layer *console_layer, GFont          layer_font)               {((console_data_struct*)layer_get_data(console_layer))->layer_font             = layer_font;}

void console_layer_set_background_color       (Layer *console_layer, GColor         background_color)         {((console_data_struct*)layer_get_data(console_layer))->background_color = background_color;}
void console_layer_set_alignment              (Layer *console_layer, GTextAlignment alignment)                {((console_data_struct*)layer_get_data(console_layer))->alignment             = alignment;}
void console_layer_set_text_color             (Layer *console_layer, GColor         text_color)               {((console_data_struct*)layer_get_data(console_layer))->text_color            = text_color;}
void console_layer_set_word_wrap              (Layer *console_layer, int            word_wrap)                {((console_data_struct*)layer_get_data(console_layer))->word_wrap             = word_wrap;}
void console_layer_set_font                   (Layer *console_layer, GFont          font)                     {((console_data_struct*)layer_get_data(console_layer))->font                  = font;}

void console_layer_set_dirty_automatically    (Layer *console_layer, bool           dirty_layer_automatically){((console_data_struct*)layer_get_data(console_layer))->dirty_layer_automatically = dirty_layer_automatically;}

// ------------------------------------------------------------------------------------------------------------ //

void console_layer_set_text_style (Layer         *console_layer,
                                   GColor         text_color,
                                   GColor         background_color,
                                   GFont          font,
                                   GTextAlignment alignment,
                                   int            word_wrap) {
  console_data_struct *console_data = (console_data_struct*)layer_get_data(console_layer);
  console_data->text_color       = text_color;
  console_data->background_color = background_color;
  console_data->font             = font;
  console_data->word_wrap        = word_wrap;
  console_data->alignment        = alignment;
  
  if(console_data->dirty_layer_automatically) layer_mark_dirty(console_layer);
}

// ------------------------------------------------------------------------------------------------------------ //

void console_layer_set_layer_style (Layer         *console_layer,
                                    GColor         layer_text_color,
                                    GColor         layer_background_color,
                                    GFont          layer_font,
                                    GTextAlignment layer_alignment,
                                    bool           layer_word_wrap,
                                    bool           dirty_layer_automatically) {
  console_data_struct *console_data = (console_data_struct*)layer_get_data(console_layer);
  console_data->layer_text_color          = layer_text_color;
  console_data->layer_background_color    = layer_background_color;
  console_data->layer_font                = layer_font;
  console_data->layer_word_wrap           = layer_word_wrap;
  console_data->layer_alignment           = layer_alignment;
  console_data->dirty_layer_automatically = dirty_layer_automatically;
  
  if(dirty_layer_automatically) layer_mark_dirty(console_layer);
}

// ------------------------------------------------------------------------------------------------------------ //

void console_layer_set_border_style(Layer *console_layer, bool border_enabled, GColor border_color, int border_thickness) {
  console_data_struct *console_data = (console_data_struct*)layer_get_data(console_layer);
  console_data->border_color     = border_color;
  console_data->border_thickness = border_thickness;
  console_data->border_enabled   = border_enabled;
  
  if(console_data->dirty_layer_automatically) layer_mark_dirty(console_layer);
}

// ------------------------------------------------------------------------------------------------------------ //

void console_layer_set_header_style(Layer         *console_layer,
                                    bool           header_enabled,
                                    GColor         header_text_color,
                                    GColor         header_background_color,
                                    GFont          header_font,
                                    GTextAlignment header_text_alignment) {
  console_data_struct *console_data = (console_data_struct*)layer_get_data(console_layer);
  console_data->header_background_color = header_background_color;
  console_data->header_text_color       = header_text_color;
  console_data->header_font             = header_font;
  console_data->header_text_alignment   = header_text_alignment;
  console_data->header_enabled          = header_enabled;

  if(console_data->dirty_layer_automatically) layer_mark_dirty(console_layer);
}

// ------------------------------------------------------------------------------------------------------------ //








// ------------------------------------------------------------------------------------------------------------ //
// Write Layer
// ------------------------------------------------------------------------------------------------------------ //

void console_layer_clear(Layer *console_layer) {
  console_data_struct *console_data = (console_data_struct*)layer_get_data(console_layer);
  console_data->pos = 0;
  console_data->buffer[0] = 0;
  console_data->buffer[1] = 0;
  //console_data->buffer[console_data->buffer_size - 1] = 0;

  console_data->background_color = GColorInherit;
  console_data->text_color       = GColorInherit;
  console_data->font             = GFontInherit;
  console_data->alignment        = GTextAlignmentInherit;
  console_data->word_wrap        = WordWrapInherit;

  if(console_data->dirty_layer_automatically)
    layer_mark_dirty(console_layer);
}

// ------------------------------------------------------------------------------------------------------------ //

void console_layer_write_text_and_image_styled(Layer *console_layer, GBitmap *image, char *text, GColor text_color, GColor background_color, GFont font, GTextAlignment alignment, int word_wrap, bool advance) {
  console_data_struct *console_data = (console_data_struct*)layer_get_data(console_layer);

  console_data->pos += ((UINTPTR_MAX - (UINTPTR_MAX % console_data->buffer_size)) - console_data->buffer_size);

  // Copy text (forwards in memory, but from last char to first char) to buffer
  char *begin, *end;
  while(*text || image) {
    uint8_t settings = 0;  // new settings for each row

    
    
    // Adding feature: Draw text on top of image
    begin = text;
    if(image) {
      settings = IMAGE_BIT;
      while(*text) text++;   // ends on 0
    } else {
      while((*text) && ((*text)!=10)) text++;   // ends on 0 or 10 (newline)
    }
    end = text;
    
    
    // write 0 no matter if 10 or 0
    //TODO: This part should be redone
    console_data->buffer[console_data->pos-- % console_data->buffer_size] = 0;
    if(*text==10) {
      console_data->buffer[console_data->pos-- % console_data->buffer_size] = 10;
      text++; // skip past 10
    } else if (advance) {
      console_data->buffer[console_data->pos-- % console_data->buffer_size] = 10;
    }
    
    // Copy string to buffer (forwards in memory) from end to beginning
    while(end!=begin)
      console_data->buffer[console_data->pos-- % console_data->buffer_size] = *(--end);
    
    
    
    // Copy Image Location to buffer
    if(image) {
      for (uintptr_t i=0; i<sizeof(image); i++) {
      console_data->buffer[console_data->pos-- % console_data->buffer_size] = ((uint8_t*)&image)[i];
      }
    }
    image = NULL;  // to exit the while loop above



    
    // Copy header information
    if(font) {
      for (uintptr_t i=0; i<sizeof(font); i++)
        console_data->buffer[console_data->pos-- % console_data->buffer_size] = ((uint8_t*)&font)[i];
      settings |= FONT_BIT;
    }

    if(text_color.argb) {
      console_data->buffer[console_data->pos-- % console_data->buffer_size] = text_color.argb;
      settings |= TEXT_COLOR_BIT;
    }

    if(background_color.argb) {
      console_data->buffer[console_data->pos-- % console_data->buffer_size] = background_color.argb;
      settings |= BACKGROUND_COLOR_BIT;
    }

    settings |= (word_wrap&WORD_WRAP_BITS);
    settings |= (alignment==GTextAlignmentLeft?0b0000 : alignment==GTextAlignmentCenter?0b0100 : alignment==GTextAlignmentRight?0b1000 : 0b1100);

    console_data->buffer[console_data->pos-- % console_data->buffer_size] = settings; // Save settings
    console_data->buffer[console_data->pos % console_data->buffer_size] = 0;          // EOF -- Head/Tail buffer transition point
  }

  console_data->pos %= console_data->buffer_size;

  if(console_data->dirty_layer_automatically)
    layer_mark_dirty(console_layer);
}

// ------------------------------------------------------------------------------------------------------------ //

void console_layer_write_text_and_image(Layer *console_layer, GBitmap *image, char *text) {
  console_data_struct *console_data = (console_data_struct*)layer_get_data(console_layer);
  console_layer_write_text_and_image_styled(console_layer, image, text, console_data->text_color, console_data->background_color, console_data->font, console_data->alignment, console_data->word_wrap, false);
}

// ------------------------------------------------------------------------------------------------------------ //

void console_layer_writeln_text_and_image(Layer *console_layer, GBitmap *image, char *text) {
  console_data_struct *console_data = (console_data_struct*)layer_get_data(console_layer);
  console_layer_write_text_and_image_styled(console_layer, image, text, console_data->text_color, console_data->background_color, console_data->font, console_data->alignment, console_data->word_wrap, true);
}

// ------------------------------------------------------------------------------------------------------------ //

void console_layer_write_text_styled(Layer *console_layer, char *text, GColor text_color, GColor background_color, GFont font, GTextAlignment alignment, int word_wrap, bool advance) {
  console_layer_write_text_and_image_styled(console_layer, NULL_IMAGE, text, text_color, background_color, font, alignment, word_wrap, advance);
}

// ------------------------------------------------------------------------------------------------------------ //

void console_layer_write_image_styled(Layer *console_layer, GBitmap *image, GColor background_color, GTextAlignment alignment, bool advance) {
  if(image)
    console_layer_write_text_and_image_styled(console_layer, image, " ", GColorInherit, background_color, GFontInherit, alignment, WordWrapFalse, advance);
}

// ------------------------------------------------------------------------------------------------------------ //

void console_layer_write_text(Layer *console_layer, char *text) {
  console_data_struct *console_data = (console_data_struct*)layer_get_data(console_layer);
  console_layer_write_text_and_image_styled(console_layer, NULL_IMAGE, text, console_data->text_color, console_data->background_color, console_data->font, console_data->alignment, console_data->word_wrap, false);
}

// ------------------------------------------------------------------------------------------------------------ //

void console_layer_writeln_text(Layer *console_layer, char *text) {
  console_data_struct *console_data = (console_data_struct*)layer_get_data(console_layer);
  console_layer_write_text_and_image_styled(console_layer, NULL_IMAGE, text, console_data->text_color, console_data->background_color, console_data->font, console_data->alignment, console_data->word_wrap, true);
}

// ------------------------------------------------------------------------------------------------------------ //

void console_layer_write_image(Layer *console_layer, GBitmap *image) {
  console_data_struct *console_data = (console_data_struct*)layer_get_data(console_layer);
  console_layer_write_image_styled(console_layer, image, console_data->background_color, console_data->alignment, false);
}

// ------------------------------------------------------------------------------------------------------------ //

void console_layer_writeln_image(Layer *console_layer, GBitmap *image) {
  console_data_struct *console_data = (console_data_struct*)layer_get_data(console_layer);
  console_layer_write_image_styled(console_layer, image, console_data->background_color, console_data->alignment, true);
}

// ------------------------------------------------------------------------------------------------------------ //





// ------------------------------------------------------------------------------------------------------------ //
// Draw Layer
// ------------------------------------------------------------------------------------------------------------ //
static void console_layer_update(Layer *console_layer, GContext *ctx) {
  console_data_struct *console_data = (console_data_struct*)layer_get_data(console_layer);
  GRect bounds = layer_get_bounds(console_layer);
  graphics_context_set_stroke_width(ctx, 1);

  // Layer Background
  if(console_data->layer_background_color.argb!=GColorClear.argb) {
    graphics_context_set_fill_color(ctx, console_data->layer_background_color);
    graphics_fill_rect(ctx, (GRect){.origin = GPoint(0, 0), .size = bounds.size}, 0, GCornerNone);
  }

  // If there's a border, inset the layer's contents
  if(console_data->border_enabled && console_data->border_thickness>0)
    bounds = grect_inset(bounds, GEdgeInsets(console_data->border_thickness));

  // Set internal margin for the layer (TODO: Maybe add this as an external setting?)
  int margin_top_bottom = 0;
  int margin_left_right = 1;
  GRect margin_bounds = grect_inset(bounds, GEdgeInsets(margin_top_bottom, margin_left_right));

  // Display Rows
  int16_t y = margin_bounds.size.h; // Start at the bottom
  int16_t row_height = 0;    // row_height = tallest font on the row
   
  // Get past the EOF 0
  intptr_t cursor = console_data->pos + 1;
  
  // Make advance=true so if bounds.size.h==0 it will just quit
  bool advance = true;

  // adding "|| !advance" so all text in multiple-text-segments-on-one-row which are half cutoff by the top border are all displayed
  while ((y>margin_bounds.origin.y || !advance) && console_data->buffer[cursor % console_data->buffer_size]) {  // While text is within visible bounds && not at EOF
    advance = false;
    
    // First thing is the Settings
    uint8_t settings = console_data->buffer[cursor % console_data->buffer_size];
    
    bool word_wrap = settings&WORD_WRAP_INHERIT_BIT ? console_data->layer_word_wrap : settings&WORD_WRAP_BIT;

    // This could be quicker if I could just assume the enum: GTextAlignmentLeft=0, Center=1 and Right=2 (which it does.)  But I can't cause it'd lose abstraction.
    GTextAlignment alignment;
    switch (settings & ALIGNMENT_BITS) {
      case 0b0000: alignment = GTextAlignmentLeft;   break;
      case 0b0100: alignment = GTextAlignmentCenter; break;
      case 0b1000: alignment = GTextAlignmentRight;  break;
      default:     alignment = console_data->layer_alignment;
    }

    GColor background_color = console_data->layer_background_color;  // Assume inherit from layer
    if (settings&BACKGROUND_COLOR_BIT)
      background_color = (GColor){.argb=console_data->buffer[++cursor % console_data->buffer_size]};

    GColor text_color = console_data->layer_text_color;  // Assume inherit from layer
    if (settings&TEXT_COLOR_BIT)
      text_color = (GColor){.argb=console_data->buffer[++cursor % console_data->buffer_size]};
    graphics_context_set_text_color(ctx, text_color.argb ? text_color : console_data->layer_text_color);

    GFont font = console_data->layer_font;  // Assume inherit from layer
    if (settings&FONT_BIT)
      for (uintptr_t i=0; i<sizeof(GFont); i++) {
      ((uint8_t*)&font)[(sizeof(GFont)-1)-i] = console_data->buffer[++cursor % console_data->buffer_size];
    }

    GRect rect = GRectZero;
    GBitmap *image = NULL;
    if(settings&IMAGE_BIT) {
      // Read image
      for (uintptr_t i=0; i<sizeof(image); i++)
        ((uint8_t*)&image)[(sizeof(image)-1)-i] = console_data->buffer[++cursor % console_data->buffer_size];
      rect.size = gbitmap_get_bounds(image).size;
      switch (alignment) {
        case GTextAlignmentCenter: rect.origin.x = (margin_bounds.size.w - rect.size.w) / 2; break;
        case GTextAlignmentRight:  rect.origin.x = (margin_bounds.size.w - rect.size.w)    ; break;
        default: break;
      }
    }

    // Copy the 0-terminated string into a temp buffer (because pebble's text functions can't wrap around end of buffer)
    //char text[console_data->buffer_size + 1];         // allocate on stack (Locks up when using DictationAPI)
    char *text = malloc(console_data->buffer_size + 1); // allocate on heap
    if(text) {
      intptr_t i = -1;
      do {
        if (i < (intptr_t)(console_data->buffer_size)) i++;     // Stop it from maxing out
        text[i] = console_data->buffer[++cursor % console_data->buffer_size];
        if(text[i]==10) advance = true;  // If there ever is a 10 (newline), advance
      } while (text[i]);
      //end text

      // If we've not gone beyond the size of the buffer, then the data should be valid
      // If we HAVE gone beyond the buffer_size, buffer[cursor%size] is pointing at 0, exiting the while loop
      if(cursor - console_data->pos < console_data->buffer_size) {
        cursor++;  // Get past the string terminating 0 so the while can loop (unless we're now at the EOF 0)

        // Advance or not -- advance means moving text drawing to the next row up
        if (advance) {
          y -= row_height;
          row_height = 0;
        }

        // Draw the row background, if there is one
        // Calculate row height, draw the background if it has changed
        // object_height = height of current text to draw or height of image to draw
        // row_height = height of tallest text drawn on same row (without advance, e.g. without writeln())
        int16_t text_height = graphics_text_layout_get_content_size(word_wrap?text:" ", font, GRect(0, 0, margin_bounds.size.w, 0x7FFF), GTextOverflowModeTrailingEllipsis, alignment).h;
        int16_t object_height = rect.size.h>text_height ? rect.size.h : text_height; // Height of the current image/text being drawn is the max of the two
        if(object_height>row_height) {
          if(background_color.argb!=GColorClear.argb) {
            graphics_context_set_fill_color(ctx, background_color);
            graphics_fill_rect(ctx, GRect(bounds.origin.x, bounds.origin.y + y - object_height, bounds.size.w, object_height - row_height), 0, GCornerNone);  // fill background (or horizontal sliver if difference in font height or multi-line height)
          }
          row_height = object_height;
        }

        // Draw the image
        if(settings&IMAGE_BIT && image) {
          graphics_context_set_compositing_mode(ctx, GCompOpSet);
          graphics_draw_bitmap_in_rect(ctx, image, GRect(margin_bounds.origin.x + rect.origin.x, margin_bounds.origin.y + y - rect.size.h, rect.size.w, rect.size.h));
        }
        // Render Text (y-3 because Pebble's text rendering is dumb and goes outside rect)
          graphics_draw_text(ctx, text, font, GRect(margin_bounds.origin.x, margin_bounds.origin.y + (y-3) - text_height, margin_bounds.size.w, text_height), GTextOverflowModeTrailingEllipsis, alignment, NULL);  // align-bottom
        //graphics_draw_text(ctx, text, font, GRect(margin_bounds.origin.x, margin_bounds.origin.y + (y-3) - row_height,  margin_bounds.size.w, row_height ), GTextOverflowModeTrailingEllipsis, alignment, NULL);  // align-top
      } // END if data valid
      free(text);
    }
  } // END While

  // Draw Header (no internal margin)
  int16_t header_height = 0;
  if(console_data->header_enabled) {
    header_height = graphics_text_layout_get_content_size(console_data->header_text, console_data->header_font, bounds, GTextOverflowModeTrailingEllipsis, console_data->header_text_alignment).h;
    if(header_height>0) {
      if(console_data->header_background_color.argb!=GColorClear.argb)
        graphics_context_set_fill_color(ctx, console_data->header_background_color);
      else if(console_data->layer_background_color.argb!=GColorClear.argb)
        graphics_context_set_fill_color(ctx, console_data->layer_background_color);
      else
        graphics_context_set_fill_color(ctx, GColorWhite);  // The Console Layer Library currently doesn't support clear headers due to text/images writing into the header area
      graphics_fill_rect(ctx, GRect(bounds.origin.x, bounds.origin.y, bounds.size.w, header_height), 0, GCornerNone);
      
      if(console_data->header_text_color.argb!=GColorClear.argb) {
        graphics_context_set_text_color(ctx, console_data->header_text_color);
        graphics_draw_text(ctx, console_data->header_text, console_data->header_font, GRect(bounds.origin.x, bounds.origin.y - 3, bounds.size.w, header_height), GTextOverflowModeTrailingEllipsis, console_data->header_text_alignment, NULL);  // y-3 because Pebble's text rendering goes outside rect
      }
    }
  } // END Draw Header

  // Draw Border
  if(console_data->border_enabled && console_data->border_thickness>0 && console_data->border_color.argb!=GColorClear.argb) {
    graphics_context_set_fill_color  (ctx, console_data->border_color);
    graphics_context_set_stroke_color(ctx, console_data->border_color);
    if(header_height>0)
      graphics_draw_line(ctx, GPoint(bounds.origin.x, bounds.origin.y + header_height), GPoint(bounds.origin.x + bounds.size.w, bounds.origin.y + header_height));
    bounds = layer_get_bounds(console_layer);
    graphics_fill_rect(ctx, GRect(0, 0, bounds.size.w, console_data->border_thickness), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(0, 0, console_data->border_thickness, bounds.size.h), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(bounds.size.w-console_data->border_thickness, 0, console_data->border_thickness, bounds.size.h), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(0, bounds.size.h-console_data->border_thickness, bounds.size.w, console_data->border_thickness), 0, GCornerNone);
  } // END Draw Border
}

// ------------------------------------------------------------------------------------------------------------ //








// ------------------------------------------------------------------------------------------------------------ //
// Create Layer
// ------------------------------------------------------------------------------------------------------------ //

Layer* console_layer_create_with_buffer_size(GRect frame, int buffer_size) {
  Layer *console_layer;
  size_t data_size = sizeof (console_data_struct) + buffer_size;

  if((console_layer = layer_create_with_data(frame, data_size))) {
    console_data_struct *console_data = (console_data_struct*)layer_get_data(console_layer);
    console_data->buffer = (char*)(&(console_data->buffer) + sizeof(console_data->buffer));  // Point buffer to memory allocated just after the struct.  Assumes buffer is the last type inside struct.
    console_data->buffer_size = buffer_size;

    layer_set_clips(console_layer, true);
    console_layer_set_layer_style(console_layer, GColorBlack, GColorClear, fonts_get_system_font(FONT_KEY_GOTHIC_14), GTextAlignmentLeft, WordWrapFalse, true);
    console_layer_set_border_style(console_layer, false, GColorBlack, 1);
    console_layer_set_header_style(console_layer, false, GColorBlack, GColorLightGray, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), GTextAlignmentCenter);
    console_layer_set_header_text(console_layer, " ");  // If header is "" then no header is displayed (if enabled)
    console_layer_clear(console_layer);
    layer_set_update_proc(console_layer, console_layer_update);
  }
  return console_layer;
}

// ------------------------------------------------------------------------------------------------------------ //

Layer* console_layer_create(GRect frame) {
  return console_layer_create_with_buffer_size(frame, DEFAULT_BUFFER_SIZE);
}

// ------------------------------------------------------------------------------------------------------------ //








// ------------------------------------------------------------------------------------------------------------ //
// Internal Funciton for Debugging
// ------------------------------------------------------------------------------------------------------------ //
void log_buffer(Layer *console_layer) {
  console_data_struct *console_data = (console_data_struct*)layer_get_data(console_layer);
  printf("Head/Tail Separator Position: %d", (int)console_data->pos);
  //for(int i=console_data->pos; i<console_data->buffer_size; i++)  // Log from current position (the head) to the end of the buffer
  for(uint i=0; i<console_data->buffer_size; i++)                    // Log the whole buffer
    if(console_data->buffer[i]<=127 && console_data->buffer[i]>=32)
    printf("buffer[%d] = %d (%x) '%c'", (int)i, console_data->buffer[i], console_data->buffer[i], console_data->buffer[i]);
    else
    printf("buffer[%d] = %d (%x)", (int)i, console_data->buffer[i], console_data->buffer[i]);
}
