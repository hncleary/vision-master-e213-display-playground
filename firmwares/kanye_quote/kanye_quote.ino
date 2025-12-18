/* Kanye Quote Display
 *
 * Function:
 * Fetches random Kanye West quotes from kanye.rest API and displays them on e-ink screen
 *
 * API: https://api.kanye.rest
 * Source: https://github.com/ajzbc/kanye.rest
 *
 * Description:
 * 1. Connects to WiFi network
 * 2. Fetches a random quote from the Kanye REST API
 * 3. Displays the quote on the e-ink display with word wrapping
 * 4. Refreshes the quote every 60 seconds
 */

// Fix for Heltec library compilation errors
// Include this header before any Heltec library headers
#include "heltec_fix.h"

#include "HT_lCMEN2R13EFC1.h"
#include "HT_E0213A367.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "images.h"  // Kanye head image

// WiFi credentials - loaded from secrets.h (gitignored)
// Copy secrets.h.example to secrets.h and fill in your credentials
// If secrets.h doesn't exist, uncomment and modify the lines below as a fallback
#include "secrets.h"
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// Fallback if secrets.h doesn't exist (uncomment and remove the lines above):
// const char* ssid = "YOUR_WIFI_SSID";
// const char* password = "YOUR_WIFI_PASSWORD";

// API endpoint
const char* kanyeApiUrl = "https://api.kanye.rest";

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

// Refresh interval: 60 seconds (60000 milliseconds)
const unsigned long REFRESH_INTERVAL = 60000;
unsigned long lastUpdateTime = 0;

void setup()
{
  Serial.begin(115200);
  delay(100);
  
  // Initialize display hardware
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

  // Write cmd to read chip ID
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

  // Read chip ID to determine display type
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
  
  // Initialize display based on chip ID
  if((chipId & 0x03) != 0x01)
  {
    display = new HT_ICMEN2R13EFC1(3, 2, 5, 1, 4, 6, -1, 6000000);
  } 
  else
  {
    display = new HT_E0213A367(3, 2, 5, 1, 4, 6, -1, 6000000);
  }
  
  // Set display dimensions based on rotation
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
  
  // Initialize display
  display->init();
  display->screenRotate(DIRECTION);
  display->setFont(ArialMT_Plain_10);
  
  // Show initial connecting message
  displayMessage("Connecting to WiFi...", "Please wait");
  delay(2000);
  
  // Connect to WiFi
  connectToWiFi();
  
  // Fetch and display first quote
  fetchAndDisplayQuote();
  
  lastUpdateTime = millis();
}

void VextON(void)
{
  pinMode(18, OUTPUT);
  digitalWrite(18, HIGH);
}

void VextOFF(void)
{
  pinMode(18, OUTPUT);
  digitalWrite(18, LOW);
}

void connectToWiFi()
{
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30)
  {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("\nWiFi connection failed!");
    displayMessage("WiFi Error", "Connection failed");
    // Continue anyway - might reconnect later
  }
}

String fetchQuote()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi not connected, attempting reconnect...");
    connectToWiFi();
    
    if (WiFi.status() != WL_CONNECTED)
    {
      return "WiFi not connected";
    }
  }
  
  HTTPClient http;
  
  Serial.print("Fetching quote from: ");
  Serial.println(kanyeApiUrl);
  
  http.begin(kanyeApiUrl);
  int httpCode = http.GET();
  
  if (httpCode > 0)
  {
    if (httpCode == HTTP_CODE_OK)
    {
      String payload = http.getString();
      Serial.println("Response: " + payload);
      
      // Parse JSON: {"quote":"text here"}
      // Find the quote field and extract the value
      int quoteStart = payload.indexOf("\"quote\"");
      if (quoteStart != -1)
      {
        int colonPos = payload.indexOf(':', quoteStart);
        int quoteBegin = payload.indexOf('"', colonPos) + 1;
        int quoteEnd = payload.indexOf('"', quoteBegin);
        
        if (quoteBegin > 0 && quoteEnd > quoteBegin)
        {
          String quote = payload.substring(quoteBegin, quoteEnd);
          // Unescape any escaped characters
          quote.replace("\\\"", "\"");
          quote.replace("\\n", "\n");
          quote.replace("\\r", "");
          
          http.end();
          return quote;
        }
      }
      
      http.end();
      return "Parse error";
    }
    else
    {
      Serial.printf("HTTP error code: %d\n", httpCode);
      http.end();
      return "HTTP Error: " + String(httpCode);
    }
  }
  else
  {
    Serial.printf("HTTP request failed: %s\n", http.errorToString(httpCode).c_str());
    http.end();
    return "Request failed";
  }
}

