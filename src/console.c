#include "console.h"
// ------------------------------------------------------------------------------------------------------------ //
//  Data Structure
// ------------------------------------------------------------------------------------------------------------ //
/* Buffer holds:
       0 = Head/Tail EOF terminator
       0 = End of string
       Text is now written (from last char to first char) backwards which stores (and is therefore read) in memory forwards
       This is instead of it being written (from last char to first char) forwards causing it to be stored and read backwards
       It writes last char to first char in case the string written is larger than the buffer.  So the head eats the tail.

 ***********TOFIX Data Layout:  0zyxwvut...cbaFONTCBASzyx...cbaFONTCBAS0<buffer pos points to 0 at the tail
 ***********TOFIX Single Chunk = [previous S or 0]stringFONTCBAS0

0bABCDEFGH = settings
  A        1 bit:  Image Included? (1=yes, 2=no)
   B       1 bit:  Change Background Color? (1=yes, 2=no)
    C      1 bit:  Change       Text Color? (1=yes, 2=no)
     D     1 bit:  Change      Font? (1=yes, 2=no)
      EF   2 bits: Alignment [left=00, center=01, right=10, 11=inherit]
        GH 2 bits: Word Wrap [00 = no, 01 = yes, 1x (10 and 11) = inherit]
        G  1 bit:  Change        Word Wrap? (1=yes:change, 2=no:inherit)
         H 1 bit:  Word Wrap (If G=1, else Unused)

Maybe advance with /n or 

#define            IMAGE_BIT 0b10000000
#define          ADVANCE_BIT 0b01000000
#define            COLOR_BIT 0b00100000
#define             FONT_BIT 0b00010000
#define       ALIGNMENT_BITS 0b00001100
#define       WORD_WRAP_BITS 0b00000011


#define             FONT_BIT 0b01000000
#define       TEXT_COLOR_BIT 0b00100000
#define BACKGROUND_COLOR_BIT 0b00010000
#define       ATTRIBUTES_BIT 0b00001000
#define          ADVANCE_BIT 0b00000100
#define            IMAGE_BIT 0b00000010






Failed Plan:

0bABCDEFGH = settings
  A        1 bit:  Image = 1, Text = 0
   B       1 bit:  Advance? (1=yes, 2=no): Moves text printing's y location up if yes
    C      1 bit:  Change    Colors? (1=yes, 2=no)
     D     1 bit:  Change      Font? (1=yes, 2=no)
      EF   2 bits: Alignment [left=00, center=01, right=10, 11=inherit]
        GH 2 bits: Word Wrap [00 = no, 01 = yes, 1x (10 and 11) = inherit]
        G  1 bit:  Change        Word Wrap? (1=yes:change, 2=no:inherit)
         H 1 bit:  Word Wrap (If G=1, else Unused)

0b0------- = Text
0b0BCDEFGH = settings
  0        1 bit:  Text = 0
   B       1 bit:  Advance? (1=yes, 2=no): Moves text printing's y location up if yes
    C      1 bit:  Change    Colors? (1=yes, 2=no)
     D     1 bit:  Change      Font? (1=yes, 2=no)
      EF   2 bits: Text Alignment    (00=left, 01=center, 10=right, 11=inherit)
        GH 2 bits: Word Wrap         (00 = no, 01 = yes,  10 and 11 = inherit)
        G  1 bit:  Change Word Wrap? (1=yes, 2=no)
         H 1 bit:  Word Wrap

0b1------- = Image
0b1BCDEFGH = settings
  1        1 bit:  Image = 1
   B       1 bit:  Advance? (1=yes, 2=no): Moves text printing's y location up if yes
    C      1 bit:  Change Color? (1=yes, 2=no)
     D     1 bit:  Unused
      EF   2 bits: Image Alignment [left=00, center=01, right=10, 11=inherit]
        GH 2 bits: unused




0bABCDEFGH = settings
  A        1 bit:  Image = 1, Text = 0
   B       1 bit:  Change             Font? (1=yes, 2=no)
    C      1 bit:  Change Background Color? (1=yes, 2=no)
     D     1 bit:  Change       Text Color? (1=yes, 2=no)
      EF   2 bits: Text Alignment [left=00, center=01, right=10, 11=inherit]
        GH 2 bits: Word Wrap [00 = no, 01 = yes, 1x (10 and 11) = inherit]
        G  1 bit:  Change        Word Wrap? (1=yes:change, 2=no:inherit)
         H 1 bit:  Word Wrap (If G=1, else Unused)

0b0xxxxxxx = Text
0b0BCDEFGH = settings
  0        1 bit:  Text = 0
   B       1 bit:  Change             Font? (1=yes, 2=no)
    C      1 bit:  Change Background Color? (1=yes, 2=no)
     D     1 bit:  Change       Text Color? (1=yes, 2=no)
      EF   2 bits: Text Alignment [left=00, center=01, right=10, 11=inherit]
        GH 2 bits: Word Wrap [00 = no, 01 = yes, 1x (10 and 11) = inherit]
        G  1 bit:  Change        Word Wrap? (1=yes, 2=no)
         H 1 bit:  Word Wrap

0b1xxxxxxx = Image
0b1BCDEFGH = settings
  A        1 bit:  Image = 1
   B       1 bit:  Unused
    C      1 bit:  Change Background Color? (1=yes, 2=no)
     D     1 bit:  Unused
      EF   2 bits: Image Alignment [left=00, center=01, right=10, 11=inherit]
        GH 2 bits: unused

Old Method:
         0 = Head/Tail terminator
        10 = new line
    32-127 = ascii
   128-255 = End of section, change font, color, alignment, etc

   advance vertically or keep same row
   write vs writeln

0bABCDEFGH = settings
  A        1 bit: High Bit (must = 1 to denote not ASCII string)
   B       1 bit: Change               Font? (1=yes, 2=no)
    C      1 bit: Change   Background Color? (1=yes, 2=no)
     D     1 bit: Change         Text Color? (1=yes, 2=no)
      E    1 bit: Change Alignment/WordWrap? (1=yes, 2=no)
       F   1 bit: Advance? (1=yes, 2=no): Moves text printing's y location up if yes
        G  1 bit: Image = 1, Text = 0
         H 1 bit:

0bABCDEEFF = attributes
      ee   2 bits: Text Alignment [left=00, center=01, right=10, 11=inherit]
        ff 2 bits: Word Wrap [00 = no, 01 = yes, 1x (10 and 11) = inherit]


//
// Data Layout:  0zyxwvut...cbaFONTCBASzyx...cbaFONTCBAS0<buffer pos points to 0 at the tail
// Single Chunk = [previous S or 0]stringFONTCBAS0
//                                               ^ *pos points to Begin/End split point
// 0x00       = 1 byte: Circular Buffer Begin/End split point (must = 0)
// 0xFONTFONT = 4 byte: Font Pointer (optional, if settings bit b=1)
// 0xCC       = 1 byte: Text Color (optional, if settings bit c=1)
// 0xBB       = 1 byte: Text Background Color (optional, if settings bit d=1)
// 0xAA       = 1 byte: Attributes (optional, if settings bit e=1)
//         0babcdeeff = Attributes
//           abcd     4 bits: unused
//               ee   2 bits: Text Alignment [left=00, center=01, right=10, 11=inherit]
//                 ff 2 bits: Word Wrap [00 = no, 01 = yes, 1x (10 and 11) = inherit]
//                 x  1 bit:  Word Wrap Inherit (0 = no, 1 = yes)
//                  y 1 bit:  If Word Wrap Inherit = yes, Word Wrap (0 = no, 1 = yes)
// 0xSS       = 1 byte: Settings
//         0b1bcdefgh = Settings
//           1        1 bit: High Bit (must = 1 to denote not ASCII string)
//            b       1 bit: Change               Font? (1=yes, 2=no)
//             c      1 bit: Change         Text Color? (1=yes, 2=no)
//              d     1 bit: Change   Background Color? (1=yes, 2=no)
//               e    1 bit: Change Alignment/WordWrap? (1=yes, 2=no)
//                f   1 bit: Advance? (1=yes, 2=no): Moves text printing's y location up if yes
//                 g  1 bit: unused
//                  h 1 bit: unused

*/

