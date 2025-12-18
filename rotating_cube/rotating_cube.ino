#include "HT_lCMEN2R13EFC1.h"
#include "HT_E0213A367.h"
#include <math.h>

#ifndef PI
#define PI 3.14159265358979323846
#endif

// Initialize the display
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

// Cube parameters
float cubeSize = 30.0;  // Size of the cube
float angleX = 0.0;     // Rotation angle around X axis
float angleY = 0.0;     // Rotation angle around Y axis
float angleZ = 0.0;     // Rotation angle around Z axis

// Cube vertices in 3D space (centered at origin)
float cubeVertices[8][3] = {
  {-1, -1, -1},  // 0: bottom-left-back
  { 1, -1, -1},  // 1: bottom-right-back
  { 1,  1, -1},  // 2: top-right-back
  {-1,  1, -1},  // 3: top-left-back
  {-1, -1,  1},  // 4: bottom-left-front
  { 1, -1,  1},  // 5: bottom-right-front
  { 1,  1,  1},  // 6: top-right-front
  {-1,  1,  1}   // 7: top-left-front
};

// Edges of the cube (pairs of vertex indices)
int cubeEdges[12][2] = {
  {0, 1}, {1, 2}, {2, 3}, {3, 0},  // Back face
  {4, 5}, {5, 6}, {6, 7}, {7, 4},  // Front face
  {0, 4}, {1, 5}, {2, 6}, {3, 7}   // Connecting edges
};

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
  
  // Reset e-ink
  digitalWrite(PIN_EINK_RES, LOW);
  delay(20);
  digitalWrite(PIN_EINK_RES, HIGH);
  delay(20);

  digitalWrite(PIN_EINK_DC, LOW);
  digitalWrite(PIN_EINK_CS, LOW);

  // Write cmd
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

  // Read chip ID
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
  if((chipId & 0x03) != 0x01)
  {
    display = new HT_ICMEN2R13EFC1(3, 2, 5, 1, 4, 6, -1, 6000000);
  } 
  else
  {
    display = new HT_E0213A367(3, 2, 5, 1, 4, 6, -1, 6000000);
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
  
  display->init();
  display->screenRotate(DIRECTION);
  display->setFont(ArialMT_Plain_10);
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

// Rotate point around X axis
void rotateX(float &y, float &z, float angle)
{
  float cos_a = cos(angle);
  float sin_a = sin(angle);
  float new_y = y * cos_a - z * sin_a;
  float new_z = y * sin_a + z * cos_a;
  y = new_y;
  z = new_z;
}

// Rotate point around Y axis
void rotateY(float &x, float &z, float angle)
{
  float cos_a = cos(angle);
  float sin_a = sin(angle);
  float new_x = x * cos_a + z * sin_a;
  float new_z = -x * sin_a + z * cos_a;
  x = new_x;
  z = new_z;
}

// Rotate point around Z axis
void rotateZ(float &x, float &y, float angle)
{
  float cos_a = cos(angle);
  float sin_a = sin(angle);
  float new_x = x * cos_a - y * sin_a;
  float new_y = x * sin_a + y * cos_a;
  x = new_x;
  y = new_y;
}

// Project 3D point to 2D screen coordinates (orthographic projection)
void project3D(float x, float y, float z, int &screenX, int &screenY)
{
  // Scale and center (using x and y, ignoring z for orthographic projection)
  screenX = (int)(width / 2 + x * cubeSize);
  screenY = (int)(height / 2 + y * cubeSize);
}

void drawCube()
{
  // Following the pattern from stick_figure: clear, draw, update buffers
  display->clear();
  
  // Rotate and project vertices
  float rotatedVertices[8][3];
  int projectedX[8], projectedY[8];
  
  for (int i = 0; i < 8; i++)
  {
    // Copy original vertex
    rotatedVertices[i][0] = cubeVertices[i][0];
    rotatedVertices[i][1] = cubeVertices[i][1];
    rotatedVertices[i][2] = cubeVertices[i][2];
    
    // Apply rotations
    rotateX(rotatedVertices[i][1], rotatedVertices[i][2], angleX);
    rotateY(rotatedVertices[i][0], rotatedVertices[i][2], angleY);
    rotateZ(rotatedVertices[i][0], rotatedVertices[i][1], angleZ);
    
    // Project to 2D
    project3D(rotatedVertices[i][0], rotatedVertices[i][1], rotatedVertices[i][2], 
              projectedX[i], projectedY[i]);
  }
  
  // Draw all edges
  for (int i = 0; i < 12; i++)
  {
    int v0 = cubeEdges[i][0];
    int v1 = cubeEdges[i][1];
    drawLine(projectedX[v0], projectedY[v0], projectedX[v1], projectedY[v1]);
  }
  
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
  // Update rotation angles
  angleX += 0.05;
  angleY += 0.03;
  angleZ += 0.02;
  
  // Keep angles in reasonable range to avoid overflow
  if (angleX > 2 * PI) angleX -= 2 * PI;
  if (angleY > 2 * PI) angleY -= 2 * PI;
  if (angleZ > 2 * PI) angleZ -= 2 * PI;
  
  // Draw the cube with new rotation
  drawCube();
  
  // E-ink displays are very slow to update - need longer delay to prevent flashing
  // The display() call itself takes time, so we wait longer to ensure it completes
  delay(1000);
}

