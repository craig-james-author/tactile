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

/*======================================================================
|
| This class is essentially a full Arduino sketch wrapped up in a
| C++ class, including the setup() and loop() methods. The only thing
| needed to program a fully functioning app is to specify the options
| (behavior) desired, and forward the Arduino setup() and loop()
| functions to this class.
|
| NOTE: For this "wrapper" class, all sensor numbers go from 1 to 4 rather
|       than the internal numbering of 0 to 3. That is, incoming API calls
|       subtract 1 from the track number, and outgoing results (e.g.
|       logging to the serial monitor) add 1 to the track number. This
|       it to make it more "natural" for non-programmers, and to match
|       the numbering of the E1..E4 subdirectories used for random tracks.
|
+======================================================================*/

#include <Arduino.h>
#include "Tactile.h"

void Tactile::setLogLevel(int level) {
  _tc->setLogLevel(level);
}

const char *Tactile::getTrackName(int trackNum) {
  return _ta->getTrackName(trackNum);
}

void Tactile::setVolume(int percent) {
  _ta->setVolume(percent);
}

void Tactile::setTouchReleaseThresholds(int touch, int release) {
  for (int s = 1; s <= NUM_SENSORS; s++)                // note: external sensor number 1..N
    setTouchReleaseThresholds(s, touch, release);
}

void Tactile::setTouchReleaseThresholds(int externSensorNumber, int touch, int release) {
  int sensorNumber = externSensorNumber - 1;
  if (touch > 100)
    touch = 100;
  else if (touch < 1)
    touch = 1;
  if (release >= touch)
    release = touch - 1;
  else if (release < 0)
    release = 0;
  _touchThreshold[sensorNumber] = touch;
  _releaseThreshold[sensorNumber] = release;
  _ts->setTouchReleaseThresholds(sensorNumber, (float)touch, (float)release);
}

void Tactile::ignoreSensor(int externSensorNumber, bool ignore) {
  int sensorNumber = externSensorNumber - 1;
  _ts->ignoreSensor(sensorNumber, ignore);
}

void Tactile::setTouchToStop(boolean on) {
  _touchToStop = on;
  _ts->setTouchToggleMode(on);
}

void Tactile::setMultiTrackMode(boolean on) {
  _multiTrack = on;
}

void Tactile::setContinueTrackMode(boolean on) {
  _continueTrack = on;
}

void Tactile::setLoopMode(boolean on) {
  _ta->setLoopMode(on);
}

void Tactile::setInactivityTimeout(int seconds) {
  if (seconds < 0)
    seconds = 0;
  _restartTimeout = (uint32_t)(seconds * 1000);
}

void Tactile::setPlayRandomTrackMode(boolean on) {
  _ta->setPlayRandomTrackMode(on);
}

void Tactile::setProximityMultiplier(int externSensorNumber, float m) {
  int sensorNumber = externSensorNumber - 1;
  _ts->setProximityMultiplier(sensorNumber, m);
}

void Tactile::setAveragingStrength(int samples) {
  _ts->setAveragingStrength(samples);
}

void Tactile::setProximityAsVolumeMode(boolean on) {
  _useProximityAsVolume = on;
  if (on) {
    _ta->setFadeInTime(0);        // Fade in/out isn't compatible with proximity-as-volume
    _ta->setFadeOutTime(0);
  }
}

void Tactile::setFadeInTime(int milliseconds) {
  if (milliseconds > 0 && _useProximityAsVolume) {
    Serial.println("WARNING: proximity-as-volume mode is incompatible with fade-in/fade-out. "
                   "Fade-in time ignored.");
    return;
  }
  if (milliseconds < 0)
    milliseconds = 0;
  _ta->setFadeInTime(milliseconds);
}

void Tactile::setFadeOutTime(int milliseconds) {
  if (milliseconds > 0 && _useProximityAsVolume) {
    Serial.println("WARNING: proximity-as-volume mode is incompatible with fade-in/fade-out. "
                   "Fade-out time ignored.");
    return;
  }
  if (milliseconds < 0)
    milliseconds = 0;
  _ta->setFadeOutTime(milliseconds);
}

