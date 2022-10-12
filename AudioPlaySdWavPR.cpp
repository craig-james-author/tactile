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
