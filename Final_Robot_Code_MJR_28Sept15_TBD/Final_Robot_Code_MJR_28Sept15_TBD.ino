
//***********************************************************
//  EE40LX
//  Sketch Robot - Final
//
//  Marcelo Rovai
//  28Sept15
//************************************************************


// ******** Import IR library

#include "IRremote.h"

// *********Arduino Pin Mapping 

// IR receiver

const int receiver = 5; 

// Motor Drives

const int out1R = 6; 
const int out2R = 7; 
const int eneMR = 10; 
const int out1L = 2; 
const int out2L = 3; 
const int eneML = 9;

//PowerBlock & Photocells & LEDs
const int powerBlock = 8;
const int inPhotoR = 11;
const int inPhotoL = 12;
const int ledGreen = A2;
const int ledBlue = A1;
const int ledRed = A0;

// Buzzer
int outBuz = 4; 

// Microphone
int inMic = A4;  


// **********Definitions 

//IR Receiver
IRrecv irrecv(receiver);           
decode_results results;           

//Motor Drive
const int ON = 1;
const int OFF = 0;
const int RIGHT = 1;
const int LEFT = 0;
const int FW = 1;
const int BW = 0;
const int SLOW = 75;
const int AVERAGE = 125;
const int FAST = 250; 


//PowerBlock & Photocells 

//Buzzer

//Microphone
int micLevel = 0;
int micThreshold = 875;

//Others

int cont = 0;
int start = 0;
int reverse = 0;
int iniDir;


void setup() {

  Serial.begin(9600);
  
  //IR Receiver
  irrecv.enableIRIn(); // Start the receiver
  
  // Motor Drives
  pinMode(out1R, OUTPUT);
  pinMode(out2R, OUTPUT);
  pinMode(eneMR, OUTPUT);
  pinMode(out1L, OUTPUT);
  pinMode(out2L, OUTPUT);
  pinMode(eneML, OUTPUT);

  //PowerBlock & Photocells & LEDs
  pinMode(powerBlock, OUTPUT);
  pinMode(inPhotoR, INPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledBlue, OUTPUT);
  pinMode(ledRed, OUTPUT);
  
  //Buzzer
  pinMode(outBuz, OUTPUT);
 
  //*********Initial Settings
  
  digitalWrite(powerBlock, LOW); 
  
  digitalWrite(eneMR, LOW);
  digitalWrite(eneML, LOW);

}

void loop() {
  
  digitalWrite(ledGreen, LOW);
  digitalWrite(ledBlue, LOW);
  digitalWrite(ledRed, LOW);
  
  start = 0;
  reverse = 0;
  iniDir = FW;
  
  //turn off both motors
  motorCmdOff(LEFT);
  motorCmdOff(RIGHT);
  
  //Bip 4 times
  for(cont = 0; cont < 4; cont++){ 
    beep(outBuz, 1000, 100);
    delay(100);
  }
  
  // have we received an IR signal?
  do{
   if(irrecv.decode(&results)){ 
    translateIR(); 
    irrecv.resume();
    }
  } while (start == 0);
  
  start = 0;
  digitalWrite(ledBlue, HIGH);
  
  
  //Bip 2 times
  for(cont = 0; cont < 2; cont++){ 
    beep(outBuz, 1000, 100);
    delay(100);
    }
  
  //Read Mic Voltage Level and set up ambient level thershold		       
  Serial.println(micThreshold); // *****************SERIAL PRINT
  for(cont = 0; cont < 100; cont++){
    micLevel = analogRead(inMic);  
    if (micLevel > micThreshold){
     micThreshold = micLevel;
     delay(1);
     }
    } 
   
   // wait for sound
     do{
       micLevel = analogRead(inMic);
       delay(1);
     } while (micLevel < micThreshold);
     
     digitalWrite(ledBlue, LOW);
     digitalWrite(ledGreen, HIGH);
     motorCmdOn (RIGHT, FW, SLOW); 
     motorCmdOn (LEFT, FW, SLOW); 
    
     
     // Play Music
     for (cont=30; cont<100; cont+=1){
         beep(outBuz, 10*cont, 10);
     }
     for (cont=100; cont>30; cont-=1){
      beep(outBuz, 10*cont, 10);
     }
  
  // run motor FW until POWER button pressed or fhotocell blocked
  
  do{
    if(irrecv.decode(&results)){ // have we received an IR signal?
     irrecv.resume();
     translateIR(); 
    }
    checkLeftPhotocell();
    checkRightPhotocell();
    
    // If FW and Photocell Left blocked; goes BW and vice-versa
    motorCmdOn (RIGHT, iniDir, SLOW); 
    motorCmdOn (LEFT, iniDir, SLOW); 
 
  } while (start == 0);
  start = 0;                  
}


