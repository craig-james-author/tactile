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

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "TactileAudio.h"

// GUItool: begin automatically generated code
AudioPlaySdWavPR           playSdWav1;     //xy=124,100
AudioPlaySdWavPR           playSdWav2;     //xy=124,160
AudioPlaySdWavPR           playSdWav3;     //xy=124,220
AudioPlaySdWavPR           playSdWav4;     //xy=124,280
AudioMixer4              mixer1;         //xy=470,160
AudioMixer4              mixer2;         //xy=470,280
AudioOutputI2S           i2s1;           //xy=650,220
AudioConnection          patchCord1(playSdWav1, 0, mixer1, 0);
AudioConnection          patchCord2(playSdWav1, 1, mixer2, 0);
AudioConnection          patchCord3(playSdWav2, 0, mixer1, 1);
AudioConnection          patchCord4(playSdWav2, 1, mixer2, 1);
AudioConnection          patchCord5(playSdWav3, 0, mixer1, 2);
AudioConnection          patchCord6(playSdWav3, 1, mixer2, 2);
AudioConnection          patchCord7(playSdWav4, 0, mixer1, 3);
AudioConnection          patchCord8(playSdWav4, 1, mixer2, 3);
AudioConnection          patchCord9(mixer1, 0, i2s1, 0);
AudioConnection          patchCord10(mixer2, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000;     //xy=127,379.111083984375
// GUItool: end automatically generated code

TactileAudio::TactileAudio(TactileCPU *tc) {
  _tc = tc;
}

TactileAudio* TactileAudio::setup(TactileCPU *tc) {

  TactileAudio* t = new TactileAudio(tc);
  t->_fadeInTime          = 0;
  t->_fadeOutTime         = 0;
  t->setVolume(100);
  t->_randomTrackMode     = false;
  t->_loopMode            = false;

  for (int trackNumber = 0; trackNumber < NUM_TRACKS; trackNumber++) {
    t->_targetVolume[trackNumber]          = 100;
    t->_actualVolume[trackNumber]          = 0;
    t->_lastStartTime[trackNumber]         = 0;
    t->_lastStopTime[trackNumber]          = 0;
    t->_thisFadeInTime[trackNumber]        = 0;
    t->_thisFadeOutTime[trackNumber]       = 0;
    t->_lastRandomTrackPlayed[trackNumber] = -1;
    t->_isPaused[trackNumber]              = false;
  }  

  // Initialization for the Teensy Audio Shield
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14
  AudioMemory(2*NUM_TRACKS+4);
  sgtl5000.enable();
  sgtl5000.volume(0.90);
  delay(1000);  // wait for SGTL5000 to initialize

  for (int i = 0; i < 4; i++) {
    mixer1.gain(i, 1.0);
    mixer2.gain(i, 1.0);
  }

  t->_fm = new TactileFileManager(tc);
 
  tc->log2("TactileAudio::setup() complete.");

  return t;
}

/*----------------------------------------------------------------------
 * Tracks
 ----------------------------------------------------------------------*/

const char *TactileAudio::getTrackName(int trackNum) {
  if (trackNum < 0 || trackNum > NUM_TRACKS)
    return NULL;
  return _fm->getFileName(trackNum);
}

/*----------------------------------------------------------------------
 * Volume controls
 ----------------------------------------------------------------------*/

void TactileAudio::_setActualVolume(int trackNum, int percent) {
  _actualVolume[trackNum] = percent;
  float gain  = (float)percent/100.0;  // Convert percent (0-100) to gain (0-1.0)
  mixer1.gain(trackNum, gain);
  mixer2.gain(trackNum, gain);
}

void TactileAudio::setVolume(int trackNum, int percent) {
  if (trackNum < 0)
    trackNum = 0;
  else if (trackNum >= NUM_TRACKS)
    trackNum = NUM_TRACKS - 1;
  _targetVolume[trackNum] = percent;
  if (!_fadeInTime)
    _setActualVolume(trackNum, percent);
}

void TactileAudio::setVolume(int percent) {
  for (int i = 0; i < NUM_TRACKS; i++)
    setVolume(i, percent);
}

void TactileAudio::setFadeInTime(int milliseconds) {
  _fadeInTime = milliseconds;
  _tc->logAction2("TactileAudio: setFadeInTime: ", milliseconds);
}

void TactileAudio::setFadeOutTime(int milliseconds) {
  _fadeOutTime = milliseconds;
  _tc->logAction2("TactileAudio: setFadeOutTime: ", milliseconds);
}

void TactileAudio::cancelFades(int trackNumber) {
  _lastStartTime[trackNumber] = 0;
  _lastStopTime[trackNumber] = 0;
  _setActualVolume(trackNumber, 0);
}
  
int TactileAudio::cancelAll() {
  int cancelled = 0;
  for (int trackNumber = 0; trackNumber < NUM_TRACKS; trackNumber++) {
    AudioPlaySdWavPR *player = _getPlayerByTrack(trackNumber);
    if (!player) return 0;
    if (player->isPlaying()) {
      player->stop();
      cancelled++;
    }
    _lastStartTime[trackNumber] = 0;
    _lastStopTime[trackNumber] = 0;
  }
  return cancelled;
}    

/*----------------------------------------------------------------------
 * Start and Stop tracks
 ----------------------------------------------------------------------*/

void TactileAudio::setPlayRandomTrackMode(bool r) {
  _randomTrackMode = r;
}

void TactileAudio::setLoopMode(bool on) {
  _loopMode = on;
}

AudioPlaySdWavPR *TactileAudio::_getPlayerByTrack(int trackNumber) {
  if (trackNumber < 0)
    trackNumber = 0;
  else if (trackNumber >= NUM_TRACKS)
    trackNumber = NUM_TRACKS - 1;
  switch (trackNumber) {
  case 0: return &playSdWav1;
  case 1: return &playSdWav2;
  case 2: return &playSdWav3;
  case 3: return &playSdWav4;
  }
  _tc->logAction("TactileAudio: Invalid trackNumber: ", trackNumber);
  return NULL;  // to keep compiler happy, never happens
}

void TactileAudio::startTrack(int trackNumber) {
  if (trackNumber < 0)
    trackNumber = 0;
  else if (trackNumber >= NUM_TRACKS)
    trackNumber = NUM_TRACKS - 1;
  if (_randomTrackMode)
    _startRandomTrack(trackNumber);
  else
    _startTrack(trackNumber);
  if (_fadeInTime == 0)
    _setActualVolume(trackNumber, _targetVolume[trackNumber]);
  else
    _thisFadeInTime[trackNumber] = _calculateFadeTime(trackNumber, true);
  _lastStartTime[trackNumber] = millis();
  _lastStopTime[trackNumber] = 0;
}

void TactileAudio::_startTrack(int trackNumber) {
  const char *trackName = _fm->getFileName(trackNumber);
  if (!trackName) {
    _tc->logAction("Can't find that track: ", trackNumber);
    return;
  }
  AudioPlaySdWavPR *player = _getPlayerByTrack(trackNumber);
  if (!player) return;
  player->play(trackName);
  if (_tc->getLogLevel() > 1) {
    Serial.print("TactileAudio: start track ");
    Serial.print(trackNumber);
    Serial.print(", ");
    Serial.println(trackName);
  }
}

void TactileAudio::_startRandomTrack(int trackNumber) {
  // Selects a track randomly from directory EN, where "N"
  // is the track number (e.g. E0, E1, ...). However, it
  // tries to avoid replaying the last-played "random" track.

  _tc->logAction2("TactileAudio: startRandomTrack ", trackNumber);

  AudioPlaySdWavPR *player = _getPlayerByTrack(trackNumber);
  if (!player) return;

  int numFiles = _fm->getNumFiles(trackNumber);
  _tc->logAction2("TactileAudio: Files in directory: ", numFiles);
  if (numFiles < 1) return;

  int r;
  int tries = 0;
  while (tries < 30) {
    r = random(numFiles);
    if (r != _lastRandomTrackPlayed[trackNumber])       // avoid last track played
      break;
    tries++;
  }
  _lastRandomTrackPlayed[trackNumber] = r;
  _tc->logAction2("TactileAudio: Random track selected: ", r);
  const char *fileName = _fm->getFileName(trackNumber, r);
  if (!fileName) {
    _tc->logAction2("Error, couldn't get random filename (this shouldn't happen) for track ", trackNumber);
    return;
  }

  char filePath[MAX_FILE_NAME+5] = "/Ex/";
  filePath[2] = '1' + trackNumber;  // i.e. /E1/, /E2/, ...
  strcpy(filePath+4, fileName);
  _tc->log2(filePath);

  player->play(filePath);

  if (_tc->getLogLevel() > 1) {
    Serial.print("TactileAudio: start random track: ");
    Serial.print(filePath);
    Serial.print(" (");
    Serial.print(trackNumber);
    Serial.print(", ");
    Serial.print(r);
    Serial.println(")");
  }
}

void TactileAudio::stopTrack(int trackNumber) {
  AudioPlaySdWavPR *player = _getPlayerByTrack(trackNumber);
  if (!player) return;
  if (_fadeOutTime == 0) {
    player->stop();
    _setActualVolume(trackNumber, 0);
  } else {
    // If fade-out enabled, don't actually stop the track. That will happen
    // when fade-out finishes (see _doFadeInOut(), below).
    _thisFadeOutTime[trackNumber] = _calculateFadeTime(trackNumber, false);
  }

  _tc->logAction2("TactileAudio: stop ", trackNumber);
  _lastStopTime[trackNumber] = millis();  // for calculating fade-out
  _lastStartTime[trackNumber] = 0;
}  

bool TactileAudio::isPlaying(int trackNumber) {
  AudioPlaySdWavPR *player = _getPlayerByTrack(trackNumber);
  if (!player) return 0;
  return player->isPlaying();
}

/*----------------------------------------------------------------------
 * Pause and Resume tracks
 ----------------------------------------------------------------------*/

void TactileAudio::pauseTrack(int trackNumber) {
  AudioPlaySdWavPR *player = _getPlayerByTrack(trackNumber);
  if (!player) return; 
  if (_fadeOutTime == 0) {
    player->pause();
    _setActualVolume(trackNumber, 0);
  } else {
    // If fade-out enabled, don't actually pause the track. That will happen
    // when fade-out finishes (see _doFadeInOut(), below).
    _thisFadeOutTime[trackNumber] = _calculateFadeTime(trackNumber, false);
  }
  _isPaused[trackNumber] = true;
  _lastStartTime[trackNumber] = 0;
  _lastStopTime[trackNumber] = millis();     // for calculating fade-out
  _tc->logAction2("TactileAudio: pause ", trackNumber);
}

void TactileAudio::resumeTrack(int trackNumber) {

  AudioPlaySdWavPR *player = _getPlayerByTrack(trackNumber);
  if (!player) return;

  player->resume();
  _isPaused[trackNumber] = false;
  if (_fadeInTime == 0)
    _setActualVolume(trackNumber, _targetVolume[trackNumber]);
  else
    _thisFadeInTime[trackNumber] = _calculateFadeTime(trackNumber, true);

  _lastStartTime[trackNumber] = millis();
  _lastStopTime[trackNumber] = 0;

  _tc->logAction2("TactileAudio: resume ", trackNumber);
}

bool TactileAudio::isPaused(int trackNumber) {
  return _isPaused[trackNumber];
}

int TactileAudio::_calculateFadeTime(int trackNumber, bool goingUp) {
  int deltaVolume;
  int fadeTime;
  if (goingUp) {
    deltaVolume = 100 - _actualVolume[trackNumber];
    fadeTime = _fadeInTime;
  } else {
    deltaVolume = _actualVolume[trackNumber];
    fadeTime = _fadeOutTime;
  }
  int time = (int)(0.499 + (float)fadeTime * (float)deltaVolume/100.0);
  return time;
}

void TactileAudio::_doFadeInOut(int trackNumber)
{
  int targetVol = _targetVolume[trackNumber];
  int actualVol = _actualVolume[trackNumber];

  // Do fade-in? When fade-in is enabled, tracks are started at zero volume,
  // so they're initially in the "is playing" state even though the volume
  // is zero.

  if (_lastStartTime[trackNumber] > 0
      && _fadeInTime != 0
      && actualVol < targetVol
      && isPlaying(trackNumber)) {

    // Calculate the target volume based on how much elapsed time since the track started playing.
    unsigned long elapsedTime = millis() - _lastStartTime[trackNumber];

    // But if _thisFadeInTime is different that _fadeInTime, it means we started in the middle
    // of a fade-out (i.e. the volume wasn't zero yet), so push the elapsed time forward by
    // difference of the two.
    if (_fadeInTime != _thisFadeInTime[trackNumber])
      elapsedTime += _fadeInTime - _thisFadeInTime[trackNumber]; // push elapsed time forward
    
    int newVolumePercent = int((float)targetVol*(float)elapsedTime/(float)_fadeInTime);
    
    // Time to increase volume?
    if (newVolumePercent != actualVol) {
      if (newVolumePercent >= targetVol) {
	newVolumePercent = targetVol;
      }
      _tc->logAction2("TactileAudio: Fade-in, set volume: ", newVolumePercent);
      _setActualVolume(trackNumber, newVolumePercent);
    }
  }

  // Else -- do fade-out? When fade-out is enabled, a track can be in the "is stopped"
  // state but still be actually playing. Only when the target volume gets to zero
  // is the track actually stopped.

  else if (_fadeOutTime != 0                            // fade-out is enabled
           && _lastStopTime[trackNumber] > 0            // the track is stopped or paused...
           && isPlaying(trackNumber)                    // ... but the track is still playing
	   && actualVol > 0) {                          // and volume hasn't reached zero yet

    // Calculate the target volume based on how much elapsed time since the track stopped playing.
    unsigned long elapsedTime = millis() - _lastStopTime[trackNumber];

    // But if _thisFadeOutTime is different than _fadeOutTime, it means we started with volume less
    // than 100%, so push elapsed time forward by the difference of the two.
    if (_fadeOutTime != _thisFadeOutTime[trackNumber]) {
      elapsedTime += _fadeOutTime - _thisFadeOutTime[trackNumber];
    }

    int newVolumePercent = targetVol - int((float)(targetVol)*(float)elapsedTime/(float)_fadeOutTime);

    if (newVolumePercent != actualVol) {

      // Time to decrease volume.
      // _tc->logAction2("TactileAudio: Fade-out, set volume: ", newVolumePercent);
      if (newVolumePercent < 0)
	newVolumePercent = 0;
      _setActualVolume(trackNumber, newVolumePercent);

      // If fade-out reached zero volume, actually stop or pause the track.
      // Note that _isPaused is true as soon as pauseTrack()
      // is called, but the track keeps playing until this fade-out finishes.
      if (newVolumePercent == 0) {
        AudioPlaySdWavPR *player = _getPlayerByTrack(trackNumber);
        if (!player) return;
	if (isPaused(trackNumber)) {
          player->pause();
	  _tc->logAction2("TactileAudio: fade-out done, track paused: ", trackNumber);
	} else {
	  player->stop();
          _tc->logAction2("TactileAudio: fade-out done, track stopped: ", trackNumber);
	}
        _lastStopTime[trackNumber] = 0;
      }
    }
  }
  // No fade-in or fade-out
  // else {
  //   if (_actualVolume[trackNumber] != _targetVolume[trackNumber]) {
  //     AudioPlaySdWavPR *player = _getPlayerByTrack(trackNumber);
  //     if (player->isPlaying() && !player->isPaused()) {
  //       _setActualVolume(trackNumber, _targetVolume[trackNumber]);
  //     } else {
  //       _setActualVolume(trackNumber, 0);
  //     }
  //   }
  // }
}

void TactileAudio::doTimerTasks()
{
  for (int trackNumber = 0; trackNumber < NUM_TRACKS; trackNumber++)
    _doFadeInOut(trackNumber);

  // If a track that was playing reached the end of the track, change its status.
  for (int trackNumber = 0; trackNumber < NUM_TRACKS; trackNumber++) {
    if (_lastStartTime[trackNumber] > 0) {
      AudioPlaySdWavPR *player = _getPlayerByTrack(trackNumber);
      if (!player) return;
      uint32_t now = millis();
      if (now - _lastStartTime[trackNumber] > 50) {  // Player doesn't reliably report isPlaying() for a
        if (!player->isPlaying()) {                  // few msec, so if it just started playing, skip this.
          if (_loopMode) {
            startTrack(trackNumber);
            _tc->logAction2("end of track, looping: ", trackNumber);
          } else {
            _lastStartTime[trackNumber] = 0;
            _lastStopTime[trackNumber] = now;
            _tc->logAction2("end of track ", trackNumber);
          }
        }
      }
    }
  }
}
