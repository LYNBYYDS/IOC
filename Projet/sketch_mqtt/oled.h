#ifndef OLED_H
#define OLED_H

#include "buzzer.h"

// Define a structure to hold the data for the OLED display
struct Oled 
{
  int timer;                                // The timer index for the OLED display
  unsigned long period;                     // The period for updating the OLED display (in microseconds)
};

// This function sets up the Oled structure with a specified period
void setup_Oled(struct Oled *oled, unsigned long period)
{
  oled->timer = 2;                          // Set timer for OLED
  oled->period = period;                    // Set period for OLED
}

// This function displays an alert on the OLED display with a given value
void displayAlert(int Value)
{
  if (Value == 0){
    char affich[10] = "Alert Fin";
    display.clearDisplay();                 // Clear the display buffer
    display.setTextSize(2);                 // Set the text size to 2
    display.setTextColor(WHITE);            // Set the text color to white
    display.setCursor(16, 32);              // Set the cursor position
    display.print(affich);                  // Print the alert message with the value
    display.display();                      // Update the OLED display

  }else{
    char alert[9] = "Alert:";               // Alert message prefix
    char str[3];                            // Create a char array to hold the value as a string
    sprintf(str, "%02d", Value);            // Convert the value to a string
    display.clearDisplay();                 // Clear the display buffer
    display.setTextSize(2);                 // Set the text size to 2
    display.setTextColor(WHITE);            // Set the text color to white
    display.setCursor(16, 32);              // Set the cursor position
    strcat(alert, str);                     // Concatenate the alert message and value
    display.print(alert);                   // Print the alert message with the value
    Serial.println(alert);                  // Print the alert message with the value to the serial monitor
    display.display();                      // Update the OLED display 
  }
}

// This function displays logo
void displayLogo()
{
  char logo[5] = "LTSS";                    // Logo
  display.clearDisplay();                   // Clear the display buffer
  display.setTextSize(4);                   // Set the text size to 3
  display.setTextColor(WHITE);              // Set the text color to white
  display.setCursor(24, 20);                // Set the cursor position
  display.print(logo);                      // Print the logo
  display.display();                        // Update the OLED display 
}

void loop_Oled(struct Oled * oled, mailbox_buzzer *BuzzerMailbox, int *Start)
{
  // Wait for the timer to expire
  if (!(waitFor(oled->timer, oled->period))) return;
  if (*Start){
    displayAlert(BuzzerMailbox->second);
  }else {
    displayLogo();
  }
}

#endif
