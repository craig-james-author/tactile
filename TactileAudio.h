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

#ifndef TactileAudio_h
#define TactileAudio_h 1

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "TactileCPU.h"
#include "TactileFileManager.h"
#include "AudioPlaySdWavPR.h"     // extension of Audio.h that adds pause/resume feature

class TactileAudio
{
 public:

  TactileAudio(TactileCPU *tc);
  static TactileAudio* setup(TactileCPU *tc);

  const char *getTrackName(int trackNum);

  void setVolume(int percent);
  void setVolume(int trackNumber, int percent);
  void setFadeInTime(int milliseconds);
  void setFadeOutTime(int milliseconds);
  void cancelFades(int trackNumber);

  void setPlayRandomTrackMode(bool r);

  void startTrack(int sensorNumber);
  void stopTrack(int sensorNumber);
  bool isPlaying(int sensorNumber);

  void pauseTrack(int sensorNumber);
  void resumeTrack(int sensorNumber);
  bool isPaused(int sensorNumber);

  int  cancelAll();

  void doTimerTasks();
  
 private:

  TactileCPU *_tc;
  TactileFileManager *_fm;

  // Volume control
  int _targetVolume[NUM_TRACKS];
  int _actualVolume[NUM_TRACKS];
  int _fadeInTime;
  int _fadeOutTime;

  bool _randomTrackMode;

  // Audio player status (per track)
  uint32_t _lastStartTime[NUM_TRACKS];
  uint32_t _lastStopTime[NUM_TRACKS];
  int      _thisFadeInTime[NUM_TRACKS];
  int      _thisFadeOutTime[NUM_TRACKS];
  int      _lastRandomTrackPlayed[NUM_TRACKS];
  bool     _isPaused[NUM_TRACKS];
  
  // Internal methods
  AudioPlaySdWavPR *_getPlayerByTrack(int trackNumber);
  uint8_t _volumePctToByte(int percent);
  void    _setActualVolume(int trackNum, int percent);
  int     _calculateFadeTime(int trackNumber, bool goingUp);
  void    _doFadeInOut(int trackNumber);
  void    _startTrack(int trackNumber);
  void    _startRandomTrack(int trackNumber);
};

#endif
