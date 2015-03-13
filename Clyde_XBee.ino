#include <Wire.h>
#include <EEPROM.h>
#include <Clyde.h>
#include <SerialCommand.h>
#include <SoftwareSerial.h>
#include <MPR121.h>
#define FIRMWARE_VERSION 1

SerialCommand sCmd(Serial1);

void setup() {
  Wire.begin();
  
  Serial1.begin(9600);
  
  sCmd.addCommand("VERSION", cmdVersion);
  sCmd.addCommand("SET_AMBIENT", cmdSetAmbient);
  sCmd.addCommand("SET_WHITE", cmdSetWhite);
  sCmd.addCommand("BLINK", cmdBlink);
  sCmd.addCommand("BLINK_AMBIENT", cmdBlinkAmbient);
  sCmd.addCommand("OFF", cmdOff);
  sCmd.setDefaultHandler(unrecognized);
  
  //Clyde.eeprom()->reset();
  Clyde.begin();
  Serial1.write("Clyde is Ready!"); 
}

void loop() {
  //read the serial communication if any
  sCmd.readSerial();
  
  //calibrate the eye and check for press
  Clyde.updateEye();
  
  //update the mouth to play sounds
  Clyde.updateMouth();
  
  //update the lights
  Clyde.updateAmbientLight();
  Clyde.updateWhiteLight();
  
  //make Clyde behave after the eye was calibrated once
  if (Clyde.wasEyeCalibratedOnce())
    Clyde.updatePersonalities();
}

void cmdBlink()
{           
    for (int i=0; i <= 255; i += 255) {
    Clyde.setWhite(i);  // setting the white light to 255 turns it OFF.
    Clyde.updateWhiteLight();
    delay(500);  
    }
}

void cmdBlinkAmbient()
{
 char *param1, *param2, *param3, *param4;
  int numBlinks, r, g, b;
  
  //Get arguments
  param1 = sCmd.next();    // Number of times to blinks
  param2 = sCmd.next();    // Red
  param3 = sCmd.next();    // Green
  param4 = sCmd.next();    // Blue
  numBlinks = atoi(param1);
  r = atoi(param2);
  g = atoi(param3);
  b = atoi(param4);

  for (int i =0; i < numBlinks*2; i++) { 
    if (i % 2==1) { //Off
      Clyde.setAmbient(RGB(0, 0, 0)); // setting the r, g and b to 255 turns it off.
    } else {
      Clyde.setAmbient(RGB(r, g, b));
    } 
    Clyde.updateAmbientLight();
    delay(1000);  // wait for a second
  }
}

void cmdOff() {
  Clyde.setWhite(255);
  Clyde.setAmbient(RGB(0, 0, 0)); 
  Clyde.updateAmbientLight();
}

void cmdVersion() {
  uint16_t vers = FIRMWARE_VERSION;
  Serial1.print("OK "); 
  Serial1.println(vers);
}

void cmdSetAmbient() {
  char *param1, *param2, *param3;
  int r, g, b;
  
  //Get arguments
  param1 = sCmd.next();    // Red
  param2 = sCmd.next();    // Green
  param3 = sCmd.next();    // Blue
  r = atoi(param1);
  g = atoi(param2);
  b = atoi(param3);
  
  Serial.println(r);
  Serial.println(g);
  Serial.println(b);
  Clyde.setAmbient(RGB(r, g, b));
  Serial.println("OK");
}

void cmdSetWhite() {
  char *param1;
  int w;
  
  //Get arguments
  param1 = sCmd.next();    // Brightness
  w = atoi(param1);
  
  if (w > 255) w = 255;
  w = 255 - w;
  
  Clyde.setWhite(w);
  Serial1.println("OK");
}

// This gets set as the default handler, and gets called when no other command matches.
void unrecognized(const char *command) {
  Serial1.println("What?");
}
