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

/*----------------------------------------------------------------------
 * This class extends the Audio.h library from PJRC for the Teensy 4
 * audio shield, adding a pause/resume feature.
 *
 * Note: it's not clear if this is the right way to do this. While paused,
 * this simply doesn't get any more data from the SD file and forward it
 * to the audio player. It might be that a more correct way is to
 * fill the buffer with zero bytes or something like that. But this
 * seems to work, so...
 *
 * See: https://www.pjrc.com/teensy/td_libs_Audio.html
 ----------------------------------------------------------------------*/

#ifndef _AUDIO_PLAY_SD_WAV_PR_H_
#define _AUDIO_PLAY_SD_WAV_PR_H_ 1

#include <Audio.h>

class AudioPlaySdWavPR : public AudioPlaySdWav {

public:
  
  // Constructor.
  AudioPlaySdWavPR() {
    paused = 0;
  }

  // override base-class methods
  void update(void);
  void play(const char *filename);
  void stop(void);
  
  // New methods
  void pause(void);
  void resume(void);
  unsigned char isPaused(void);

 private:
  unsigned char paused;
};

#endif // _AUDIO_PLAY_SD_WAV_PR_H_