typedef struct console_data_struct {
  bool               dirty_layer_after_writing;
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

#define DEFAULT_BUFFER_SIZE 500  // bytes

#define             IMAGE_BIT  0b10000000 // Image Included? (1=yes, 2=no)
#define  BACKGROUND_COLOR_BIT  0b01000000 // Change Background Color? (1=yes, 2=no)
#define        TEXT_COLOR_BIT  0b00100000 // Change       Text Color? (1=yes, 2=no)
#define              FONT_BIT  0b00010000 // Change             Font? (1=yes, 2=no)
#define         ALIGNMENT_BITS 0b00001100 // Alignment [left=00, center=01, right=10, 11=inherit]
#define         WORD_WRAP_BITS 0b00000011 // Word Wrap [00 = no, 01 = yes, 1x (10 and 11) = inherit]
#define WORD_WRAP_INHERIT_BIT  0b00000010 // Change        Word Wrap? (1=yes, 2=no)
#define         WORD_WRAP_BIT  0b00000001 // Word Wrap (If WORD_WRAP_INHERIT_BIT=1, else Unused)


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

bool           console_layer_get_dirty_automatically    (Layer *console_layer) {return ((console_data_struct*)layer_get_data(console_layer))->dirty_layer_after_writing;}


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
void console_layer_set_layer_text_color       (Layer *console_layer, GColor         layer_text_color)         {((console_data_struct*)layer_get_data(console_layer))->layer_text_color = layer_text_color;}
void console_layer_set_layer_alignment        (Layer *console_layer, GTextAlignment layer_alignment)          {((console_data_struct*)layer_get_data(console_layer))->layer_alignment = layer_alignment;}
void console_layer_set_layer_word_wrap        (Layer *console_layer, bool           layer_word_wrap)          {((console_data_struct*)layer_get_data(console_layer))->word_wrap = layer_word_wrap;}
void console_layer_set_layer_font             (Layer *console_layer, GFont          layer_font)               {((console_data_struct*)layer_get_data(console_layer))->layer_font = layer_font;}

void console_layer_set_background_color       (Layer *console_layer, GColor         background_color)         {((console_data_struct*)layer_get_data(console_layer))->background_color = background_color;}
void console_layer_set_alignment              (Layer *console_layer, GTextAlignment alignment)                {((console_data_struct*)layer_get_data(console_layer))->alignment             = alignment;}
void console_layer_set_text_color             (Layer *console_layer, GColor         text_color)               {((console_data_struct*)layer_get_data(console_layer))->text_color            = text_color;}
void console_layer_set_word_wrap              (Layer *console_layer, int            word_wrap)                {((console_data_struct*)layer_get_data(console_layer))->word_wrap             = word_wrap;}
void console_layer_set_font                   (Layer *console_layer, GFont          font)                     {((console_data_struct*)layer_get_data(console_layer))->font                  = font;}

void console_layer_set_dirty_automatically    (Layer *console_layer, bool           dirty_layer_after_writing){((console_data_struct*)layer_get_data(console_layer))->dirty_layer_after_writing = dirty_layer_after_writing;}

// ------------------------------------------------------------------------------------------------------------ //

void console_layer_set_text_colors                  (Layer *console_layer, GColor text_color, GColor background_color) {
  console_layer_set_text_color(console_layer, text_color);
  console_layer_set_background_color(console_layer, background_color);
}

// ------------------------------------------------------------------------------------------------------------ //

void console_layer_set_layer_style (Layer         *console_layer,
                                    GColor         layer_text_color,
                                    GColor         layer_background_color,
                                    GFont          layer_font,
                                    GTextAlignment layer_alignment,
                                    bool           layer_word_wrap,
                                    bool           dirty_layer_after_writing) {
  console_data_struct *console_data = (console_data_struct*)layer_get_data(console_layer);
  console_data->layer_text_color             = layer_text_color;
  console_data->layer_background_color       = layer_background_color;
  console_data->layer_font                   = layer_font;
  console_data->layer_word_wrap              = layer_word_wrap;
  console_data->layer_alignment              = layer_alignment;
  console_data->dirty_layer_after_writing    = dirty_layer_after_writing;
}

// ------------------------------------------------------------------------------------------------------------ //

void console_layer_set_border_style(Layer *console_layer, bool border_enabled, GColor border_color, int border_thickness) {
  console_data_struct *console_data = (console_data_struct*)layer_get_data(console_layer);
  console_data->border_color     = border_color;
  console_data->border_thickness = border_thickness;
  console_data->border_enabled   = border_enabled;
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
}

// ------------------------------------------------------------------------------------------------------------ //








// ------------------------------------------------------------------------------------------------------------ //
// Write Layer
// ------------------------------------------------------------------------------------------------------------ //
void console_layer_write_text_and_image_styled(Layer         *console_layer,
                                               GBitmap       *image,
                                               char          *text,
                                               GColor         text_color,
                                               GColor         background_color,
                                               GFont          font,
                                               GTextAlignment alignment,
                                               int            word_wrap,
                                               bool           advance) {

  console_data_struct *console_data = (console_data_struct*)layer_get_data(console_layer);

  console_data->pos += ((UINTPTR_MAX - (UINTPTR_MAX % console_data->buffer_size)) - console_data->buffer_size);

  // Copy text (forwards in memory, but from last char to first char) to buffer
  char *begin, *end;
  while(*text || image) {
    uint8_t settings = 0;  // new settings each time

    
    
    // Adding feature: Draw text on top of image
    begin = text;
    if(image) {
      settings = IMAGE_BIT;
      while(*text) text++;   // ends on 0
    } else {
      while((*text) && ((*text)!=10)) text++;   // ends on 0 or 10 (newline)
    }
    end = text;  // don't write 10 or 0
    
    
    // write 0 no matter if 10 or 0
    console_data->buffer[console_data->pos-- % console_data->buffer_size] = 0;
    if(*text==10) {
      console_data->buffer[console_data->pos-- % console_data->buffer_size] = 10;
      text++; // skip past 10
    } else if (advance) {
      console_data->buffer[console_data->pos-- % console_data->buffer_size] = 10;
      printf("did a 10");
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

  if(console_data->dirty_layer_after_writing)
    layer_mark_dirty(console_layer);
}

// ------------------------------------------------------------------------------------------------------------ //

void console_layer_write_text_styled(Layer         *console_layer,
                                     char          *text,
                                     GColor         text_color,
                                     GColor         background_color,
                                     GFont          font,
                                     GTextAlignment alignment,
                                     int            word_wrap,
                                     bool           advance) {
  console_layer_write_text_and_image_styled(console_layer, NULL /*image*/, text, text_color, background_color, font, alignment, word_wrap, advance);
}

//   console_data_struct *console_data = (console_data_struct*)layer_get_data(console_layer);

//   console_data->pos += ((UINTPTR_MAX - (UINTPTR_MAX % console_data->buffer_size)) - console_data->buffer_size);

//   // Copy text (forwards in memory, but from last char to first char) to buffer
//   char *begin, *end;
//   while(*text) {
//     uint8_t settings = 0;  // new settings each time
    
//     begin = text;
//     while((*text) && ((*text)!=10)) text++;   // ends on 0 or 10
//     end = text;  // don't write 10 or 0
    
//     // write 0 no matter if 10 or 0
//     console_data->buffer[console_data->pos-- % console_data->buffer_size] = 0;
//     if(*text==10) {
//       console_data->buffer[console_data->pos-- % console_data->buffer_size] = 10;
//       text++; // skip past 10
//       //settings |= ADVANCE_BIT;  // there was a 10, so probably wanna advance the rows
//     }
    
//     // Copy string to buffer (forwards in memory) from end to beginning
//     while(end!=begin)
//       console_data->buffer[console_data->pos-- % console_data->buffer_size] = *(--end);
    
//     // Copy Image to buffer
//     // Not in write_text functions
//     //settings |= IMAGE_BIT;
    
//     // Copy header information
//     if(font) {
//       for (uintptr_t i=0; i<sizeof(font); i++)
//         console_data->buffer[console_data->pos-- % console_data->buffer_size] = ((uint8_t*)&font)[i];
//       settings |= FONT_BIT;
//     }

//     if(text_color.argb) {
//       console_data->buffer[console_data->pos-- % console_data->buffer_size] = text_color.argb;
//       settings |= TEXT_COLOR_BIT;
//     }

//     if(background_color.argb) {
//       console_data->buffer[console_data->pos-- % console_data->buffer_size] = background_color.argb;
//       settings |= BACKGROUND_COLOR_BIT;
//     }

//     settings |= (word_wrap&WORD_WRAP_BITS);
//     settings |= (alignment==GTextAlignmentLeft?0b0000 : alignment==GTextAlignmentCenter?0b0100 : alignment==GTextAlignmentRight?0b1000 : 0b1100);

//     console_data->buffer[console_data->pos-- % console_data->buffer_size] = settings; // Save settings
//     console_data->buffer[console_data->pos % console_data->buffer_size] = 0;          // EOF -- Head/Tail buffer transition point
//   }

//   console_data->pos %= console_data->buffer_size;

//   if(console_data->dirty_layer_after_writing)
//     layer_mark_dirty(console_layer);
// }












// void console_layer_write_text_styled(Layer         *console_layer,
//                                      char          *text,
//                                      GColor         text_color,
//                                      GColor         background_color,
//                                      GFont          font,
//                                      GTextAlignment alignment,
//                                      int            word_wrap,
//                                      bool           advance) {
  
//   console_data_struct *console_data = (console_data_struct*)layer_get_data(console_layer);

//   console_data->pos += ((UINTPTR_MAX - (UINTPTR_MAX % console_data->buffer_size)) - console_data->buffer_size);

//   // Copy text (forwards in memory, but from last char to first char) to buffer
//   char *reverse_text = text;
  
//   while(*reverse_text) reverse_text++;   // ends on 0
  
//   do {
//     console_data->buffer[console_data->pos-- % console_data->buffer_size] = *reverse_text;
//   } while((reverse_text--)!=text);
  
//   // Copy header information
//   //TODO: Try settings = 0
//   uint8_t settings = 0b10000000;
//   if(font) {
//     for (uintptr_t i=0; i<sizeof(font); i++)
//       console_data->buffer[console_data->pos-- % console_data->buffer_size] = ((uint8_t*)&font)[i];
//     settings |= FONT_BIT;
//   }

//   if(text_color.argb) {
//     console_data->buffer[console_data->pos-- % console_data->buffer_size] = text_color.argb;
//     settings |= TEXT_COLOR_BIT;
//   }

//   if(background_color.argb) {
//     console_data->buffer[console_data->pos-- % console_data->buffer_size] = background_color.argb;
//     settings |= BACKGROUND_COLOR_BIT;
//   }

//   uint8_t attributes = (alignment==GTextAlignmentLeft?0b0000 : alignment==GTextAlignmentCenter?0b0100 : alignment==GTextAlignmentRight?0b1000 : 0b1100)  + (word_wrap&3);
//   if(attributes != (0b00001100+WordWrapInherit)) {
//     console_data->buffer[console_data->pos-- % console_data->buffer_size] = attributes;
//     settings |= ATTRIBUTES_BIT;
//   }

//   if(advance || *reverse_text==10)
//     settings |= ADVANCE_BIT;

//   console_data->buffer[console_data->pos-- % console_data->buffer_size] = settings;
//   console_data->buffer[console_data->pos % console_data->buffer_size] = 0;  // First 0 = EOF -- Head/Tail buffer transition point
//   //console_data->buffer[(console_data->pos - 1) % console_data->buffer_size] = 0;  // Second 0 = EOF -- Head/Tail buffer transition point

//   console_data->pos %= console_data->buffer_size;

//   if(console_data->dirty_layer_after_writing)
//     layer_mark_dirty(console_layer);
// }

// ------------------------------------------------------------------------------------------------------------ //

void console_layer_write_image_styled(Layer *console_layer, GBitmap *image, GColor background_color, GTextAlignment alignment, bool advance) {
    if(image)
      console_layer_write_text_and_image_styled(console_layer, image, " ", GColorInherit /*textcolor*/, background_color, NULL /*font*/, alignment, false /*wordwrap*/, advance);
}

//   if(image) {
//     //TODO try settings = 0
//     //uint8_t settings = 0b10000010;
//     uint8_t settings = IMAGE_BIT;

//     console_data_struct *console_data = (console_data_struct*)layer_get_data(console_layer);
    
//     console_data->pos += ((UINTPTR_MAX - (UINTPTR_MAX % console_data->buffer_size)) - console_data->buffer_size);

//     for (uintptr_t i=0; i<sizeof(image); i++) {
//       console_data->buffer[console_data->pos-- % console_data->buffer_size] = ((uint8_t*)&image)[i];
//     }

//     if(background_color.argb) {
//       console_data->buffer[console_data->pos-- % console_data->buffer_size] = background_color.argb;
//       settings |= BACKGROUND_COLOR_BIT;
//     }

//     uint8_t attributes = (alignment==GTextAlignmentLeft?0b0000 : alignment==GTextAlignmentCenter?0b0100 : alignment==GTextAlignmentRight?0b1000 : 0b1100);
//     if(attributes != 0b1100) {
//       console_data->buffer[console_data->pos-- % console_data->buffer_size] = attributes;
// //       settings |= ATTRIBUTES_BIT;
//     }

// //     if(advance)
// //       settings |= ADVANCE_BIT;

    
//     console_data->buffer[console_data->pos-- % console_data->buffer_size] = settings;

//     console_data->buffer[console_data->pos % console_data->buffer_size] = 0;          // 0 = EOF -- Head/Tail buffer transition point

//     console_data->pos %= console_data->buffer_size;        // Make sure pos is within buffer_size bounds

//     if(console_data->dirty_layer_after_writing)
//       layer_mark_dirty(console_layer);
//   }
// }

// ------------------------------------------------------------------------------------------------------------ //

void console_layer_clear(Layer *console_layer) {
  console_data_struct *console_data = (console_data_struct*)layer_get_data(console_layer);
  console_data->pos = 0;
  console_data->buffer[0] = 0;
  console_data->buffer[1] = 0;
  console_data->buffer[console_data->buffer_size - 1] = 0;

  console_data->background_color = GColorInherit;
  console_data->text_color       = GColorInherit;
  console_data->font             = GFontInherit;
  console_data->alignment        = GTextAlignmentInherit;
  console_data->word_wrap        = WordWrapInherit;

  if(console_data->dirty_layer_after_writing)
    layer_mark_dirty(console_layer);
}

// ------------------------------------------------------------------------------------------------------------ //

void console_layer_write_text(Layer *console_layer, char *text) {
  console_data_struct *console_data = (console_data_struct*)layer_get_data(console_layer);
  console_layer_write_text_styled(console_layer, text, console_data->text_color, console_data->background_color, console_data->font, console_data->alignment, console_data->word_wrap, false);
}

// ------------------------------------------------------------------------------------------------------------ //

void console_layer_writeln_text(Layer *console_layer, char *text) {
  console_data_struct *console_data = (console_data_struct*)layer_get_data(console_layer);
  console_layer_write_text_styled(console_layer, text, console_data->text_color, console_data->background_color, console_data->font, console_data->alignment, console_data->word_wrap, true);
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
  
  // SAVE CONTEXT
  //GContext ctx;
  //memcpy(&ctx, ctx_old, sizeof(ctx));
  
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
  
  // Note: If you want an internal margin for the whole layer, uncomment this:
  //int margin = 0; bounds = grect_inset(bounds, GEdgeInsets(margin));

  // Display Rows
  int16_t y = bounds.size.h; // Start at the bottom
  int16_t row_height = 0; // row_height = tallest font on the row
   
  // Get past the EOF 0
  intptr_t cursor = console_data->pos + 1;
  
  // Assume not advancing (gets us into the while loop)
  // TODO: Actually, maybe make advance=true, cause if bounds.size.h==0 it should quit
  bool advance = true;

  // adding "|| !advance" so all text in multiple-text-segments-on-one-row which are half cutoff by the top border are all displayed
  while ((y>bounds.origin.y || !advance) && console_data->buffer[cursor % console_data->buffer_size]) {  // While text is within visible bounds && not at EOF
    advance = false;
    // First thing is the Settings
    uint8_t settings = console_data->buffer[cursor % console_data->buffer_size];
    

    // Get Alignment and WordWrap out of Settings
    bool word_wrap = settings&WORD_WRAP_INHERIT_BIT ? console_data->layer_word_wrap : settings&WORD_WRAP_BIT;
    
    // This could be quicker if I could just assume the enum: GTextAlignmentLeft=0, Center=1 and Right=2 (which it does.)  But I can't cause it'd lose abstraction.
    GTextAlignment alignment;
    switch (settings & ALIGNMENT_BITS) {
      case 0b0000: alignment = GTextAlignmentLeft;   break;
      case 0b0100: alignment = GTextAlignmentCenter; break;
      case 0b1000: alignment = GTextAlignmentRight;  break;
      default:     alignment = console_data->layer_alignment;
    }

    GColor background_color = console_data->layer_background_color;
    if (settings&BACKGROUND_COLOR_BIT)
      background_color = (GColor){.argb=console_data->buffer[++cursor % console_data->buffer_size]};

    GColor text_color = console_data->layer_text_color;
    if (settings&TEXT_COLOR_BIT)
      text_color = (GColor){.argb=console_data->buffer[++cursor % console_data->buffer_size]};
    graphics_context_set_text_color(ctx, text_color.argb ? text_color : console_data->layer_text_color);

    GFont font = console_data->layer_font;
    if (settings&FONT_BIT)
      for (uintptr_t i=0; i<sizeof(GFont); i++) {
      ((uint8_t*)&font)[(sizeof(GFont)-1)-i] = console_data->buffer[++cursor % console_data->buffer_size];
    }



    GRect rect = GRect(0, 0, 0, 0);
    GBitmap *image = NULL;
    if(settings&IMAGE_BIT) {
      // Read image
      for (uintptr_t i=0; i<sizeof(image); i++)
        ((uint8_t*)&image)[(sizeof(image)-1)-i] = console_data->buffer[++cursor % console_data->buffer_size];
      rect.size = gbitmap_get_bounds(image).size;
      switch (alignment) {
          case GTextAlignmentLeft:   rect.origin.x = 0;                                 break;
          case GTextAlignmentCenter: rect.origin.x = (bounds.size.w - rect.size.w) / 2; break;
          case GTextAlignmentRight:  rect.origin.x = bounds.size.w - rect.size.w;       break;
        //case GTextAlignmentCenter: rect.origin.x = (bounds.size.w - gbitmap_get_bounds(image).size.w) / 2; break;
        //case GTextAlignmentRight:  rect.origin.x = bounds.size.w - gbitmap_get_bounds(image).size.w;       break;
      }
    }
    
    // Copy the 0-terminated string into a temp buffer (because pebble's text functions can't wrap around end of buffer)
    char text[console_data->buffer_size + 1];
    intptr_t i = -1;
    do {
      if (i < (intptr_t)(console_data->buffer_size)) i++;     // Stop it from maxing out
      text[i] = console_data->buffer[++cursor % console_data->buffer_size];
      if(text[i]==10) {  // If there ever is a 10 (newline), advance
        advance = true;
//         if(!image) i--; // Only allow multi-line text on top of an image (else should only appear at the end of a string)
      }
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
      int16_t text_height = graphics_text_layout_get_content_size(word_wrap?text:"0", font, bounds, GTextOverflowModeTrailingEllipsis, alignment).h;
      int16_t object_height = rect.size.h>text_height ? rect.size.h : text_height; // Height of the current image/text being drawn is the max of the two
      if(object_height>row_height) {
        if(background_color.argb!=GColorClear.argb) {
          graphics_context_set_fill_color(ctx, background_color);
          graphics_fill_rect(ctx, GRect(bounds.origin.x, bounds.origin.y + y - object_height, bounds.size.w, object_height - row_height), 0, GCornerNone);  // fill background (or horizontal sliver if difference in font height or multi-line height)
        }
        row_height = object_height;
      }

      // Draw the text or image
      if(image) {
        graphics_context_set_compositing_mode(ctx, GCompOpSet);
        graphics_draw_bitmap_in_rect(ctx, image, GRect(bounds.origin.x + rect.origin.x, bounds.origin.y + y - rect.size.h, rect.size.w, rect.size.h));
      }
      graphics_draw_text(ctx, text, font, GRect(bounds.origin.x, bounds.origin.y + (y-3) - text_height, bounds.size.w, text_height), GTextOverflowModeTrailingEllipsis, alignment, NULL);  // align-bottom  
      //graphics_draw_text(ctx, text, font, GRect(bounds.origin.x, bounds.origin.y + (y-3) - row_height,    bounds.size.w, row_height   ), GTextOverflowModeTrailingEllipsis, alignment, NULL);  // align-top
      // y-3 because Pebble's text rendering is dumb and goes outside rect
    } // END if data valid
  } // END While

  // Draw Header
  int16_t header_height = 0;
  if(console_data->header_enabled) {
    header_height = graphics_text_layout_get_content_size(console_data->header_text, console_data->header_font, bounds, GTextOverflowModeTrailingEllipsis, console_data->header_text_alignment).h;
    if(header_height>0) {
      if(console_data->header_background_color.argb!=GColorClear.argb) {
        graphics_context_set_fill_color(ctx, console_data->header_background_color);
        graphics_fill_rect(ctx, GRect(bounds.origin.x, bounds.origin.y, bounds.size.w, header_height), 0, GCornerNone);
      }
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
      
// static void console_layer_update(Layer *console_layer, GContext *ctx) {
  
//   // SAVE CONTEXT
//   //GContext ctx;
//   //memcpy(&ctx, ctx_old, sizeof(ctx));
  
//   console_data_struct *console_data = (console_data_struct*)layer_get_data(console_layer);
//   GRect bounds = layer_get_bounds(console_layer);
//   graphics_context_set_stroke_width(ctx, 1);

//   // Layer Background
//   if(console_data->layer_background_color.argb!=GColorClear.argb) {
//     graphics_context_set_fill_color(ctx, console_data->layer_background_color);
//     graphics_fill_rect(ctx, (GRect){.origin = GPoint(0, 0), .size = bounds.size}, 0, GCornerNone);
//   }

//   // If there's a border, inset the layer's contents
//   if(console_data->border_enabled && console_data->border_thickness>0)
//     bounds = grect_inset(bounds, GEdgeInsets(console_data->border_thickness));
  
//   // Note: If you want an internal margin, you can add it here
//   //int margin = 0; bounds = grect_inset(bounds, GEdgeInsets(margin));

//   // Display Rows
//   int16_t y = bounds.size.h; // Start at the bottom
//   int16_t object_height = 0;
//   int16_t row_height = 0; // row_height = tallest font on the row
//   intptr_t cursor = console_data->pos + 1;
//   bool advance = false;


//   // adding "|| !advance" so all text in multiple-text-segments-on-one-row which are half cutoff by the top border are all displayed
//   while ((y>bounds.origin.y || !advance) && console_data->buffer[cursor % console_data->buffer_size]) {  // While text is within visible bounds && not at EOF
//     uint8_t settings = console_data->buffer[cursor % console_data->buffer_size];

//     advance = (settings&ADVANCE_BIT) ? true : false;

//     // Get Alignment and WordWrap out of Settings
//     GTextAlignment alignment = console_data->layer_alignment;
//     bool word_wrap = console_data->layer_word_wrap;
//     if (settings&ATTRIBUTES_BIT) {
//       uint8_t attributes = console_data->buffer[++cursor % console_data->buffer_size];
//       // This could be quicker if I could just assume the enum: GTextAlignmentLeft=0, Center=1 and Right=2 (which it does.)  But I can't cause it'd lose abstraction.
//       switch ((attributes>>2) & 3) {
//         case 0: alignment = GTextAlignmentLeft;   break;
//         case 1: alignment = GTextAlignmentCenter; break;
//         case 2: alignment = GTextAlignmentRight;  break;
//         default: alignment = console_data->layer_alignment;
//       }
//       word_wrap = attributes&2 ? console_data->layer_word_wrap : attributes&1;
//     }

//     GColor background_color = console_data->layer_background_color;
//     if (settings&BACKGROUND_COLOR_BIT)
//       background_color = (GColor){.argb=console_data->buffer[++cursor % console_data->buffer_size]};

//     GColor text_color = console_data->layer_text_color;
//     if (settings&TEXT_COLOR_BIT)
//       text_color = (GColor){.argb=console_data->buffer[++cursor % console_data->buffer_size]};
//     graphics_context_set_text_color(ctx, text_color.argb ? text_color : console_data->layer_text_color);

//     GFont font = console_data->layer_font;
//     if (settings&FONT_BIT)
//       for (uintptr_t i=0; i<sizeof(GFont); i++) {
//       ((uint8_t*)&font)[(sizeof(GFont)-1)-i] = console_data->buffer[++cursor % console_data->buffer_size];
//     }



//     GRect rect = GRect(0, 0, 0, 0);
//     char text[console_data->buffer_size + 1];
//     GBitmap *image = NULL;
//     // If Image
//     if(settings&IMAGE_BIT) {
//       // Read image
//       for (uintptr_t i=0; i<sizeof(image); i++)
//         ((uint8_t*)&image)[(sizeof(image)-1)-i] = console_data->buffer[++cursor % console_data->buffer_size];
//       rect.size = gbitmap_get_bounds(image).size;
//       switch (alignment) {
//         case GTextAlignmentLeft:   rect.origin.x = 0;                                 break;
//         case GTextAlignmentCenter: rect.origin.x = (bounds.size.w - rect.size.w) / 2; break;
//         case GTextAlignmentRight:  rect.origin.x = bounds.size.w - rect.size.w;       break;
//         default:                   rect.origin.x = 0;                                 break;
//       }
//       object_height = rect.size.h;
//     } else {  // Else it's Text
//       // Copy the 0-terminated string into a temp buffer (because pebble's text functions can't wrap around end of buffer)
//       intptr_t i = -1;
//       do {
//         if (i < (intptr_t)(console_data->buffer_size)) i++;     // Stop it from maxing out
//         text[i] = console_data->buffer[++cursor % console_data->buffer_size];
//       } while (text[i]);
//       if(cursor - console_data->pos < console_data->buffer_size)  
//         object_height = graphics_text_layout_get_content_size(word_wrap?text:"0", font, bounds, GTextOverflowModeTrailingEllipsis, alignment).h;
//     }

//     // If we've not gone beyond the size of the buffer, then the data should be valid
//     // If we HAVE gone beyond the buffer_size, buffer[cursor%size] is pointing at 0, exiting the while loop
//     //  .... well, now that there's images, it might not point to 0, but as long as there's some text it eventually will.
//     if(cursor - console_data->pos < console_data->buffer_size) {
//       cursor++;
      
//       // Advance or not -- advance means moving text drawing to the next row up
//       if (advance) {
//         y -= row_height;
//         row_height = 0;
//       }

//       // Draw the row background, if there is one
//       // Calculate row height, draw the background if it has changed
//       // object_height = height of current text to draw or height of image to draw
//       // row_height = height of tallest text drawn on same row (without advance, e.g. without writeln())
//       if(object_height>row_height) {
//         if(background_color.argb!=GColorClear.argb) {
//           graphics_context_set_fill_color(ctx, background_color);
//           graphics_fill_rect(ctx, GRect(bounds.origin.x, bounds.origin.y + y - object_height, bounds.size.w, object_height - row_height), 0, GCornerNone);  // fill background (or horizontal sliver if difference in font height or multi-line height)
//         }
//         row_height = object_height;
//       }

//       // Draw the text or image
//       if(settings&IMAGE_BIT) {
//         graphics_context_set_compositing_mode(ctx, GCompOpSet);
//         graphics_draw_bitmap_in_rect(ctx, image, GRect(bounds.origin.x + rect.origin.x, bounds.origin.y + y - object_height, rect.size.w, object_height));
//       } else {
//         graphics_draw_text(ctx, text, font, GRect(bounds.origin.x, bounds.origin.y + (y-3) - object_height, bounds.size.w, object_height), GTextOverflowModeTrailingEllipsis, alignment, NULL);  // align-bottom  
//         //graphics_draw_text(ctx, text, font, GRect(bounds.origin.x, bounds.origin.y + (y-3) - row_height,    bounds.size.w, row_height   ), GTextOverflowModeTrailingEllipsis, alignment, NULL);  // align-top
//         // y-3 because Pebble's text rendering is dumb and goes outside rect
//       }
//     }
//   } // END While

//   // Draw Header
//   int16_t header_height = 0;
//   if(console_data->header_enabled) {
//     header_height = graphics_text_layout_get_content_size(console_data->header_text, console_data->header_font, bounds, GTextOverflowModeTrailingEllipsis, console_data->header_text_alignment).h;
//     if(header_height>0) {
//       if(console_data->header_background_color.argb!=GColorClear.argb) {
//         graphics_context_set_fill_color(ctx, console_data->header_background_color);
//         graphics_fill_rect(ctx, GRect(bounds.origin.x, bounds.origin.y, bounds.size.w, header_height), 0, GCornerNone);
//       }
//       if(console_data->header_text_color.argb!=GColorClear.argb) {
//         graphics_context_set_text_color(ctx, console_data->header_text_color);
//         graphics_draw_text(ctx, console_data->header_text, console_data->header_font, GRect(bounds.origin.x, bounds.origin.y - 3, bounds.size.w, header_height), GTextOverflowModeTrailingEllipsis, console_data->header_text_alignment, NULL);  // y-3 because Pebble's text rendering goes outside rect
//       }
//     }
//   } // END Draw Header

//   // Draw Border
//   if(console_data->border_enabled && console_data->border_thickness>0 && console_data->border_color.argb!=GColorClear.argb) {
//     graphics_context_set_fill_color  (ctx, console_data->border_color);
//     graphics_context_set_stroke_color(ctx, console_data->border_color);
//     if(header_height>0)
//       graphics_draw_line(ctx, GPoint(bounds.origin.x, bounds.origin.y + header_height), GPoint(bounds.origin.x + bounds.size.w, bounds.origin.y + header_height));
//     bounds = layer_get_bounds(console_layer);
//     graphics_fill_rect(ctx, GRect(0, 0, bounds.size.w, console_data->border_thickness), 0, GCornerNone);
//     graphics_fill_rect(ctx, GRect(0, 0, console_data->border_thickness, bounds.size.h), 0, GCornerNone);
//     graphics_fill_rect(ctx, GRect(bounds.size.w-console_data->border_thickness, 0, console_data->border_thickness, bounds.size.h), 0, GCornerNone);
//     graphics_fill_rect(ctx, GRect(0, bounds.size.h-console_data->border_thickness, bounds.size.w, console_data->border_thickness), 0, GCornerNone);

//   } // END Draw Border
// }










// ------------------------------------------------------------------------------------------------------------ //
// Create / Destroy Layer
// ------------------------------------------------------------------------------------------------------------ //

Layer* console_layer_create_with_size(GRect frame, int buffer_size) {
  Layer *console_layer;
  size_t data_size = sizeof (console_data_struct) + buffer_size;

  if((console_layer = layer_create_with_data(frame, data_size))) {
    console_data_struct *console_data = (console_data_struct*)layer_get_data(console_layer);
    console_data->buffer = (char*)(&(console_data->buffer) + sizeof(console_data->buffer));  // Point buffer to memory allocated just after the struct.  Assumes buffer is the last type inside struct.
    console_data->buffer_size = buffer_size;

    layer_set_clips(console_layer, true);
    console_layer_set_layer_style(console_layer, GColorBlack, GColorClear, fonts_get_system_font(FONT_KEY_GOTHIC_14), GTextAlignmentLeft, WordWrapFalse, true);
    console_layer_set_border_style(console_layer, true, GColorBlack, 3);
    console_layer_set_header_style(console_layer, true, GColorBlack, GColorLightGray, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), GTextAlignmentCenter);
    console_layer_set_header_text(console_layer, "Console");
    console_layer_clear(console_layer);
    layer_set_update_proc(console_layer, console_layer_update);
  }
  return console_layer;
}

Layer* console_layer_create(GRect frame) {
  return console_layer_create_with_size(frame, DEFAULT_BUFFER_SIZE);
}

// Converted these to PREPROCESSOR MACROS in the header
// void console_layer_destroy(Layer *console_layer) {
//   layer_destroy(console_layer);
// }

// void console_layer_safe_destroy(Layer *console_layer) {
//   if (console_layer) {
//     layer_destroy(console_layer);
//     console_layer = NULL;
//   }
// }


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
