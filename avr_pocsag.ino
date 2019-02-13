
#include <Pocsag.h>
#include <TimerOne.h>

#define FSK_PIN 11

void setup() {
  Serial.begin(9600);
  pinMode(FSK_PIN, OUTPUT);
  initTimer();
}

int SPEED = 512;
bool invert = false;

Pocsag pocsag;
int state = 0;
int pointer = 0;


void loop() {
  // data
  long int address;
  int addresssource;
  int repeat;
  char textmsg[42]; // to store text message;
  int msgsize;

  // init var
  int rc;
  int state;
  address=0;
  addresssource=0;
  msgsize=0;


  while (state >= 0 and state < 8) {
  char c;
    // loop until we get a character from serial input
    while (!Serial.available()) {
    }; // end while
  
    c=Serial.read();
    if (state == 1) {
      if ((c >= '0') && (c <= '9')) {
        // digit -> first digit of address. Go to state 2 and process
        state=2;
        // continue to state 2 without fetching next char
      } else {
        state=-1;
        continue;
      }; // end else - if
    }

  // state 2: address ("0" to "9")
  if (state == 2) {
    if ((c >= '0') && (c <= '9')) {
      long int newaddress;

      newaddress = address * 10 + (c - '0');
      if (newaddress <= 0x1FFFFF) {
        // valid address
        address=newaddress;
      } else {
        // address to high.
        state=-2;
      }; // end else - if

    } else if (c == ' ') {
      // received space, go to next field (address source)
      state=3;
    } else {
      state=-3;
    }; 
    // get next char
    continue;
  }; // end state 2

  // state 3: address source: one single digit from 0 to 3
  if (state == 3) {
    if ((c >= '0') && (c <= '3')) {
      addresssource= c - '0';
      state=4;
    } else {
      state=-4;
    }; // end if
    continue;
  }; // end state 3
  
  if (state == 4) {
    if ((c >= '0') && (c <= '9')) {
      repeat=c - '0';

      // move to next state
      state=5;
    } else {
      state=-5;
    }; 
    continue;
  };

  if(state == 5) {
    rc = pocsag.CreatePocsag(address,addresssource,textmsg,0,1);
    if (!rc) {
      state = -6;
      delay(10000);
    } else {
      for (int l=-1; l < repeat; l++) {
        state = 6;
        while(state == 6) {
          delay(100);  
        }
        delay(3000);
      };
      state = 8;
    }
  }
  
  }
  Serial.write(state);
}

void initTimer() 
{
  Timer1.stop(); 
  long bitPeriod = (long) ((1.0f/SPEED) * 1000000); //micros
  Timer1.initialize(bitPeriod);         
  Timer1.attachInterrupt(sender);    
}

void sender() {
  if(state != 6)
    return;
  
}
