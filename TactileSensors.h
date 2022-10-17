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

#ifndef TactileSensors_h
#define TactileSensors_h 1

#include "TactileCPU.h"

// Touches to the electrodes
#define IS_TOUCHED 1
#define IS_RELEASED 0
#define TOUCH_NO_CHANGE 0
#define NEW_TOUCH 1
#define NEW_RELEASE 2


class TactileSensors
{
 public:

  // Constructors
  TactileSensors(TactileCPU* tc);
  static TactileSensors* setup(TactileCPU* tc);

  // Touch and proximity sensing
  void  setTouchReleaseThresholds(float touchThreshold, float releaseThreshold);
  void  setTouchReleaseThresholds(int sensorNumber, float touchThreshold, float releaseThreshold);
  void  ignoreSensor(int sensorNumber, bool ignore);
  void  setTouchToggleMode(bool on);
  int   getTouchStatus(int sensorStatus[], int sensorChanges[]);
  float getProximityPercent(int sensorNumber);
  void  setAveragingStrength(int samples);
  void  setProximityMultiplier(int sensorNumber, float m);

 private:

  const int _sensorNumberToPinNumber[NUM_SENSORS] = {A14, A15, A16, A17};

  TactileCPU *_tc;
  
  // General controls
  bool _touchToggleMode;        // touch-on-touch-off rather than touch-on-release-off

  // Touch sensors
  int   _lastSensorTouched;
  float _touchThreshold[NUM_SENSORS];          // Percent, 0..100
  float _releaseThreshold[NUM_SENSORS];
  bool  _ignoreSensor[NUM_SENSORS];
  int   _lastSensorStatus[NUM_SENSORS];
  int   _lastSensorPseudoStatus[NUM_SENSORS];    // for touchToggleMode only
  unsigned long _lastActionTime[NUM_SENSORS];
  float _proximityMultiplier[NUM_SENSORS];

  // Proximity detection and smoothing
  float _filteredSensorValue[NUM_SENSORS];
  int   _averagingSamples;

  int _checkSensorRange(int sensorNumber);

};

#endif
