#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128                // OLED display width, in pixels
#define SCREEN_HEIGHT 64                // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET    16                // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define the analog input pin for the photo-resistor
#define LUM_PIN 36
// Define the period for reading the photo-resistor (0.5 seconds)
#define LUM_PERIOD 500000
// Define the period for counter  (1 seconds)
#define COUNTER_PERIOD 1000000


#define MAX_WAIT_FOR_TIMER 2

// This function waits for a specified period of time
unsigned int waitFor(int timer, unsigned long period)
{
  static unsigned long waitForTimer[MAX_WAIT_FOR_TIMER];  // Timer array
  unsigned long newTime = micros() / period;              // Calculate current time
  int delta = newTime - waitForTimer[timer];              // Calculate time since last call
  if ( delta < 0 ) delta = 1 + newTime;                   // Handle overflow
  if ( delta ) waitForTimer[timer] = newTime;             // Update timer array
  return delta;
}

struct Counter 
{
  int value;
  int timer;
  unsigned long period;
}; 

void setup_Counter(struct Counter * ctr, unsigned long period)
{
  ctr -> value = 0;    
  ctr -> timer = 0;   
  ctr -> period = period;
}

void displayCounter(int Value)
{
  char str[3];  
  sprintf(str, "%02d", Value); 
  display.fillRect(0, 0, 32, 16, BLACK);
  display.setTextSize(2);     
  display.setTextColor(WHITE); 
  display.setCursor(0, 0);     
  display.print(str); 
  display.display(); 
}

void loop_Counter(struct Counter *ctr) 
{
  if (!(waitFor(ctr->timer,ctr->period))) return;         
  (ctr -> value) = (ctr -> value) < 60 ? (ctr -> value)+1 : 0;
  displayCounter(ctr -> value);                           
}

struct Lum 
{
  int value;
  int timer;
  unsigned long period;
};

void setup_Lum(struct Lum * lum, unsigned long period)
{
  lum -> value = 0;    
  lum -> timer = 1;   
  lum -> period = period;
}

int getLuminosity() 
{
    int lumValue = analogRead(LUM_PIN);
    return map(lumValue, 4095, 800, 0, 100);
    //Serial.println(lumValue);
    //Serial.println(lumPercent);
}

void displayLuminosity(int Value)
{
  char str[3];  
  sprintf(str, "%02d", Value); 
  display.fillRect((SCREEN_WIDTH - ((strlen(str)+1) * 12)) / 2, (SCREEN_HEIGHT - 16) / 2, 48, 16, BLACK);
  display.setTextSize(2);     
  display.setTextColor(WHITE); 
  display.setCursor((SCREEN_WIDTH - ((strlen(str)+1) * 12)) / 2, (SCREEN_HEIGHT - 16) / 2);  
  display.print(str+'%'); 
  display.display(); 
}

void loop_Lum(struct Lum * lum) 
{
  if (!(waitFor(lum->timer,lum->period))) return;
  displayLuminosity(getLuminosity());                               
}

enum {EMPTY, FULL};
struct mailbox {
  int state;
  int val;
};


struct Counter counter1;
struct Lum lum1;


struct mailbox luminosity = {.state = EMPTY};











void setup() {
  // Set up I2C communication on pins 4 and 15
  Wire.begin(4, 15);
  Serial.begin(9600);

  // Initialize the OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds
  display.clearDisplay();   // Clear the display buffer

  // Set up the counter with a period of 1 second (1,000,000 microseconds)
  setup_Counter(&counter1, COUNTER_PERIOD);
  setup_Lum(&lum1, LUM_PERIOD);
}

void loop() {
  // Increment the counter and update the display
  loop_Counter(&counter1);
  loop_Lum(&lum1);
}
