/* -*-C-*-
+======================================================================
| Copyright (c) 2022, Craig A. James
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
