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

#include <AudioPlaySdWavPR.h>

void AudioPlaySdWavPR::update(void) {
  if (paused) {
    return;
  }
  AudioPlaySdWav::update();
}

void AudioPlaySdWavPR::pause(void) {
  paused = 1;
}

void AudioPlaySdWavPR::resume(void) {
  paused = 0;
}

void AudioPlaySdWavPR::play(const char *filename) {
  if (!filename) {
    Serial.println("AudioPlaySdWavPR: ERROR: null filename");
    return;
  }
  paused = 0;
  AudioPlaySdWav::play(filename);
}

void AudioPlaySdWavPR::stop(void) {
  paused = 0;
  AudioPlaySdWav::stop();
}

unsigned char AudioPlaySdWavPR::isPaused(void) {
  if (paused && !isPlaying())  // happens if the track reaches the end
    paused = 0;
  return paused;
}
