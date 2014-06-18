#include "Arduino.h"
#include "PlainProtocol.h"
#include "U8glib.h"
#include "blunoAccessory.h"
#include "Time.h"

#define RIGHT 1
#define UP 2
#define LEFT 3
#define DOWN 4
#define PUSH 5
#define MID 0

//PlainProtocol constructor, define the Serial port and the baudrate.
PlainProtocol myBLUNO(Serial,115200);

//blunoAccessory constructor, for setting the relay,buzzer, temperature, humidity, knob, joystick and RGBLED
blunoAccessory myAccessory;

//OLED constructor, for oled display
U8GLIB_SSD1306_128X64 myOled(U8G_I2C_OPT_NONE);

String oledDisplay="";      //the display string recieved from the mobile device

int ledRed=0;               //RGBLED red value
int ledGreen=0;             //RGBLED green value
int ledBlue=0;              //RGBLED blue value

int joyStick=0;             //joystick state

String distance="0";

void setup() {
  myAccessory.begin();
  myBLUNO.init();
  myAccessory.setRGBLed(0,0,0);   //turn off the RGBLED
  setTime(14,21,10,29,4,2014);
}


void draw (void)
{
  if (distance == "0") { // initial state, show clock
    time_t t = now();

    myOled.setFont(u8g_font_fur20);
    myOled.setPrintPos(10,44);
    myOled.print(hour(t));
    myOled.print(":");
    myOled.print(minute(t));
    myOled.print(":");
    myOled.print(second(t));
  }
  else if (distance == "-1"){ // disconnected
    myOled.setFont(u8g_font_unifont);
    myOled.setPrintPos(36,16);
    myOled.print("(x_x)");

    myOled.setFont(u8g_font_fur20);
    myOled.setPrintPos(0,50);
    myOled.print("sugu iku");
  }
  else if (oledDisplay.length()){ // show distance and received msg
    myOled.setFont(u8g_font_unifont);
    myOled.setPrintPos(40,16);
    myOled.print(distance);
    myOled.print(" m");

    myOled.setFont(u8g_font_fur20);
    myOled.setPrintPos(0,50);
    myOled.print(oledDisplay);
  }
  else { // show distance
    myOled.setFont(u8g_font_fur20);
    myOled.setPrintPos(36,44);
    myOled.print(distance);
    myOled.print(" m");
  }
}

void setLED () {
  if (distance == "0") {
    ledRed=255;
    ledGreen=0;
    ledBlue=255;
    myAccessory.setBuzzer(0);
  }
  else if (distance == "1") {
    ledRed=0;
    ledGreen=0;
    ledBlue=255;
    myAccessory.setBuzzer(0);
  }
  else if (distance == "3") {
    ledRed=0;
    ledGreen=255;
    ledBlue=0;
    myAccessory.setBuzzer(0);
  }
  else if (distance == "10") {
    ledRed=255;
    ledGreen=255;
    ledBlue=0;
    myAccessory.setBuzzer(0);
  }
  else if (distance == "50") {
    ledRed=255;
    ledGreen=0;
    ledBlue=0;
    myAccessory.setBuzzer(1);
  }
  else if (distance == "-1") {
    ledRed=255;
    ledGreen=0;
    ledBlue=0;
    myAccessory.setBuzzer(0);
  }
  myAccessory.setRGBLed(ledRed, ledGreen, ledBlue);   //set the color to the RGBLED
}

void loop()
{
  static unsigned long connTimer=millis();
  if (myBLUNO.available()) {    //receive valid command
    connTimer=millis();
    Serial.print(myBLUNO.read());
    if (myBLUNO.equals("DISP")){
      oledDisplay=myBLUNO.readString();
    }
    else if(myBLUNO.equals("DIST")){
      distance = myBLUNO.readString();
      oledDisplay = "";
    }             
  }
  else if (millis() - connTimer >= 5000) {
    if (distance != "0") {
      distance = "-1";
    }
  }

  setLED(); // Sets LED color based on distance 

  static unsigned long oledTimer=millis();        //every 500ms update the oled display
  if (millis() - oledTimer >= 500) {
    oledTimer=millis();
    myOled.firstPage();                         //for more information about the U8glib library, go to https://code.google.com/p/u8glib/
    do{
      draw();
    }
    while(myOled.nextPage());
  }

  if (myAccessory.joystickAvailable()) {          //if the state of joystick is changed
    joyStick=myAccessory.readJoystick();
    if (joyStick != 0) {
      myBLUNO.write("ROCKER", joyStick);          //send the command and value to mobile device
    }
  }

}
















