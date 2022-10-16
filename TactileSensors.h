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
