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

#ifndef TactileCPU_h
#define TactileCPU_h 1

#include "TactileBasics.h"

// Hardware-specific Teensy 4.1 definitions
#define TC_LED_PIN 13

class TactileCPU {
 public:

  static TactileCPU *setup();
  void turnLedOn();
  void turnLedOff();
  void setLogLevel(int level);
  int  getLogLevel();
  void log(const char *msg);
  void log2(const char *msg);
  void logAction(const char *msg, int track);
  void logAction2(const char *msg, int track);
  void sleep(int milliseconds);

 private:
  int _logLevel;
};

#endif
