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
#include "TactileSensors.h"

/*----------------------------------------------------------------------
 * Initialization.
 ----------------------------------------------------------------------*/

TactileSensors::TactileSensors(TactileCPU *tc) {
  _tc = tc;
}

TactileSensors* TactileSensors::setup(TactileCPU *tc) {

  // Note: it might seem like this could be a static object declared at the
  // program start, but that doesn't work due to some out-of-sequence
  // operations that would occur before the hardware is ready. By creating
  // the TactileSensors object dynamically during the Arduino setup()
  // function, we avoid those problems.

  TactileSensors* t = new TactileSensors(tc);

  for (int sensorNumber = FIRST_SENSOR; sensorNumber <= LAST_SENSOR; sensorNumber++) {
    t->_lastActionTime[sensorNumber] = 0;
    t->_lastSensorStatus[sensorNumber] = IS_RELEASED;
    t->_lastSensorPseudoStatus[sensorNumber] = IS_RELEASED;
    t->_filteredSensorValue[sensorNumber] = 0.0;
    t->_ignoreSensor[sensorNumber] = false;
    t->setProximityMultiplier(sensorNumber, 1.0);
    t->setTouchReleaseThresholds(sensorNumber, 95.0, 65.0);
  }
  t->_lastSensorTouched = -1;
  t->_touchToggleMode = false;

  t->setAveragingStrength(200);

  return t;
}


/*----------------------------------------------------------------------
 * Touch system: was a key touched or released?
 *
 * If multi-touch is not enabled, touch is exclusive; only one sensor will
 * be considered touched at a time. If two or more are touched, the
 * following rules are used:
 *
 *   1. First touched excludes others until it is released
 *   2. Simultaneous touch (two or more): the lowest-numbered sensor
 *      wins, and others are excluded intil it is released.
 *   3. When a sensor is released, if another sensor is already touched,
 *      it will be returned as a new touch the moment the first is
 *      released. If two or more other sensors are touched, the lowest-
 *      numbered one wins.
 *
 * The touchThreshold is in percent, 0.0 to 100.0%. 
 ----------------------------------------------------------------------*/

void TactileSensors::setTouchReleaseThresholds(float touchThreshold, float releaseThreshold) {
  for (int s = 0; s < NUM_SENSORS; s++)
    setTouchReleaseThresholds(s, touchThreshold, releaseThreshold);
}

void TactileSensors::setTouchReleaseThresholds(int sensorNumber, float touchThreshold, float releaseThreshold) {

  if (touchThreshold < 0)
    _touchThreshold[sensorNumber] = 1;
  else if (touchThreshold > 100)
    _touchThreshold[sensorNumber] = 100;
  else
    _touchThreshold[sensorNumber] = touchThreshold;

  if (releaseThreshold >= _touchThreshold[sensorNumber])
    _releaseThreshold[sensorNumber] = _touchThreshold[sensorNumber] - 1;
  else if (releaseThreshold < 0)
    _releaseThreshold[sensorNumber] = 0;
  else
    _releaseThreshold[sensorNumber] = releaseThreshold;

  _tc->logAction2("TactileSensors: Touch threshold: ", _touchThreshold[sensorNumber]);
  _tc->logAction2("TactileSensors: Release threshold: ", _releaseThreshold[sensorNumber]);
}  

void TactileSensors::ignoreSensor(int sensorNumber, bool ignore) {
  if (sensorNumber < 0 || sensorNumber >= NUM_SENSORS)
    return;
  _ignoreSensor[sensorNumber] = ignore;
}

void TactileSensors::setTouchToggleMode(bool on) {
  _touchToggleMode = on;
  _tc->logAction2("TactileSensors: touchToggleMode: ", on ? 1 : 0);
}

/* Returns number of changes since the last call.
 *   - Array sensorChanges is filled with NEW_TOUCH, NEW_RELEASE, or TOUCH_NO_CHANGE.
 *   - Array sensorStatus[] is filled with true/false (1/0) indicating if the sensor it touched or not
 */

