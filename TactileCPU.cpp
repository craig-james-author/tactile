/* -*-C-*-
+======================================================================
| Copyright (c) 2022, Craig A. James
|
| This file is part of of the "Tactile" library.
|
| Tactile is free software: you can redistribute it and/or modify it under
| the terms of the GNU Lesser General Public License (LGPL) as published by
| the Free Software Foundation, either version 3 of the License, or (at
| your option) any later version.
|
| Tactile is distributed in the hope that it will be useful, but WITHOUT
| ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
| FITNESS FOR A PARTICULAR PURPOSE. See the LGPL for more details.
|
| You should have received a copy of the LGPL along with Tactile. If not,
| see <https://www.gnu.org/licenses/>.
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
