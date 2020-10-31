

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


/* RADIO PINS + RADIO CHANNEL */

RF24 radio(9, 10); // CE, CSN         
const byte address[6] = "00001";     //Byte of array representing the address. This is the address where we will send the data. This should be same on the receiving side.


/* PINS DECLARATION */

const int trigPinL = 5;
const int echoPinL = 8;
const int trigPinR = 3;
const int echoPinR = 4;
const int crossPinR = 2;
const int crossPinL = 6;

/* CROSSING STATE VARIABLES */

boolean crossedL = 0;
boolean crossedR = 0;
boolean crossingL = 0;
boolean crossingR = 0;
float durationL, distanceL, durationR, distanceR;
unsigned long crossStartedL = 0;
unsigned long crossStartedR = 0;
unsigned long crossEndedL = 0;
unsigned long crossEndedR = 0;
unsigned long crossDurationL = 0;
unsigned long crossDurationR = 0;
unsigned long sinceLastCrossL = 0;
unsigned long sinceLastCrossR = 0;
unsigned long sinceCrossEndedL = 0;
unsigned long sinceCrossEndedR = 0;
boolean beenCrossedL = 0;
boolean beenCrossedR = 0;



/* SETTABLE PARAMETERS */

const int minDistance = 80;   // threshold for crossing
const int minReadTime = 100;  // minimum time for a single sensor detection to read a single crossing
const int maxWaitRead = 200;  // max time after which a cross has finished
const int maxTimeSpan = 1500; //  max time allowed to define a passage between sensors
boolean leftRight = 0; // set direction left to right, set to 0 for right to left




void setup() {
  
/* DISTANCE SENSORS SETUP */

pinMode(trigPinL, OUTPUT);
pinMode(echoPinL, INPUT);
pinMode(trigPinR, OUTPUT);
pinMode(echoPinR, INPUT);
pinMode(crossPinR, OUTPUT);
pinMode(crossPinL, OUTPUT);

/* RADIO TRANSMITTER SETUP */
radio.begin();                  //Starting the Wireless communication
radio.openWritingPipe(address); //Setting the address where we will send the data
radio.setPALevel(RF24_PA_MIN);  //You can set it as minimum or maximum depending on the distanceL between the transmitter and receiver.
radio.stopListening();          //This sets the module as transmitter
Serial.begin(9600);

}



void loop() {

  /* CALCULATE ULTRASOUND TIME OF RETURN LEFT */
  
  digitalWrite(trigPinL, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinL, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinL, LOW);
  durationL = pulseIn(echoPinL, HIGH);
  distanceL = (durationL * .0343) / 2;

  /* CALCULATE ULTRASOUND TIME OF RETURN RIGHT */
  
  digitalWrite(trigPinR, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinR, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinR, LOW);
  durationR = pulseIn(echoPinR, HIGH);
  distanceR = (durationR * .0343) / 2;
  
   Serial.print("L: ");
   Serial.print(distanceL);
   Serial.print(" R: ");
   Serial.print(distanceR);
   Serial.println();
  delay(10);


/* CROSSING SENSOR LEFT */

if (distanceL < minDistance)
    {
    // Serial.println("Left sensor crossed");
    crossedL = 1;
    }
else
    {
     crossedL = 0; 
    }


/* CROSS START LEFT */

if (crossedL == 1) {

  if (crossDurationL == 0) crossStartedL = millis() - 1;

  crossDurationL = millis() - crossStartedL;

  if (crossDurationL > minReadTime) {

      crossingL = 1;
      beenCrossedL = 1;
      
    }
  
}


/* CROSS END LEFT */

if (crossedL == 0 && crossingL == 1) {

  sinceLastCrossL = millis() - (crossStartedL + crossDurationL);
  
  if (sinceLastCrossL > maxWaitRead) {

    //Serial.print(" durata crossing L: ");
    //Serial.println(crossDurationL);

    crossEndedL = crossStartedL + crossDurationL;
    crossingL = 0;
    crossDurationL = 0;

  }
  
}


/* CROSSING STATE LEFT */

if ((beenCrossedL == 1) && (crossingL == 0)) {

  sinceCrossEndedL = millis() - crossEndedL;

  if (sinceCrossEndedL > maxTimeSpan) {
    
    beenCrossedL = 0;
    
  }
}


/////////////


/* CROSSING SENSOR RIGHT */

if (distanceR < minDistance)
    {
    // Serial.println("Left sensor crossed");
    crossedR = 1;
    }
else
    {
     crossedR = 0; 
    }


/* CROSS START RIGHT */

if (crossedR == 1) {

  if (crossDurationR == 0) crossStartedR = millis() - 1;

  crossDurationR = millis() - crossStartedR;

  if (crossDurationR > minReadTime) {

      crossingR = 1;
      beenCrossedR = 1;
      
    }
  
}


/* CROSS END RIGHT */

if (crossedR == 0 && crossingR == 1) {

  sinceLastCrossR = millis() - (crossStartedR + crossDurationR);
  
  if (sinceLastCrossR > maxWaitRead) {

    //Serial.print(" durata crossing R: ");
    //Serial.println(crossDurationR);

    crossEndedR = crossStartedR + crossDurationR;
    crossingR = 0;
    crossDurationR = 0;

  }
  
  
}

/* CROSSING STATE RIGHT */

if ((beenCrossedR == 1) && (crossingR == 0)) {

  sinceCrossEndedR = millis() - crossEndedR;

  if (sinceCrossEndedR > maxTimeSpan) {
    
    beenCrossedR = 0;
    
  }
}


digitalWrite(crossPinL, beenCrossedL);
digitalWrite(crossPinR, beenCrossedR);

////////

if (beenCrossedL == 1 && beenCrossedR == 1) {
  
  if (crossStartedL > crossStartedR) {

     Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println("crossed from right to left");
    if (leftRight == 0) 
        radio.write(&beenCrossedL, sizeof(beenCrossedL));  //Sending the sensor state to receiver 
    
  } 

  if (crossStartedR > crossStartedL) {

    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println("crossed from left to right");
    if (leftRight == 1) 
        radio.write(&beenCrossedR, sizeof(beenCrossedR));  //Sending the sensor state to receiver 
    
  }

  delay(100);
   beenCrossedL = 0;
    beenCrossedR = 0;

  boolean endSignal = 0;
  radio.write(&endSignal, sizeof(endSignal));  //Sending the sensor state to receiver 
  
}


}