// Funtions

//Beep

void beep(int pin, int freq, long ms)   //square wave - freq ==> ms
{
  int i;
  long semiper = (long) (1000000/(freq*2));
  long loops = (long)((ms*1000)/(semiper*2));
  for (i=0;i<loops;i++){
    digitalWrite(pin, HIGH);  //set buzzer pin high
    delayMicroseconds(semiper);  //for half of the period
    digitalWrite(pin, LOW);   //set buzzer pin low
    delayMicroseconds(semiper);  //for the other half of the period
    }
}

// Motor
//  side ==> LEFT; RIGHT; dir ==> FWD; BWD; speedy ==> SLOW; AVERAGE; FAST

void motorCmdOn(int side, int dir, int speedy){ 
  if (side == 1){ //RIGHT Motor ON
    analogWrite(eneMR, speedy);
    if(dir == 1){        
      digitalWrite(out2R, HIGH); //FW
      digitalWrite(out1R, LOW);  //FW
    }  
    else{
      digitalWrite(out1R, HIGH); //BW
      digitalWrite(out2R, LOW);  //BW
    }
  }
  else{ //LEFT
    analogWrite(eneML, speedy);
    if(dir == 1){        
      digitalWrite(out2L, HIGH); //FW
      digitalWrite(out1L, LOW);  //FW
    }  
    else{
      digitalWrite(out1L, HIGH); //BW
      digitalWrite(out2L, LOW);  //BW
    }
  }
  }

void motorCmdOff(int side){ // side ==> LEFT; RIGHT;
  if (side == 1){ //RIGHT Motor
    analogWrite(eneMR, OFF);
    }
  else{ //LEFT
    analogWrite(eneML, OFF);
  }
}
 
 void checkLeftPhotocell()
 {
 
  digitalWrite(powerBlock, HIGH);   // decide if innitial direcion is FW or BW (no light) 
    delayMicroseconds(20);           
    if (digitalRead(inPhotoL) && iniDir){      
     digitalWrite(ledRed, LOW);
     digitalWrite(ledGreen, HIGH);
     iniDir = FW;
    }
    else{
      iniDir = BW;
      digitalWrite(ledGreen, LOW);
      digitalWrite(ledRed, HIGH);
    }
 }
 
 void checkRightPhotocell()  // If FW and Photocell Left blocked; goes BW and vice-versa
 {
 
  digitalWrite(powerBlock, HIGH);   // decide if innitial direcion is FW or BW (no light) 
    delayMicroseconds(20);           
    if (digitalRead(inPhotoR) && !iniDir){      
     digitalWrite(ledGreen,LOW);
     digitalWrite(ledRed, HIGH);
     iniDir = BW;
    }
    else{
      iniDir = FW;
      digitalWrite(ledRed, LOW);
      digitalWrite(ledGreen, HIGH);
    }
 }
 
 
// IR Receiver codes

void translateIR() // takes action based on IR code received 
{
   switch(results.value)
  {

  case 0xFFA25D:  
    Serial.println(" POWER            "); 
    start = 1;
    break;

  default: 
    Serial.println(" other button   ");
    //start = 1;  in case want any button start/stop robot

  } 
  
}
 
 
