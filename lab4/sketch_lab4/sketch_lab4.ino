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
#define BUTTON_PERIOD 20000
#define MAX_WAIT_FOR_TIMER 7

#define BUZZER_PIN 17
#define BUTTON_PIN 23
#define LUM_TABLE 120

char inputString;
bool stringComplete = false;

enum {EMPTY, FULL};
// Define a structure to hold the data for the luminosity mailbox
struct mailbox_lum 
{
  int state; // Whether the mailbox is empty or full
  int lumPercent; // The percentage of luminosity (0-100)
  int lumTable[LUM_TABLE]; // An array of past luminosity values
};

// Define a structure to hold the data for the LED mailbox
struct mailbox_led 
{
  int state; // Whether the mailbox is empty or full
  int period; // The period for flashing the LED (in microseconds)
};

// Define a structure to hold the data for the ISR mailbox
struct mailbox_isr 
{
  int state; // Whether the mailbox is empty or full
  int s_pressed; // Whether the S button has been pressed
};

// Define a structure to hold the data for the button mailbox
struct mailbox_button
{
  int state; // Whether the mailbox is empty or full
  int pressed; // Whether the button has been pressed
};

// Define a structure to hold the data for the counter
struct Counter 
{
  int timer; // The timer index for the counter
  unsigned long period; // The period for updating the counter (in microseconds)
  int value; // The current value of the counter (0-59)
}; 

// Define a structure to hold the data for the luminosity sensor
struct Lum 
{
  int timer; // The timer index for the luminosity sensor
  unsigned long period; // The period for reading the luminosity sensor (in microseconds)
};

// Define a structure to hold the data for the OLED display
struct Oled 
{
  int timer; // The timer index for the OLED display
  unsigned long period; // The period for updating the OLED display (in microseconds)
};

// Define a structure to hold the data for the LED
struct Led 
{
  int timer; // The timer index for the LED
  unsigned long period; // The period for flashing the LED (in microseconds)
  int etat; // The current state of the LED (on or off)
}; 

// Define a structure to hold the data for the terminal
struct Terminal
{
  int timer; // The timer index for the terminal
  unsigned long period; // The period for updating the terminal (in microseconds)
}; 

// Define a structure to hold the data for the button
struct Button
{
  int timer;               // stores the timer number for debounce logic
  unsigned long period;    // stores the debounce period
  int etat_now;            // current state of the button (HIGH or LOW)
  int etat_pre;            // previous state of the button (HIGH or LOW)
};

