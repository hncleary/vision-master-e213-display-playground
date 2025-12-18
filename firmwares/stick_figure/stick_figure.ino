
#include "HT_lCMEN2R13EFC1.h"
#include "HT_E0213A367.h"

// Initialize the display
// HT_ICMEN2R13EFC1 display(3, 2, 5, 1, 4, 6, -1, 6000000); // rst,dc,cs,busy,sck,mosi,miso,frequency
ScreenDisplay *display;

/* screen rotation
 * ANGLE_0_DEGREE
 * ANGLE_90_DEGREE
 * ANGLE_180_DEGREE
 * ANGLE_270_DEGREE
 */
#define DIRECTION ANGLE_0_DEGREE

int width, height;
#define PIN_EINK_SCLK 4
#define PIN_EINK_DC   2
#define PIN_EINK_CS   5
#define PIN_EINK_RES  3
#define PIN_EINK_MOSI 6
void setup()
{
    Serial.begin(115200);
    delay(100);
    VextON();
    delay(100);
    pinMode(PIN_EINK_SCLK, OUTPUT); 
    pinMode(PIN_EINK_DC, OUTPUT); 
    pinMode(PIN_EINK_CS, OUTPUT);
    pinMode(PIN_EINK_RES, OUTPUT);
    
    //rest e-ink
    digitalWrite(PIN_EINK_RES, LOW);
    delay(20);
    digitalWrite(PIN_EINK_RES, HIGH);
    delay(20);

    digitalWrite(PIN_EINK_DC, LOW);
    digitalWrite(PIN_EINK_CS, LOW);

    // write cmd
    uint8_t cmd = 0x2F;
    pinMode(PIN_EINK_MOSI, OUTPUT);  
    digitalWrite(PIN_EINK_SCLK, LOW);
    for (int i = 0; i < 8; i++)
    {
        digitalWrite(PIN_EINK_MOSI, (cmd & 0x80) ? HIGH : LOW);
        cmd <<= 1;
        digitalWrite(PIN_EINK_SCLK, HIGH);
        delayMicroseconds(1);
        digitalWrite(PIN_EINK_SCLK, LOW);
        delayMicroseconds(1);
    }
    delay(10);

    digitalWrite(PIN_EINK_DC, HIGH);
    pinMode(PIN_EINK_MOSI, INPUT_PULLUP); 

    // read chip ID
    uint8_t chipId = 0;
    for (int8_t b = 7; b >= 0; b--) 
    {
      digitalWrite(PIN_EINK_SCLK, LOW);  
      delayMicroseconds(1);
      digitalWrite(PIN_EINK_SCLK, HIGH);
      delayMicroseconds(1);
      if (digitalRead(PIN_EINK_MOSI)) chipId |= (1 << b);  
    }
    digitalWrite(PIN_EINK_CS, HIGH);
    if((chipId &0x03) !=0x01)
    {
      display = new HT_ICMEN2R13EFC1(3, 2, 5, 1, 4, 6, -1, 6000000); // rst,dc,cs,busy,sck,mosi,miso,frequency
    } 
    else
    {
      display = new HT_E0213A367(3, 2, 5, 1, 4, 6, -1, 6000000); // rst,dc,cs,busy,sck,mosi,miso,frequency
    }
  if (DIRECTION == ANGLE_0_DEGREE || DIRECTION == ANGLE_180_DEGREE)
  {
    width = 250;
    height = 122;
  }
  else
  {
    width = 122;
    height = 250;
  }
  // Initialising the UI will init the display too.
  display->init();
  display->screenRotate(DIRECTION);
  display->setFont(ArialMT_Plain_10);
  
  // Draw the stick figure once in setup
  drawStickFigure();
}