Tactile* Tactile::setup() {

  Tactile *t = new(Tactile);

  t->_tc = TactileCPU::setup();
  t->_tc->setLogLevel(1);

  t->_ts = TactileSensors::setup(t->_tc);
  t->_ta = TactileAudio::setup(t->_tc);

  t->setMultiTrackMode(false);
  t->setContinueTrackMode(false);
  t->setInactivityTimeout(0);
  t->setPlayRandomTrackMode(false);
  t->setProximityAsVolumeMode(false);
  t->setTouchReleaseThresholds(95, 65);

  t->_ledCycle = 0;
  t->_trackCurrentlyPlaying = -1;
  t->_lastActionTime = millis();
  
  return t;
}
    
void Tactile::_touchLoop() {
  int sensorStatus[NUM_SENSORS];
  int sensorChanged[NUM_SENSORS];

  int numChanged = _ts->getTouchStatus(sensorStatus, sensorChanged);

  int numTouched = 0;
  for (int sensorNumber = FIRST_SENSOR; sensorNumber <= LAST_SENSOR; sensorNumber++) {
    if (sensorStatus[sensorNumber] == IS_TOUCHED)
      numTouched++;
  }
  if (numTouched > 0 || numChanged > 0)
    _lastActionTime = millis();

  if (numTouched > 0)
    _tc->turnLedOn();
  else
    _tc->turnLedOff();

  if (numChanged == 0)
    return;

  // MULTI-TRACK MODE. Simple: if a sensor is touched, start playing the
  // track; if it's released, stop playing. Multiple tracks can go at
  // the same time.

  if (_multiTrack) {
    for (int sensorNumber = FIRST_SENSOR; sensorNumber <= LAST_SENSOR; sensorNumber++) {
      int isPlaying = _ta->isPlaying(sensorNumber);
      if (sensorChanged[sensorNumber] == NEW_TOUCH) {
        if (!isPlaying) {
          if (!_continueTrack)
            _ta->cancelFades(sensorNumber);
          _ta->startTrack(sensorNumber);
          _tc->logAction("start track ", sensorNumber+1);
        } else {
          if (_ta->isPaused(sensorNumber)) {
            _ta->resumeTrack(sensorNumber);
            _tc->logAction("resume track ", sensorNumber+1);
          } else {
            _ta->startTrack(sensorNumber);
            _tc->logAction("restart track (was paused?) ", sensorNumber+1);
          }
        }
      }
      else if (sensorChanged[sensorNumber] == NEW_RELEASE) {
        if (isPlaying) {
          if (_continueTrack) {
            _ta->pauseTrack(sensorNumber);
            _tc->logAction("pause track ", sensorNumber+1);
          } else {
            _ta->stopTrack(sensorNumber);
            _tc->logAction("stop track ", sensorNumber+1);
          }
        }
      }
    }
  }

  // SINGLE-TRACK MODE. This is, surprisingly, a bit more complicated.
  // 
  // When a sensor is touched, play that track if nothing is already
  // playing, and as long as the sensor is still touched, keep playing.
  //
  // When a sensor is released, it's more complicated.
  //   - If no other sensor is being touched, just stop the track playing.
  //   - If one or more other sensors are being touched as this one
  //     is released, select the lowest, and consider it a "new touch",
  //     that is, start that track.
  
  else {

    // If the sensor for the track currently playing is still touched, keep playing (i.e. do nothing).
    if (_trackCurrentlyPlaying >= 0
        && sensorStatus[_trackCurrentlyPlaying] == IS_TOUCHED) {
      return;
    }      

    // If there's a release event, stop or pause that track.
    if (_trackCurrentlyPlaying >= 0
        && sensorChanged[_trackCurrentlyPlaying] == NEW_RELEASE) {
      if (_continueTrack) {
        _ta->pauseTrack(_trackCurrentlyPlaying);
        _tc->logAction("pause track ", _trackCurrentlyPlaying+1);
      } else {
        _ta->stopTrack(_trackCurrentlyPlaying);
        _tc->logAction("stop track ", _trackCurrentlyPlaying+1);
      }
      _trackCurrentlyPlaying = -1;
    }

    // Is one or more other sensor being touched? The track for the lowest-numbered
    // touched sensor is played (whether it's a new touch or an ongoing touch
    // that started before the last release).

    for (int sensorNumber = FIRST_SENSOR; ; sensorNumber++) {
      if (sensorNumber > LAST_SENSOR) {
        _trackCurrentlyPlaying = -1;    // no other sensors are being touched, nothing is playing
        break;
      }
      if (sensorStatus[sensorNumber] == IS_TOUCHED) {
        if (_ta->isPaused(sensorNumber)) {
          _ta->resumeTrack(sensorNumber);
          _tc->logAction("resume track ", sensorNumber+1);
        } else {
          if (!_continueTrack)
            _ta->cancelFades(sensorNumber);
          _ta->startTrack(sensorNumber);
          _tc->logAction("start track ", sensorNumber+1);
        }
        _trackCurrentlyPlaying = sensorNumber;
        break;
      }
    }
  }
}
    
