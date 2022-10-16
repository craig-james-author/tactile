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

#ifndef Tactile_h
#define Tactile_h 1

#include "TactileCPU.h"
#include "TactileSensors.h"
#include "TactileAudio.h"

#define TOUCH_MODE 1
#define PROXIMITY_MODE 2

class Tactile
{
 public:
  static Tactile *setup();
  void loop(void);

  void setLogLevel(int level);

  void setTouchReleaseThresholds(int touch, int release);
  void setTouchReleaseThresholds(int sensorNumber, int touch, int release);
  void ignoreSensor(int sensorNumber, bool ignore);
  
  void setTouchToStop(bool on);                // true == touch-on-touch-off (normally touch-on-release-off)
  void setMultiTrackMode(bool on);             // true == enable multiple simultaneous tracks
  void setContinueTrackMode(bool on);          // true == 2nd touch continues track where it left off
  void setInactivityTimeout(int seconds);      // continueTrackMode: reset to beginning if idle this long

  void setPlayRandomTrackMode(bool on);        // true == random selection from sensor's directory

  void setVolume(int percent);
  void setProximityAsVolumeMode(bool on);      // Proximity controls volume, or fixed volume
  void setProximityMultiplier(int sensorNumber, float m);  // 1.0 is no amplification, more increases sensitivity
  void setFadeInTime(int milliseconds);
  void setFadeOutTime(int milliseconds);

  void setAveragingStrength(int samples);      // more smooths signal, default is 200

  const char *getTrackName(int trackNum);
  
 private:
  TactileCPU     *_tc;
  TactileSensors *_ts;
  TactileAudio   *_ta;
  TactileFileManager *_fm;

  float    _touchThreshold[NUM_SENSORS];
  float    _releaseThreshold[NUM_SENSORS];
  bool     _touchToStop;
  bool     _multiTrack;
  bool     _continueTrack;
  uint32_t _restartTimeout;
  bool     _useProximityAsVolume;
  int      _ledCycle;
  int      _trackCurrentlyPlaying;
  uint32_t _lastActionTime;
  
  void _touchLoop();
  void _proximityLoop();
  void _doVolumeFadeInAndOut();
  void _startTrackIfStartDelayReached();
  void _doTimerTasks();
};

#endif
