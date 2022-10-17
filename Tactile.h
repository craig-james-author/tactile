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
