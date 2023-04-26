#ifndef BUZZER_H
#define BUZZER_H

#include "config.h"
#include "music.h"

// Define a structure to hold the data for the buzzer mailbox
struct mailbox_buzzer
{
  int state;                                // Whether the mailbox is empty or full
  int typemusic;                            // How long the buzzer buzzes
  int second;
};

// Define a structure to hold the data for the buzzer
struct Buzzer
{
  int timer;                                // stores the timer number for buzzer tone generation
  int etat;                                 // stores the current state of the buzzer (ON or OFF)1/0
  unsigned long period;
};

// This function initializes a Buzzer struct with the given parameters
void setup_Buzzer(struct Buzzer * bz, unsigned long period, int precent)
{  
  bz->timer = 1;                            // Set the timer value for the buzzer
  bz->period = period;                      // Set the period for the buzzer
  bz->etat = 0;                             // Set the state of the buzzer to off
  pinMode(BUZZER_PIN, OUTPUT);              // Set the pin mode of the buzzer pin to output
}

// This function controls the behavior of the buzzer during each loop iteration
void loop_Buzzer(struct Buzzer *bz, mailbox_buzzer *BuzzerMailbox) 
{
  if (!(waitFor(bz->timer,bz->period))) return;     // Wait for the appropriate amount of time before executing the loop
  if (BuzzerMailbox->state = FULL)                                  // Check if the mailbox containing button state information is full
  {
    if (bz->etat == 0)
    {
      beep(NOTE_A4, (BuzzerMailbox->second)*1000);
      BuzzerMailbox->state = EMPTY;
    }
  }}
#endif