void Tactile::_proximityLoop() {
  float sensorValues[NUM_SENSORS];
  float maxSensorValue = 0.0;
  int   maxSensorNumber = -1;

  for (int sensorNumber = FIRST_SENSOR; sensorNumber <= LAST_SENSOR; sensorNumber++) {
    sensorValues[sensorNumber] = _ts->getProximityPercent(sensorNumber);
    if (sensorValues[sensorNumber] > maxSensorValue) {
      maxSensorValue = sensorValues[sensorNumber];
      maxSensorNumber = sensorNumber;
    }
  }

  // Set the LED brightness proportional to whichever sensor has the highest value
  int led_percent = int(maxSensorValue);
  if (_ledCycle < led_percent)
    _tc->turnLedOn();
  else
    _tc->turnLedOff();
  _ledCycle++;

  // Log sensor values (once every 100 loops)
  if (_ledCycle > 99) {
    _ledCycle = 0;
    /* for (int sensorNumber = FIRST_SENSOR; sensorNumber <= LAST_SENSOR; sensorNumber++) { */
    /*   Serial.print(sensorNumber); */
    /*   Serial.print(":"); */
    /*   Serial.print(sensorValues[sensorNumber]); */
    /*   Serial.print("   "); */
    /* } */
    /* Serial.println(); */
  }

  for (int sensorNumber = FIRST_SENSOR; sensorNumber <= LAST_SENSOR; sensorNumber++) {
    if (_multiTrack || sensorNumber == maxSensorNumber) {
      float sensorValue = sensorValues[sensorNumber];
      int playing = _ta->isPlaying(sensorNumber) && !_ta->isPaused(sensorNumber);
      if (sensorValue > _touchThreshold[sensorNumber]) {
        if (!playing) {
          if (_ta->isPaused(sensorNumber)) {
            _ta->resumeTrack(sensorNumber);
            _tc->logAction("resume ", sensorNumber+1);
          } else {
            _ta->startTrack(sensorNumber);
            _tc->logAction("play ", sensorNumber+1);
          }
        }
        _ta->setVolume(sensorNumber, sensorValue);
        _lastActionTime = millis();
      } else if (sensorValue < _releaseThreshold[sensorNumber]) {
        if (playing) {
          if (_continueTrack) {
            _ta->pauseTrack(sensorNumber);
            _tc->logAction("pause ", sensorNumber+1);
          } else {
            _ta->stopTrack(sensorNumber);
            _tc->logAction("stop ", sensorNumber+1);
          }
          _ta->setVolume(sensorNumber, 0);
          _lastActionTime = millis();
        }
      }
    }
  }
}

void Tactile::loop() {

  // Respond to sensor touch/proximity
  if (_useProximityAsVolume)
    _proximityLoop();
  else
    _touchLoop();

  // Do fade-in/out
  _ta->doTimerTasks();
  
  // If the idle-time has expired, reset the continue-track feature to start over
  uint32_t elapsed = millis() - _lastActionTime;
  if (elapsed > _restartTimeout) {
    if (_ta->cancelAll()) {
      _tc->logAction("Inactivity timeout: ", _restartTimeout);
      _lastActionTime = millis();
    }
  }
}


