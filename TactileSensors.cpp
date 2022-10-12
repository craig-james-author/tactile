/* -*-C++-*-
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
  // operations that would occur before the BareTouch board is ready. By
  // creating the TactileSensors object dynamically during the Arduino setup()
  // function, we avoid those problems.


  TactileSensors* t = new TactileSensors(tc);

  for (int sensorNumber = FIRST_SENSOR; sensorNumber <= LAST_SENSOR; sensorNumber++) {
    t->_lastActionTime[sensorNumber] = 0;
    t->_lastSensorStatus[sensorNumber] = IS_RELEASED;
    t->_lastSensorPseudoStatus[sensorNumber] = IS_RELEASED;
    t->_filteredSensorValue[sensorNumber] = 0.0;
    t->setProximityMultiplier(sensorNumber, 1.0);
  }
  t->_lastSensorTouched = -1;

  t->setTouchReleaseThresholds(95, 65);
  t->setAveragingStrength(200);

  return t;
}


/*----------------------------------------------------------------------
 * Touch system: was a key touched or released? Touch is exclusive; only
 * one sensor will be considered touched at a time. If two or more are
 * touched, the following rules are used:
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

  if (touchThreshold < 0)
    _touchThreshold = 1;
  else if (touchThreshold > 100)
    _touchThreshold = 100;
  else
    _touchThreshold = touchThreshold;

  if (releaseThreshold >= _touchThreshold)
    _releaseThreshold = _touchThreshold - 1;
  else if (releaseThreshold < 0)
    _releaseThreshold = 0;
  else
    _releaseThreshold = releaseThreshold;

  _tc->logAction2("TactileSensors: Touch threshold: ", _touchThreshold);
  _tc->logAction2("TactileSensors: Release threshold: ", _releaseThreshold);
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
    if (prox >= _touchThreshold) {
      sensorStatus[i] = IS_TOUCHED;
      if (_lastSensorStatus[i] != sensorStatus[i]) {
        sensorChanges[i] = NEW_TOUCH;
        numChanges++;
        _lastActionTime[i] = millis();
      }
    } else if (prox < _releaseThreshold) {
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
        sensorChanges[i] = TOUCH_NO_CHANGE;             // ignore all releases
        numChanges--;
      } else if (sensorChanges[i] == NEW_TOUCH) {
        if (_lastSensorPseudoStatus[i] == IS_TOUCHED) { // alternate touches converted to toggle on/off
          sensorChanges[i] = IS_RELEASED;
          _lastSensorPseudoStatus[i] = IS_RELEASED;
        } else {
          sensorChanges[i] = NEW_TOUCH;
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
