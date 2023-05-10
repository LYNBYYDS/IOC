
#include "config.h"
#include "buzzer.h"

//-------------------------
// ------ Parametre ------
//-------------------------

#define captor_id 1

struct Buzzer buzzer1;
struct mailbox_buzzer buzzer_mailbox = {.state = EMPTY};

void setup() {
    setup_Buzzer(&buzzer1, 1000000);              // 0.5s
}

void loop() {
  Serial.setTimeout(2000);
  delay (10000);
  buzzer_mailbox.state = FULL;
  buzzer_mailbox.second = 2;
  
  delay (10000);
  loop_Buzzer(&buzzer1, &buzzer_mailbox);
}
