// The_Matrix_TTGO_GIF_animated
//
// Sketch which shows The Matrix animated GIF image in ESP32 TTGO T-Display stored in FLASH memory
//
// TFT_eSPI_memory written by Larry Bank
// bitbank@pobox.com
//
// Adapted by toponidream for the The_Matrix_TTGO_GIF_animated Arduino library:
// https://github.com/toponidream/TFT_eSPI
//
// To display a GIF from memory, a single callback function
// must be provided - GIFDRAW
// This function is called after each scan line is decoded
// and is passed the 8-bit pixels, RGB565 palette and info
// about how and where to display the line. The palette entries
// can be in little-endian or big-endian order; this is specified
// in the begin() method.
     
#define NORMAL_SPEED  

// Load GIF library
#include <AnimatedGIF.h>
AnimatedGIF gif;

#include "thematrix.h" // The Matrix image
                        
#define GIF_IMAGE thematrix   

#include <SPI.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);

  tft.begin();
#ifdef USE_DMA
  tft.initDMA();
#endif
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);

  gif.begin(BIG_ENDIAN_PIXELS);
}

#ifdef NORMAL_SPEED // Render at rate that is GIF controlled
void loop()
{
  // Put your main code here, to run repeatedly:
  if (gif.open((uint8_t *)GIF_IMAGE, sizeof(GIF_IMAGE), GIFDraw))
  {
    Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());
    tft.startWrite(); // The TFT chip select is locked low
    while (gif.playFrame(true, NULL))
    {
      yield();
    }
    gif.close();
    tft.endWrite(); // Release TFT chip select for other SPI devices
  }
}
#else // Test maximum rendering speed
void loop()
{
  long lTime = micros();
  int iFrames = 0;

  if (gif.open((uint8_t *)GIF_IMAGE, sizeof(GIF_IMAGE), GIFDraw))
  {
    tft.startWrite(); // For DMA the TFT chip select is locked low
    while (gif.playFrame(false, NULL))
    {
      // Each loop renders one frame
      iFrames++;
      yield();
    }
    gif.close();
    tft.endWrite(); // Release TFT chip select for other SPI devices
    lTime = micros() - lTime;
    Serial.print(iFrames / (lTime / 1000000.0));
    Serial.println(" fps");
  }
}
#endif
