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
  scannerLoop(); //Enter the Void On Hand Detection!
  // Serial.println(inten); //See if anything Works, uncomment if necessary
}


//======= THE MAGIC MOTHERFUCKER!!! ===================//

void scannerLoop () { //Enter The Void on Hand Detection
  //Lees infrarood signaal
  val = analogRead(infraPin);
  val = ReadSens_and_Condition();
  while (val<100) {
    for(chnl = 0; chnl <= 4; chnl++) {
      for(Intens=0; Intens < 50; Intens++) {
        DmxMaster.write(pinArrayB_L[chnl],Intens);
        DmxMaster.write(pinArrayB_R[chnl],Intens);
        delay(5);
        DmxMaster.write(pinArrayG_L[chnl],Intens);
        DmxMaster.write(pinArrayG_R[chnl],Intens);
        delay(2);
        if (chnl>=4) {
          for(chnl = 0; chnl <=4; chnl++) {
            for(Intens =50; Intens >0; Intens--){
              DmxMaster.write(pinArrayB_L[chnl],Intens);
              DmxMaster.write(pinArrayB_R[chnl],Intens);
              delay(5);
              DmxMaster.write(pinArrayG_L[chnl],Intens);
              DmxMaster.write(pinArrayG_R[chnl],Intens);
              delay(2);
            }
          }
        } 
      }
    }
  } 
}

void knightRider () { // Scan Through All Lights
  static int pos = 0;       // the position of the brightest light in the light array
  static int direction = 1; // the direction the bright spot is travelling (1 or -1)
  int light;
  int speed = analogRead(speedPin);  // how fast the light moves
  int inten = analogRead(intenPin) >> 2;  // read the value and divide by 4 to get range 0 .. 255

  if (inten > 255) inten = 255;

  for (light=0 ; light < NUMLIGHTS ; light++) {
    if (light == pos) {  // The light at this position is set bright      
      DmxMaster.write(pins[light], inten);
    } 
    else if ( light == (pos+1) || light == (pos-0)) {
      // This makes the two lights adjacent to the bright one glow at reduced intensity.
      // It makes for a nicer effect
      DmxMaster.write(pins[light], inten>>7);
    } 
    else {
      // Digital I/O pins 5 & 6 don't seem to go dark if I do analogWrite(pins[light], 0)
      // By doing digitalWrite it all looks correct
      //DmxMaster.write(pins[light], 0);
    }    
  }

  // move the light position
  pos += direction;
  // if we've reached the end, reverse directions
  if (pos >= (NUMLIGHTS-1)) direction = -1;
  if (pos <= 0) direction = 1;
  delay(speed);
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

//VU Meter On Sound Levels From Mic with AGC
// Quick and Dirty for Now.
void VUmeter() {

  if (inten < 20) { //Laagste stand
    DmxMaster.write(26,inten/2);
    DmxMaster.write(20,0);
    DmxMaster.write(14,0);
    DmxMaster.write(8,0);
    DmxMaster.write(7,0);
    DmxMaster.write(1,0);
  } 
  else if (inten < 60) {
    DmxMaster.write(26,inten);
    DmxMaster.write(20,inten);
    DmxMaster.write(14,inten/3);
    DmxMaster.write(8,0);
    DmxMaster.write(7,0);
    DmxMaster.write(1,0);
  } 
  else if (inten < 100) {
    DmxMaster.write(26,inten);
    DmxMaster.write(20,inten);
    DmxMaster.write(14,inten);
    DmxMaster.write(8,inten/3);
    DmxMaster.write(7,inten/2);
    DmxMaster.write(1,0);
  } 
  else if (inten < 140) {
    DmxMaster.write(26,inten);
    DmxMaster.write(20,inten);
    DmxMaster.write(14,inten);
    DmxMaster.write(8,inten/3);
    DmxMaster.write(7,inten/2);
    DmxMaster.write(1,inten/2);
  } 
  else if (inten < 180) { //hoogste stand
    DmxMaster.write(26,inten);
    DmxMaster.write(20,inten);
    DmxMaster.write(14,inten);
    DmxMaster.write(8,inten);
    DmxMaster.write(7,inten);
    DmxMaster.write(1,inten);
  }
}


//The Arduino Map function but for floats
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}













