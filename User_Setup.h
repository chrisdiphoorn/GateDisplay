#define USER_SETUP_LOADED 1 // Prevents other setup files from loading

#define ST7789_DRIVER       // Set a Dummmy Driver as Some Regsiters need to be set otherwise it will error.
#define Generic_Driver      // Forces TFT_eSPI into a pure memory buffer state

#define TFT_WIDTH  466      // Matches the exact dimensions of your AMOLED panel
#define TFT_HEIGHT 466

// Load the standard font libraries for text generation
#define LOAD_GLCD   
#define LOAD_FONT2  
#define LOAD_FONT4  
#define LOAD_FONT6  
#define LOAD_FONT7  
#define LOAD_FONT8  
#define LOAD_GFXFF  

#define SMOOTH_FONT
