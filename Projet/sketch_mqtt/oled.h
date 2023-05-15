#ifndef OLED_H
#define OLED_H

// Define a structure to hold the data for the OLED display
struct Oled 
{
  int timer; // The timer index for the OLED display
  unsigned long period; // The period for updating the OLED display (in microseconds)
};

// This function sets up the Oled structure with a specified period
void setup_Oled(struct Oled * oled, unsigned long period)
{
  oled->timer = 2; // Set timer for Oled
  oled->period = period; // Set period for Oled
}

void displayAlert(int Value)
{
  char alert[9]="Alert:";
  char str[3];             // create a char array to hold the counter value as a string
  sprintf(str, "%02d", Value);  // convert the counter value to a string
  //display.fillRect(16, 32, 96, 16, BLACK);   // clear the OLED display
    display.clearDisplay();   // Clear the display buffer
  display.setTextSize(2);     // set the text size to 2
  display.setTextColor(WHITE); // set the text color to white
  display.setCursor(16, 32);     // set the cursor position to (0,0)
  strcat(alert, str);
  display.print(alert);          // print the counter value
  Serial.println(alert);
  display.display();           // update the OLED display
}


#endif
