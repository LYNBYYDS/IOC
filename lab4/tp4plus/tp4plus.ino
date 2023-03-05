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

#define MAX_WAIT_FOR_TIMER 3

#define BUZZER_PIN 17
#define BUTTON_PIN 23
#define LUM_TABLE 120

char inputString;
bool stringComplete = false;

enum {EMPTY, FULL};
struct mailbox_lum {
  int state;
  int lumPercent[LUM_TABLE];
};

struct mailbox_led {
  int state;
  int period;
};

struct mailbox_isr {
  int state;
  int s_pressed;
};

struct Counter 
{
  int value;
  int timer;
  unsigned long period;
}; 

struct Lum 
{
  int value;
  int timer;
  unsigned long period;
};

struct Led 
{
  int timer;
  int period;
  int etat;
}; 

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

void setup_Led( struct Led * ctx) 
{
  ctx->timer = 2;
  ctx->etat = 0;
  ctx->period = 100000;
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, ctx->etat);
}

void loop_Led( struct Led * ctx, mailbox_led * LedMailbox, mailbox_isr * IsrMailbox) 
{
  if (!waitFor(ctx->timer, ctx->period)) return;
  if ((IsrMailbox->state == FULL) && (IsrMailbox->s_pressed == 1))
  {
    ctx->etat = 0;
    digitalWrite(LED_BUILTIN,ctx->etat);
    IsrMailbox->state == EMPTY;
  }
  else if (IsrMailbox->s_pressed == 0)
  {
    if (LedMailbox->state == FULL)
    {
      ctx->period = LedMailbox->period;
      LedMailbox->state = EMPTY;
    }
    digitalWrite(LED_BUILTIN,ctx->etat);
    ctx->etat = 1 - ctx->etat;
  }
}

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

void setup_Lum(struct Lum * lum, unsigned long period, mailbox_lum * luminosity)
{
  lum -> value = 0;    
  lum -> timer = 1;   
  lum -> period = period;
  for(int i = 0; i < LUM_TABLE; i++)
  {
    luminosity->lumPercent[i] = 57;
  }
}

int getLuminosity() 
{
    int lumValue = analogRead(LUM_PIN);
    int lumPercent = map(lumValue, 4095, 800, 0, 100);
    lumPercent = lumPercent > 99 ? 99 : lumPercent;
    return lumPercent;
    //Serial.println(lumValue);
    //Serial.println(lumPercent);
}

void displayLuminosity(int Value)
{
  char str[3];  
  sprintf(str, "%02d", Value); 
  //Serial.println(str);
  display.fillRect((SCREEN_WIDTH - ((strlen(str)+1) * 12)) / 2, (SCREEN_HEIGHT - 16) / 2, 48, 16, BLACK);
  display.setTextSize(2);     
  display.setTextColor(WHITE); 
  display.setCursor((SCREEN_WIDTH - ((strlen(str)+1) * 12)) / 2, (SCREEN_HEIGHT - 16) / 2);  
  display.print(str);
  display.print('%');
  display.display(); 
}

int addPixel(int lumPercent, mailbox_lum * luminosity)
{
  for(int i = 0; i < LUM_TABLE-1; i++)
  {
    luminosity->lumPercent[i] = luminosity->lumPercent[i+1];
    Serial.print(luminosity->lumPercent[i]);
    Serial.print(' ');
  }
  luminosity->lumPercent[LUM_TABLE-1] = 64-lumPercent/3-7;
  Serial.println(' ');
}

void loop_Lum(struct Lum * lum, mailbox_lum * luminosity, mailbox_led * LedMailbox) 
{
  if (!(waitFor(lum->timer,lum->period))) return;
  int lumPercent = getLuminosity();
  if (luminosity->state == EMPTY)
  {
    addPixel(lumPercent, luminosity);
    luminosity->state = FULL;
  }
  if (LedMailbox->state == EMPTY)
  {
    LedMailbox->period = lumPercent > 15 ? lumPercent*10000 : 150000;
    LedMailbox->state = FULL;
  }
  //displayLuminosity(lumPercent);                               
}

void displayLuminositytable(int * lumPercent)
{
  display.fillRect(0, 19, 128, 34, BLACK);
  display.drawLine(0, 57, 128, 57, WHITE);
  //for(int i = 0; i < LUM_TABLE; i++)
  //{
  //  display.drawPixel(i+4, lumPercent[i], WHITE);
  //}
  for(int i = 0; i < LUM_TABLE-1; i++)
  {
    display.drawLine(i+4, lumPercent[i], i+5, lumPercent[i+1], WHITE);
  }
  display.display(); 
}

void loop_oled(struct mailbox_lum * luminosity)
{
   if (luminosity->state != FULL) return;
   displayLuminositytable(luminosity->lumPercent);
   luminosity->state = EMPTY;
}


void loop_terminal(mailbox_isr * IsrMailbox) {
  if (!stringComplete) return;
  if (inputString == 's')
  {
    if (IsrMailbox->state == EMPTY)
    {
      IsrMailbox->s_pressed = 1;
      IsrMailbox->state = FULL;
    }
  }
  inputString = ' ';
  stringComplete = false;
}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      stringComplete = true;
    }else{
      inputString = inChar;
      //Serial.println(inChar);
    }
  }
}

struct Counter counter1;
struct Lum lum1;
struct Led led1;
struct mailbox_lum luminosity = {.state = EMPTY};
struct mailbox_led LedMailbox = {.state = EMPTY};
struct mailbox_isr IsrMailbox = {.state = EMPTY};

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
  setup_Lum(&lum1, LUM_PERIOD, &luminosity);
  setup_Led(&led1);
  //pinMode(2, INPUT_PULLUP); why with this line it not work i dont know 
  attachInterrupt(digitalPinToInterrupt(2), serialEvent, FALLING);
}

void loop() {
  // Increment the counter and update the display
  loop_Counter(&counter1);
  loop_Lum(&lum1, &luminosity, &LedMailbox);
  loop_oled(&luminosity);
  loop_Led(&led1, &LedMailbox, &IsrMailbox);
  loop_terminal(&IsrMailbox);
}