// Draw a line using Bresenham's algorithm with bounds checking
void drawLine(int x0, int y0, int x1, int y1)
{
  int dx = abs(x1 - x0);
  int dy = abs(y1 - y0);
  int sx = (x0 < x1) ? 1 : -1;
  int sy = (y0 < y1) ? 1 : -1;
  int err = dx - dy;
  int x = x0;
  int y = y0;
  int lastX = x0, lastY = y0;
  int maxSteps = dx + dy + 1;
  int stepCount = 0;

  while (stepCount < maxSteps)
  {
    // Only draw pixel if within bounds
    if (x >= 0 && x < width && y >= 0 && y < height)
    {
      display->setPixel(x, y);
    }
    
    if (x == x1 && y == y1) break;
    
    lastX = x;
    lastY = y;
    
    int e2 = 2 * err;
    if (e2 > -dy)
    {
      err -= dy;
      x += sx;
    }
    if (e2 < dx)
    {
      err += dx;
      y += sy;
    }
    
    stepCount++;
    
    // Prevent infinite loop
    if (x == lastX && y == lastY) break;
  }
}

// Helper function to plot circle points
void plotCirclePoint(int cx, int cy, int x, int y)
{
  if (cx + x >= 0 && cx + x < width && cy + y >= 0 && cy + y < height) display->setPixel(cx + x, cy + y);
  if (cx - x >= 0 && cx - x < width && cy + y >= 0 && cy + y < height) display->setPixel(cx - x, cy + y);
  if (cx + x >= 0 && cx + x < width && cy - y >= 0 && cy - y < height) display->setPixel(cx + x, cy - y);
  if (cx - x >= 0 && cx - x < width && cy - y >= 0 && cy - y < height) display->setPixel(cx - x, cy - y);
  if (cx + y >= 0 && cx + y < width && cy + x >= 0 && cy + x < height) display->setPixel(cx + y, cy + x);
  if (cx - y >= 0 && cx - y < width && cy + x >= 0 && cy + x < height) display->setPixel(cx - y, cy + x);
  if (cx + y >= 0 && cx + y < width && cy - x >= 0 && cy - x < height) display->setPixel(cx + y, cy - x);
  if (cx - y >= 0 && cx - y < width && cy - x >= 0 && cy - x < height) display->setPixel(cx - y, cy - x);
}

// Draw a circle using setPixel (midpoint circle algorithm)
void drawCirclePixels(int centerX, int centerY, int radius)
{
  int x = 0;
  int y = radius;
  int d = 1 - radius;
  
  plotCirclePoint(centerX, centerY, x, y);
  
  while (x < y)
  {
    if (d < 0)
    {
      d += 2 * x + 3;
    }
    else
    {
      d += 2 * (x - y) + 5;
      y--;
    }
    x++;
    plotCirclePoint(centerX, centerY, x, y);
  }
}

void drawStickFigure()
{
  // Following the pattern from drawRectDemo: clear, draw, update buffers
  display->clear();
  
  // Center position for the stick figure
  int centerX = width / 2;
  int startY = 20; // Top of the figure
  
  // Draw head (circle using setPixel)
  int headRadius = 8;
  int headY = startY + headRadius;
  drawCirclePixels(centerX, headY, headRadius);
  
  // Draw body (vertical line from head to torso bottom)
  int bodyStartY = headY + headRadius;
  int bodyLength = 35;
  int bodyEndY = bodyStartY + bodyLength;
  drawLine(centerX, bodyStartY, centerX, bodyEndY);
  
  // Draw arms (diagonal lines from shoulder)
  int shoulderY = bodyStartY + 10;
  int armLength = 20;
  // Left arm
  drawLine(centerX, shoulderY, centerX - armLength, shoulderY + armLength);
  // Right arm
  drawLine(centerX, shoulderY, centerX + armLength, shoulderY + armLength);
  
  // Draw legs (diagonal lines from hip)
  int hipY = bodyEndY;
  int legLength = 30;
  // Left leg
  drawLine(centerX, hipY, centerX - 15, hipY + legLength);
  // Right leg
  drawLine(centerX, hipY, centerX + 15, hipY + legLength);
  
  display->update(BLACK_BUFFER);
  display->update(COLOR_BUFFER);
  display->display();
}

void VextON(void)
{
  pinMode(18, OUTPUT);
  digitalWrite(18, HIGH);
}

void VextOFF(void) // Vext default OFF
{
  pinMode(18, OUTPUT);
  digitalWrite(18, LOW);
}

void loop()
{
  // E-ink holds the image, so no need to redraw constantly
  // Just keep the program running
  delay(1000);
} 