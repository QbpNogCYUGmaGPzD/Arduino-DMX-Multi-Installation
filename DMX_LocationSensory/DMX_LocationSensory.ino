/* 
 *  Bibliotheek Gouda
 *
 *  Interactieve Weergave Data uit de Bibliotheek Gouda
 *
 *  Door Dennis Neumann
 */

// Speed Control Rotarie on     :Connector #1 (A0)
// Intensity Control Rotarie on :Connector #2 (A1)
// Infra Red Meter on           :Connector #3 (A2)
// Audio Analog Meter on        :Connector #4 (A3)

// And, also! put The AGC Mic on 3V!


//------------------LIBRARIES-----------------------//
#include <DmxMaster.h>

//==================CONSTANTS=======================//

//10 Lights: Channels for R,G,B
int pinArrayR[] = {
  1, 7, 13, 19, 25, 31, 37, 43, 49, 55};
int pinArrayG[] = {
  2, 8, 14, 20, 26, 32, 38, 44, 50, 56};
int pinArrayB[] = {
  3, 9, 15, 21, 27, 33, 39, 45, 51, 57};

//kanalen voor links en rechts
int pinArrayR_L[] = {
  1, 7, 13, 19, 25};
int pinArrayR_R[] = {
  31, 37, 43, 49, 55};
int pinArrayG_L[] = {
  2, 8, 14, 20, 26};
int pinArrayG_R[] = {
  32, 38, 44, 50, 56};
int pinArrayB_L[] = {
  3, 9, 15, 21, 27};
int pinArrayB_R[] = {
  33, 39, 45, 51, 57};  

//Intensiteiteswaarde per kleurkanaal
int Intens = 0;
//Counter voor kanalen
int chnl = 1;
//Waarde InfraRood Sensor RAW
int val = 0;
//SensorAfstandswaarde
int infraDist;
//Sensor Analog Pin
int infraPin = 2;

// Timer for scanners
long interval = 500;
long previousMillis = 0;

//Bezoekersteller
int VisiNumber = 0;



// Counter for the hand trigger
int handLoop = 1;

//Rotaries voor snelheid en intensiteit
int speedPin = 0;  
int intenPin = 1;  
#define NUMLIGHTS 11
int pins[NUMLIGHTS] = { 
  1, 7, 13, 19, 25, 31, 37, 43, 49, 55};

//Audio Measurements
const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;

//Intensity for VU Meter
float inten;

//==================END CONSTANTS=====================//

//------------------START WORK------------------------//

void setup() {
  DmxMaster.usePin(3);
  DmxMaster.maxChannel(512);

  //Serieel opzetten
  Serial.begin(9600);

  //Alles uit per lamp
  for (int install = 0; install < 255; install++) {
    DmxMaster.write(install, 0);
    delay(5);
  }
}  

//-----------------START LOOP-------------------------//

void loop(){

  unsigned long startMillis= millis();  // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level

  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;

  // collect data for 50 mS
  while (millis() - startMillis < sampleWindow)
  {
    sample = analogRead(3);
    if (sample < 1024)  // toss out spurious readings
    {
      if (sample > signalMax)
      {
        signalMax = sample;  // save just the max levels
      }
      else if (sample < signalMin)
      {
        signalMin = sample;  // save just the min levels
      }
    }
  }
  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
  float volts = (peakToPeak * 3.3) / 1024;  // convert to volts

  inten = mapfloat(volts, 0.0, 3.3, 0.0, 255.0);

  //functions!
  VUmeter(); //Display Sound Levels 
  Visitors(); //Display number of visitors horizontal
  scannerLoop(); //Enter the Void On Hand Detection!
  //knightRider();

  //Serial.println(inten); //See if anything Works, uncomment if necessary
}


//======= THE MAGIC MOTHERFUCKER!!! ===================//

void scannerLoop () { //Enter The Void on Hand Detection
  //Lees infrarood signaal
  val = analogRead(infraPin);
  val = ReadSens_and_Condition();
  int burst = 0; //Count number of blue-outs.

  if (handLoop == 1) {
    if (val<100) {
      while (burst <1) {
        //reset all to black
        for (int iii =0; iii <512; iii++) {
          DmxMaster.write(iii,0);
        }
        for(int slow = 0; slow < 255; slow++) {
          DmxMaster.write(4,slow);
          DmxMaster.write(10,slow);
          DmxMaster.write(16,slow);
          DmxMaster.write(22,slow);
          DmxMaster.write(28,slow);
          DmxMaster.write(34,slow);
          DmxMaster.write(40,slow);
          DmxMaster.write(46,slow);
          DmxMaster.write(52,slow);
          DmxMaster.write(58,slow);
          delay(30);
          burst++;
          handLoop = 1;
        }
        for (int slowDown = 255; slowDown >0; slowDown--) {
          DmxMaster.write(4,slowDown);
          DmxMaster.write(10,slowDown);
          DmxMaster.write(16,slowDown);
          DmxMaster.write(22,slowDown);
          DmxMaster.write(28,slowDown);
          DmxMaster.write(34,slowDown);
          DmxMaster.write(40,slowDown);
          DmxMaster.write(46,slowDown);
          DmxMaster.write(52,slowDown);
          DmxMaster.write(58,slowDown);
          delay(30);
        }
      }
      BackToBlack();
    }
  }
}


