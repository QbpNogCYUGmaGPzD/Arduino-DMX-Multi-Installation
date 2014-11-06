#include <DmxMaster.h>

//selecteer alle relevante kanalen voor 10 lampen R G B
int pinArrayR[] = {
  1, 7, 13, 19, 25, 31, 37, 43, 49, 55};
int pinArrayG[] = {
  2, 8, 14, 20, 26, 32, 38, 44, 50, 56};
int pinArrayB[] = {
  3, 9, 15, 21, 27, 33, 39, 45, 51, 57};

void setup() {
  DmxMaster.usePin(3);
  DmxMaster.maxChannel(512);

  //Alles uit per lamp
  for (int install = 0; install < 255; install++) {
    DmxMaster.write(install, 0);
    delay(30);
  }
}


void loop() {
  //Tel iedere lamp af per 6 tot 56 voor rood
  for (int count=1; count<56; count+=6) {
    for (int counter = 0; counter <255; counter++) {
      DmxMaster.write(count, counter);
      delay(2);
    }
    delay(30);
  }
}




