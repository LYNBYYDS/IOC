#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128                // OLED display width, in pixels
#define SCREEN_HEIGHT 64                // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET    16                // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define MAX_WAIT_FOR_TIMER 2

unsigned int waitFor(int timer, unsigned long period)
{
  static unsigned long waitForTimer[MAX_WAIT_FOR_TIMER];  // 
  unsigned long newTime = micros() / period;              // 
  int delta = newTime - waitForTimer[timer];              // 
  if ( delta < 0 ) delta = 1 + newTime;                   // 
  if ( delta ) waitForTimer[timer] = newTime;             // 
  return delta;
}

struct Counter 
{
  int value;                                              //
  int timer;                                              //
  unsigned long period;                                   // 
}; 

void setup_Counter(struct Counter * ctr, unsigned long period)
{
  ctr -> value = 0;
  ctr -> timer = 0;
  ctr -> period = period;
}

void loop_Counter(struct Counter *ctr) 
{
  if (!(waitFor(ctr->timer,ctr->period))) return;         // 
  (ctr -> value)++;                                       // 
}



struct Counter counter1;

void setup() {
  Wire.begin(4, 15);
  
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  setup_Counter(&counter1, 1000000);

}

// display.drawLine(0, display.height()/2, display.width()-1, display.height()/2, WHITE);

void loop() {
  loop_Counter(&counter1);
  
  drawchar(counter1.value);
}

void drawchar(int value) {
  display.clearDisplay();
  
  int value0 = value%10+48;
  display.setTextSize(5);     
  display.setTextColor(WHITE); 
  display.setCursor(0, 0);     
  display.write(value0);
  display.display();
}