void knightRider () { // Scan Through All Lights
  const float pi = 3.14159;
  int brightness, intDegrees = 90;
  int count = 0;
  int burst = 0; //Count number of blue-outs.

  if (handLoop == 2) {
    if (val<100) {
      while (burst < 2) {
        for (count=0;count<10;count++) {
          brightness = 255 - abs(255 * sin(intDegrees * pi / 180));
          intDegrees++;
          if (intDegrees < 270)
            DmxMaster.write(pinArrayR[count], brightness);
          else if (intDegrees > 299)
            intDegrees = 90;
          delay(20);
        }
        for (count=0;count<10;count++) {
          brightness = 255 - abs(255 * sin(intDegrees * pi / 180));
          intDegrees++;
          if (intDegrees < 270)
            DmxMaster.write(pinArrayG[count], brightness);
          else if (intDegrees > 299)
            intDegrees = 90;
          delay(20);
        }
        for (count=0;count<10;count++) {
          brightness = 255 - abs(255 * sin(intDegrees * pi / 180));
          intDegrees++;
          if (intDegrees < 270)
            DmxMaster.write(pinArrayB[count], brightness);
          else if (intDegrees > 299)
            intDegrees = 90;
          delay(20);
        }
        break;          
      }
      burst++;
      handLoop = 1;      
      BackToBlack();
    }
  }
}

//Gemiddelde van Infra Sensor
int ReadSens_and_Condition(){
  int i;
  int sval = 0;

  for (i = 0; i < 5; i++){
    sval = sval + analogRead(2);    // sensor on analog pin ?
  }

  sval = sval / 5;    // average
  sval = sval / 4;    // scale to 8 bits (0 - 255)
  sval = 255 - sval;  // invert output
  return sval;
}

// Number of visitors through the day
void Visitors() {
  // Array of lights for Visitors Bar (purple)
  int visArray[] = {
    31, 33, 37, 39, 43, 45, 49, 51        };


  // Variable to store number of visitors
  VisiNumber = analogRead(0);
  VisiNumber = map (VisiNumber, 0, 1024, 0, 9);

  for (int xx = 0; xx < VisiNumber; xx+=2) {
    DmxMaster.write(visArray[xx],255);
    for (int yy =1; yy < VisiNumber; yy+=2) {
      DmxMaster.write(visArray[yy],50);      
    } 
  } 
  VisiNumber =0;
} 





//VU Meter On Sound Levels From Mic with AGC
// Quick and Dirty for Now.
void VUmeter() {

  if (inten < 50) { //Laagste stand
    DmxMaster.write(26,inten/2);
    DmxMaster.write(20,0);
    DmxMaster.write(14,0);
    DmxMaster.write(8,0);
    DmxMaster.write(7,0);
    DmxMaster.write(1,0);
  } 
  else if (inten < 60) {
    DmxMaster.write(26,inten);
    DmxMaster.write(20,inten/2);
    DmxMaster.write(14,inten/4);
    DmxMaster.write(8,0);
    DmxMaster.write(7,0);
    DmxMaster.write(1,0);
  } 
  else if (inten < 80) {
    DmxMaster.write(26,inten);
    DmxMaster.write(20,inten);
    DmxMaster.write(14,inten);
    DmxMaster.write(8,inten/3);
    DmxMaster.write(7,inten/2);
    DmxMaster.write(1,0);
  } 
  else if (inten < 100) {
    DmxMaster.write(26,inten);
    DmxMaster.write(20,inten);
    DmxMaster.write(14,inten);
    DmxMaster.write(8,inten/3);
    DmxMaster.write(7,inten/2);
    DmxMaster.write(1,inten/2);
  } 
  else if (inten < 120) { //hoogste stand
    DmxMaster.write(26,inten);
    DmxMaster.write(20,inten);
    DmxMaster.write(14,inten);
    DmxMaster.write(8,inten/3);
    DmxMaster.write(7,inten);
    DmxMaster.write(1,inten);
  }
}


//The Arduino Map function but for floats
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//Reset all lights back to black
void BackToBlack() {
  for (int reset =0; reset < 512; reset++) {
    for (int dimm = 255; dimm > 0; dimm--) {
      DmxMaster.write(reset,dimm);
    }
  }
}


























