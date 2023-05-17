#ifndef BUZZER_H
#define BUZZER_H

#include "config.h"

ToneESP32 buzzer_Tone(BUZZER_PIN, BUZZER_CHANNEL);

// Define a structure to hold the data for the buzzer mailbox
struct mailbox_buzzer
{
  int state;                            // Whether the mailbox is empty or full
  int second;                           // How long the buzzer buzzes, in seconds
};

// Define a structure to hold the data for the buzzer
struct Buzzer
{
  int timer;                            // Stores the timer number for buzzer tone generation
  int etat;                             // Stores the current state of the buzzer (ON or OFF), represented by 1 or 0
  unsigned long period;                 // The period for the buzzer, in milliseconds
};

// This function initializes a Buzzer struct with the given parameters
void setup_Buzzer(struct Buzzer *bz, unsigned long period)
{  
  bz->timer = 1;                        // Set the timer value for the buzzer
  bz->period = period;                  // Set the period for the buzzer
  bz->etat = 0;                         // Set the state of the buzzer to off
  pinMode(BUZZER_PIN, OUTPUT);          // Set the pin mode of the buzzer pin to output
}

// This function controls the behavior of the buzzer during each loop iteration
void loop_Buzzer(struct Buzzer *bz, mailbox_buzzer *BuzzerMailbox, int* Start) 
{
  if (!(waitFor(bz->timer, bz->period)))
    return;     // Wait for the appropriate amount of time before executing the loop
  if (BuzzerMailbox->state == FULL)     // Check if the mailbox containing button state information is full
  {
    Serial.println("Start beeping");
    
    if (BuzzerMailbox->second >= 1)
    {
      buzzer_Tone.tone(NOTE_A4, 1000);  // Activate the buzzer with frequency NOTE_A4 for 1s
      BuzzerMailbox->second -= 1;
    }else if (BuzzerMailbox->second == 0){
      BuzzerMailbox->state = EMPTY;     // Set the mailbox state to empty after the buzzer finishes buzzing
      *Start = 0;
    }
  }    
}

#endif
