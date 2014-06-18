#include "PlainProtocol.h"

//PlainProtocol constructor, define the Serial port and the baudrate.
PlainProtocol myBLUNO(Serial,115200);

void setup() {
  myBLUNO.init();
}

void loop()
{
  myBLUNO.write("TEMP", 0);     //return the value of temperature to mobile device
  delay(500);
}