// Define a structure to hold the data for the buzzer
struct Buzzer
{
  int timer;               // stores the timer number for buzzer tone generation
  unsigned long period;    // stores the tone period
  int precent;             // stores the percentage of time the buzzer should be on in a period
  int etat;                // stores the current state of the buzzer (ON or OFF)
  int buzzing;             // flag to indicate if the buzzer is currently buzzing
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

void setup_Counter(struct Counter * ctr, unsigned long period)
{
  ctr->value = 0;          // initialize the counter value to 0
  ctr->timer = 0;          // set the timer number to 0
  ctr->period = period;    // set the period of the counter
}

void displayCounter(int Value)
{
  char str[3];             // create a char array to hold the counter value as a string
  sprintf(str, "%02d", Value);  // convert the counter value to a string
  display.fillRect(0, 0, 32, 16, BLACK);   // clear the OLED display
  display.setTextSize(2);     // set the text size to 2
  display.setTextColor(WHITE); // set the text color to white
  display.setCursor(0, 0);     // set the cursor position to (0,0)
  display.print(str);          // print the counter value
  display.display();           // update the OLED display
}

void loop_Counter(struct Counter *ctr) 
{
  if (!(waitFor(ctr->timer,ctr->period))) return;   // wait for the specified period of time
  (ctr->value) = (ctr->value) < 60 ? (ctr->value)+1 : 0;  // increment the counter value and wrap around after 60
  displayCounter(ctr->value);   // display the updated counter value on the OLED display
}

void setup_Lum(struct Lum * lum, unsigned long period, mailbox_lum * luminosity)
{
  lum->timer = 1;   // set the timer number to 1
  lum->period = period;    // set the period of the lum sensor
  for(int i = 0; i < LUM_TABLE; i++)    // initialize the lum table with a default value of 57
  {
    luminosity->lumTable[i] = 57;
  }
}

// Function to read the value of the photoresistor and convert it to a percentage
int getLuminosity() 
{
    int lumValue = analogRead(LUM_PIN); // Read the analog value from the photoresistor
    int lumPercent = map(lumValue, 4095, 800, 0, 100); // Convert the value to a percentage
    lumPercent = lumPercent > 99 ? 99 : lumPercent; // Limit the maximum percentage to 99
    return lumPercent; // Return the calculated percentage
}

// Function to display the luminosity percentage on the OLED screen
void displayLuminosity(int Value)
{
  char str[3];  
  sprintf(str, "%02d", Value); // Convert the percentage to a 2-digit string with leading zeros
  display.fillRect((SCREEN_WIDTH - 12*3), 0, 48, 16, BLACK); // Clear the previous display
  display.setTextSize(2);     
  display.setTextColor(WHITE); 
  display.setCursor((SCREEN_WIDTH - 12*3), 0);  
  display.print(str); // Print the luminosity percentage
  display.print('%');
  display.display(); 
}

// Function to add a new value to the luminosity table and shift the existing values
int addPixel(int lumTable, mailbox_lum * luminosity)
{
  for(int i = 0; i < LUM_TABLE-1; i++)
  {
    luminosity->lumTable[i] = luminosity->lumTable[i+1]; // Shift the existing values
  }
  luminosity->lumTable[LUM_TABLE-1] = 64-lumTable/3-7; // Calculate the new value and add it to the table
}

// Function to update the luminosity and LED mailboxes
void loop_Lum(struct Lum * lum, mailbox_lum * luminosity, mailbox_led * LedMailbox) 
{
  if (!(waitFor(lum->timer,lum->period))) return; // Wait for the specified period
  int lumPercent = getLuminosity(); // Get the current luminosity percentage
  if (luminosity->state == EMPTY) // Check if the luminosity mailbox is empty
  {
    addPixel(lumPercent, luminosity); // Add the new value to the luminosity table
    luminosity->lumPercent = lumPercent; // Update the current luminosity percentage
    luminosity->state = FULL; // Set the luminosity mailbox state to full
  }
  if (LedMailbox->state == EMPTY) // Check if the LED mailbox is empty
  {
    LedMailbox->period = lumPercent > 15 ? lumPercent*10000 : 150000; // Calculate the LED blink period based on the luminosity percentage
    LedMailbox->state = FULL; // Set the LED mailbox state to full
  }
}

// Function to display the luminosity table on the OLED screen
void displayLuminositytable(int * lumTable)
{
  display.fillRect(0, 19, 128, 44, BLACK); // Clear the previous display
  display.drawLine(0, 57, 128, 57, WHITE); // Draw a horizontal line
  for(int i = 0; i < LUM_TABLE-1; i++)
  {
    display.drawLine(i+4, lumTable[i], i+5, lumTable[i+1], WHITE); // Draw a line between each pair of adjacent values in the luminosity table
  }
  display.display(); 
}

// This function sets up the Oled structure with a specified period
void setup_Oled(struct Oled * oled, unsigned long period)
{
  oled->timer = 2; // Set timer for Oled
  oled->period = period; // Set period for Oled
}

// This function loops through the Oled structure and displays luminosity if it is full
void loop_Oled(struct Oled * oled, struct mailbox_lum * luminosity)
{
  // Wait for the timer to expire
  if (!(waitFor(oled->timer, oled->period))) return;

  // If the mailbox_lum state is not FULL, return
  if (luminosity->state != FULL) return;

  // Display the luminosity percentage and table
  displayLuminosity(luminosity->lumPercent);
  displayLuminositytable(luminosity->lumTable);

  // Set the mailbox_lum state to EMPTY
  luminosity->state = EMPTY;
}

// This function sets up the Led structure with default values and initializes the LED pin
void setup_Led(struct Led * ctx) 
{
  ctx->timer = 3; // Set timer for LED
  ctx->etat = 0; // Set initial state of LED
  ctx->period = 100000; // Set default period for LED
  pinMode(LED_BUILTIN, OUTPUT); // Initialize the LED pin
  digitalWrite(LED_BUILTIN, ctx->etat); // Set the initial state of the LED pin
}

// This function loops through the Led structure and toggles the LED state based on the state of the IsrMailbox and LedMailbox
void loop_Led(struct Led * led, mailbox_led * LedMailbox, mailbox_isr * IsrMailbox) 
{
  // Wait for the timer to expire
  if (!waitFor(led->timer, led->period)) return;

  // If the IsrMailbox state is FULL and the s_pressed flag is set, turn off the LED and set the IsrMailbox state to EMPTY
  if ((IsrMailbox->state == FULL) && (IsrMailbox->s_pressed == 1))
  {
    led->etat = 0;
    digitalWrite(LED_BUILTIN,led->etat);
    IsrMailbox->state == EMPTY;
  }
  // If the s_pressed flag is not set, check the LedMailbox for a new period and toggle the LED state
  else if (IsrMailbox->s_pressed == 0)
  {
    if (LedMailbox->state == FULL)
    {
      led->period = LedMailbox->period;
      LedMailbox->state = EMPTY;
    }
    digitalWrite(LED_BUILTIN,led->etat);
    led->etat = 1 - led->etat;
  }
}

// This function sets up the Terminal structure with a specified period
void setup_Terminal(struct Terminal * terminal, unsigned long period)
{
  terminal->timer = 4; // Set timer for Terminal
  terminal->period = period; // Set period for Terminal
}

// This function is responsible for handling terminal input and updating the mailbox accordingly.
// It checks if enough time has passed since the last update and if a complete string has been received.
void loop_Terminal(struct Terminal * terminal, mailbox_isr * IsrMailbox) {
  if (!waitFor(terminal->timer, terminal->period)) return; // Wait for the specified period of time
  if (!stringComplete) return; // If a complete string hasn't been received, return
  if (inputString == 's') // If the input string is 's'
  {
    if (IsrMailbox->state == EMPTY) // If the mailbox is empty
    {
      IsrMailbox->s_pressed = 1; // Set s_pressed to 1
      IsrMailbox->state = FULL; // Set the mailbox state to FULL
    }
  }
  inputString = ' '; // Reset the input string
  stringComplete = false; // Reset the string completion flag
}

// This function is called whenever a new serial event occurs (i.e., data is received over the serial port).
void serialEvent() {
  while (Serial.available()) { // As long as there's data available on the serial port
    char inChar = (char)Serial.read(); // Read the data into inChar
    if (inChar == '\n') { // If a new line character is received
      stringComplete = true; // Set the string completion flag to true
    }else{ // Otherwise
      inputString = inChar; // Set the input string to the received character
      //Serial.println(inChar); // Uncomment this line to print the received character to the serial port
    }
  }
}

// This function initializes the button struct.
void setup_Button(struct Button * bp, unsigned long period)
{  
  bp->timer = 5;   // Set the timer for this button
  bp->period = period; // Set the period for this button
  bp->etat_now = 0;  // Set the current button state to 0
  bp->etat_pre = 0; // Set the previous button state to 0
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Set the button pin mode to INPUT_PULLUP
}

// This function is responsible for handling button input and updating the mailbox accordingly.
void loop_Button(struct Button *bp, mailbox_button *BottonMailbox) 
{
  if (!(waitFor(bp->timer,bp->period))) return; // Wait for the specified period of time
  bp->etat_pre = bp->etat_now; // Set the previous button state to the current state
  bp->etat_now = digitalRead(BUTTON_PIN); // Read the current button state
  if ((BottonMailbox->state == EMPTY) && (bp->etat_now != bp->etat_pre)) // If the mailbox is empty and the button state has changed
  {
    if (bp -> etat_now == 0) // If the button is pressed
    {
      BottonMailbox->pressed = 1; // Set the pressed flag to 1
    }
    else // Otherwise
    {
      BottonMailbox->pressed = 0; // Set the pressed flag to 0
    }
    BottonMailbox->state = FULL; // Set the mailbox state to FULL
  }
  //Serial.println(buttonState);   // Uncomment this line to print the button state to the serial port
}

// This function initializes a Buzzer struct with the given parameters
void setup_Buzzer(struct Buzzer * bz, unsigned long period, int precent)
{  
  bz->timer = 6;   // Set the timer value for the buzzer
  bz->period = period;  // Set the period for the buzzer
  bz->precent = precent;  // Set the percentage of time the buzzer will be on
  bz->etat = 0;  // Set the state of the buzzer to off
  bz->buzzing = 0;  // Set the buzzing state of the buzzer to off
  pinMode(BUZZER_PIN, OUTPUT);  // Set the pin mode of the buzzer pin to output
}

// This function controls the behavior of the buzzer during each loop iteration
void loop_Buzzer(struct Buzzer *bz, mailbox_button *BottonMailbox) 
{
  if (!(waitFor(bz->timer,bz->period*bz->precent/100))) return;  // Wait for the appropriate amount of time before executing the loop
  if (BottonMailbox->state = FULL)  // Check if the mailbox containing button state information is full
  {
    bz->etat = BottonMailbox->pressed;  // Set the state of the buzzer based on the button press information in the mailbox
    BottonMailbox->state = EMPTY;  // Empty the mailbox containing button press information
  }
  if (bz->etat)  // If the buzzer is turned on
  {
    bz->buzzing = 1 - bz->buzzing;  // Toggle the buzzing state of the buzzer
    bz->precent = bz->buzzing ? 100 - bz->precent>bz->precent? 100 - bz->precent : bz->precent : 100 - bz->precent>bz->precent? bz->precent : 100 - bz->precent;  // Adjust the percentage of time the buzzer will be on based on the buzzing state
  }
  else  // If the buzzer is turned off
  {
    bz->buzzing = 0;  // Set the buzzing state of the buzzer to off
  }
  digitalWrite(BUZZER_PIN, bz->buzzing);  // Set the buzzer pin output based on the buzzing state of the buzzer
}

struct Counter counter1;
struct Lum lum1;
struct Oled oled1;
struct Led led1;
struct Terminal terminal1;
struct Button button1;
struct Buzzer buzzer1;
struct mailbox_lum luminosity = {.state = EMPTY};
struct mailbox_led LedMailbox = {.state = EMPTY};
struct mailbox_isr IsrMailbox = {.state = EMPTY};
struct mailbox_button BottonMailbox = {.state = EMPTY};

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
  setup_Oled(&oled1, LUM_PERIOD/2);
  setup_Led(&led1);
  setup_Terminal(&terminal1, 20000);
  setup_Button(&button1, BUTTON_PERIOD);
  setup_Buzzer(&buzzer1, 100000, 60);
  //pinMode(2, INPUT_PULLUP); why with this line it not work i dont know 
  attachInterrupt(digitalPinToInterrupt(2), serialEvent, FALLING);
}

void loop() {
  // Increment the counter and update the display
  loop_Counter(&counter1);
  loop_Lum(&lum1, &luminosity, &LedMailbox);
  loop_Oled(&oled1, &luminosity);
  loop_Led(&led1, &LedMailbox, &IsrMailbox);
  loop_Terminal(&terminal1, &IsrMailbox);
  loop_Button(&button1, &BottonMailbox);
  loop_Buzzer(&buzzer1, &BottonMailbox);
}