void displayMessage(String title, String message)
{
  display->clear();
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  
  display->drawString(0, 0, title);
  
  display->setFont(ArialMT_Plain_10);
  display->drawStringMaxWidth(0, 20, width, message);
  
  display->update(BLACK_BUFFER);
  display->update(COLOR_BUFFER);
  display->display();
}

void drawSpeechBubble(int x, int y, int w, int h, int pointerX, int pointerY)
{
  // Draw speech bubble outline
  // Main rectangle
  display->drawRect(x, y, w, h);
  
  // Draw pointer triangle pointing to the left (towards Kanye's head)
  // Triangle is drawn using lines
  int pointerSize = 8;
  display->drawLine(pointerX, pointerY, pointerX - pointerSize, pointerY - pointerSize/2);
  display->drawLine(pointerX, pointerY, pointerX - pointerSize, pointerY + pointerSize/2);
  display->drawLine(pointerX - pointerSize, pointerY - pointerSize/2, 
                    pointerX - pointerSize, pointerY + pointerSize/2);
}

void displayQuote(String quote)
{
  display->clear();
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  
  // Layout constants
  const int kanyeX = 5;           // X position of Kanye's head
  const int kanyeY = 5;           // Y position of Kanye's head
  const int bubbleX = 75;         // X position of speech bubble
  const int bubbleY = 10;         // Y position of speech bubble
  const int bubbleWidth = width - bubbleX - 5;  // Width of speech bubble
  const int bubbleHeight = height - bubbleY - 5; // Height of speech bubble
  const int textMargin = 5;       // Margin inside bubble for text
  const int pointerX = bubbleX;   // X position of pointer (left edge of bubble)
  const int pointerY = kanyeY + kanye_head_height / 2; // Y position of pointer (middle of head)
  
  // Draw Kanye's head image
  display->drawXbm(kanyeX, kanyeY, kanye_head_width, kanye_head_height, kanye_head_bits);
  
  // Draw "Kanye says" text below the head image
  // Position it centered below the 60-pixel wide head image
  int labelY = kanyeY + kanye_head_height + 3; // 3 pixels gap below head
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(kanyeX + kanye_head_width / 2, labelY, "Kanye says");
  display->setTextAlignment(TEXT_ALIGN_LEFT); // Reset to left alignment for quote text
  
  // Draw speech bubble
  drawSpeechBubble(bubbleX, bubbleY, bubbleWidth, bubbleHeight, pointerX, pointerY);
  
  // Draw the quote text inside the speech bubble
  // Position text with margins inside the bubble
  int textX = bubbleX + textMargin;
  int textY = bubbleY + textMargin;
  int textMaxWidth = bubbleWidth - (textMargin * 2);
  
  display->drawStringMaxWidth(textX, textY, textMaxWidth, quote);
  
  display->update(BLACK_BUFFER);
  display->update(COLOR_BUFFER);
  display->display();
}

void fetchAndDisplayQuote()
{
  Serial.println("Fetching new quote...");
  displayMessage("Loading...", "Fetching quote");
  
  String quote = fetchQuote();
  
  if (quote.length() > 0 && !quote.startsWith("Error") && !quote.startsWith("WiFi") && !quote.startsWith("HTTP") && !quote.startsWith("Request") && !quote.startsWith("Parse"))
  {
    displayQuote(quote);
    Serial.println("Quote displayed: " + quote);
  }
  else
  {
    displayMessage("Error", quote);
    Serial.println("Failed to fetch quote: " + quote);
  }
}

void loop()
{
  unsigned long currentTime = millis();
  
  // Check if it's time to refresh the quote
  if (currentTime - lastUpdateTime >= REFRESH_INTERVAL)
  {
    fetchAndDisplayQuote();
    lastUpdateTime = currentTime;
  }
  
  // Small delay to prevent tight loop
  delay(1000);
}

