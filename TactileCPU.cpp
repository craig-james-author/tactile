/* -*-C-*-
+======================================================================
| Copyright (c) 2021, Craig A. James
|
| Permission is hereby granted, free of charge, to any person obtaining a
| copy of this software and associated documentation files (the
| "Software"), to deal in the Software without restriction, including
| without limitation the rights to use, copy, modify, merge, publish,
| distribute, sublicense, and/or sell copies of the Software, and to permit
| persons to whom the Software is furnished to do so, subject to the
| following conditions:
|
| The above copyright notice and this permission notice shall be included
| in all copies or substantial portions of the Software.
|
| THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
| OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
| MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
| NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
| DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
| OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
| USE OR OTHER DEALINGS IN THE SOFTWARE.
+======================================================================
*/

#include "Arduino.h"
#include "TactileCPU.h"

TactileCPU* TactileCPU::setup() {
  TactileCPU* tcpu = new(TactileCPU);
  //  pinMode(TC_LED_PIN, OUTPUT);    // declare the TC_LED_PIN as an OUTPUT:
  Serial.begin(57600);
  tcpu->_logLevel = 1;
  return tcpu;
}

/*----------------------------------------------------------------------
 * Simple utility functions
 ----------------------------------------------------------------------*/

void TactileCPU::setLogLevel(int level) {
  _logLevel = level;
}

int TactileCPU::getLogLevel() {
  return _logLevel;
}

void TactileCPU::log(const char *msg) {
  if (_logLevel < 1) return;
  if (msg == NULL)
    Serial.println("TactileCUP::log: NULL message!");
  else
    Serial.println(msg);
}

void TactileCPU::logAction(const char *msg, int track) {
  if (_logLevel < 1) return;
  if (msg == NULL)
    Serial.println("TactileCPU::logAction: NULL message!");
  else {
    Serial.print(msg);
    Serial.println(track);
  }
}

void TactileCPU::log2(const char *msg) {
  if (_logLevel < 2) return;
  if (msg == NULL)
    Serial.println("TactileCPU::log2: NULL message!");
  else
    Serial.println(msg);
}

void TactileCPU::logAction2(const char *msg, int track) {
  if (_logLevel < 2) return;
  if (msg == NULL)
    Serial.println("TactileCPU::logAction2: NULL message!");
  else {
    Serial.print(msg);
    Serial.println(track);
  }
}

void TactileCPU::turnLedOn() {
  //  digitalWrite(TC_LED_PIN, HIGH);
}

void TactileCPU::turnLedOff() {
  //  digitalWrite(TC_LED_PIN, LOW);
}

void TactileCPU::sleep(int milliseconds) {
  delay(milliseconds);
}