int TactileSensors::getTouchStatus(int sensorStatus[], int sensorChanges[]) {

  int numChanges = 0;

  for (int i = FIRST_SENSOR; i <= LAST_SENSOR; i++) {
    sensorChanges[i] = TOUCH_NO_CHANGE;
    float prox = getProximityPercent(i);
    if (prox >= _touchThreshold[i]) {
      sensorStatus[i] = IS_TOUCHED;
      if (_lastSensorStatus[i] != sensorStatus[i]) {
        sensorChanges[i] = NEW_TOUCH;
        numChanges++;
        _lastActionTime[i] = millis();
      }
    } else if (prox < _releaseThreshold[i]) {
      sensorStatus[i] = IS_RELEASED;
      if (_lastSensorStatus[i] != sensorStatus[i]) {
        sensorChanges[i] = NEW_RELEASE;
        numChanges++;
        _lastActionTime[i] = millis();
      }
    } else {
      sensorStatus[i] = _lastSensorStatus[i];
    }
    
    // Record for next time. Note that this is *before* the touch-toggle
    // mode below since we need to know the actual status, not the "pseudo"
    // status of the touch-toggle mode (see below).
    _lastSensorStatus[i] = sensorStatus[i];
    
    // Touch-toggle mode. This uses touch-on-touch-off rather than the standard
    // touch-on-release-off, i.e. each touch toggles the touched/released state.
    // So we convert releases to no-change, and we convert touches alternately
    // to touch/release.

    if (_touchToggleMode) {
      if (sensorChanges[i] == NEW_RELEASE) {
        sensorStatus[i] = _lastSensorPseudoStatus[i];
        sensorChanges[i] = TOUCH_NO_CHANGE;             // ignore all releases
        numChanges--;
      } else if (sensorChanges[i] == NEW_TOUCH) {
        if (_lastSensorPseudoStatus[i] == IS_TOUCHED) { // alternate touches converted to toggle on/off
          sensorChanges[i] = NEW_RELEASE;
          sensorStatus[i] = IS_RELEASED;
          _lastSensorPseudoStatus[i] = IS_RELEASED;
        } else {
          sensorChanges[i] = NEW_TOUCH;
          sensorStatus[i] = IS_TOUCHED;
          _lastSensorPseudoStatus[i] = IS_TOUCHED;
        }
      }
    }
  }

  if (numChanges > 0 && _tc->getLogLevel() > 1) {
    Serial.print("TactileSensors: ");
    Serial.print(numChanges);
    Serial.print(" changed, ");
    for (int i = FIRST_SENSOR; i <= LAST_SENSOR; i++) {
      if      (sensorChanges[i] == NEW_TOUCH)   Serial.print("T");
      else if (sensorChanges[i] == NEW_RELEASE) Serial.print("R");
      else if (sensorStatus[i]  == IS_TOUCHED)  Serial.print("t");
      else if (sensorStatus[i]  == IS_RELEASED) Serial.print("r");
    }
    Serial.println();
  }

  return numChanges;
}

/*----------------------------------------------------------------------
 * Proximity sensor.
 ----------------------------------------------------------------------*/

void TactileSensors::setAveragingStrength(int samples) {
  if (samples < 0)
    samples = 0;
  _averagingSamples = samples;
  _tc->logAction2("TactileSensors: averaging: ", _averagingSamples);
}

void TactileSensors::setProximityMultiplier(int sensorNumber, float m) {
  sensorNumber = _checkSensorRange(sensorNumber);
  _proximityMultiplier[sensorNumber] = m;
}

float TactileSensors::getProximityPercent(int sensorNumber) {
  if (_ignoreSensor[sensorNumber])
    return 0.0;
  sensorNumber = _checkSensorRange(sensorNumber);
  int p = analogRead(_sensorNumberToPinNumber[sensorNumber]);
  if (_averagingSamples > 0) {
    // Simple infinite-impulse-response filter, e.g. if averaging "strength" is 10 and "p"
    // is the new value, then new average "pavg" value is (9*(pavg) + p)/10.
    _filteredSensorValue[sensorNumber] =
      (_filteredSensorValue[sensorNumber] * (_averagingSamples - 1) + (float)p) / (float)_averagingSamples;
    p = (int)_filteredSensorValue[sensorNumber];
  }
  p = p * 100.0 * _proximityMultiplier[sensorNumber] / 1024;  // convert digital signal to percent
  if (p > 100.0)
    p = 100.0;
  return p;
}

int TactileSensors::_checkSensorRange(int sensorNumber) {
  if (sensorNumber < FIRST_SENSOR)
    return FIRST_SENSOR;
  if (sensorNumber > LAST_SENSOR)
    return LAST_SENSOR;
  return sensorNumber;
}
