This Arduino library turns a Teensy 4.x microcontroller and audio card into
a touch-sensitive .WAV player, capable of taking input from four capacitive
sensors (implemented by Adafruit Gemma-M0 wearable microcontrollers) and
playing up to four simulaneous .WAV tracks.

A key feature of this library is that you don't have to write an Arduino
sketch at all. The following trivial sketch is a complete, working version
of a touch-sensitive .WAV player.

    #include <Arduino.h>
    #include "Tactile.h"

    Tactile *t;

    void setup() {
      t = Tactile::setup();
    }

    void loop() {
      t->loop();
    }

A number of customizable features provide a wide variety of behaviors, and
can be added to the setup() function. See Tactile.h for details. Here is a
summary:

LOG LEVEL: How much is printed on the Arduino Serial Monitor window?
  0: nothing
  1: normal info
  2: verbose (for code development and debugging

TOUCH/RELEASE THRESHOLD: For simple touch sensing, (95, 60) is a good
choice. For proximity-as-volume (see below), (10, 5) is a good starting
place. The first number (Touch) must always be higher than the second
number (Release).

FADE-IN/FADE-OUT: By default a track starts playing at full volume, and
stops immediately when the sensor is released. If you specify a fade-in
and/or fade-out time, then the track's volume fades or out for the
specified length (in milliseconds, e.g.  1500 is 1.5 seconds).

MULTI-TRACK MODE: Setting this to "true" enables multiple simultaneous
tracks.  The default "false" means only one track plays at a time.

CONTINUE-TRACK MODE: When a track is playing and the sensor is released,
then touched again, does the track resume where it left off ("true"), or
start from the beginning ("false")?

INACTIVITY TIMEOUT: If you set continue-track mode (above), then the
inactivity timeout specifies an idle time; if that time passes with no
activity (no sensors touched), then all tracks are reset and will start
playing from the beginning the next time a sensor is touched. Time is in
seconds.

RANDOM-TRACK MODE: Four directories (which must be named E1, E2, E3, and
E4) can contain two or more .WAV file, which are selected randomly when the
corresponding sensor is touched.

TOUCH-TO-STOP MODE: Normally the sensors operated as touch-play-
release-stop. That is, the track plays while the sensor is being
touched. If you set touch-to-stop mode to "true", then it will operate as
touch- start-touch-stop; that is, releases are ignored, and the track plays
until the sensor is touched again.

PROXIMITY-AS-VOLUME MODE: When set to "true", the proximity of the user's
hand to the sensor is used to set the volume; the closer the user's hand,
the louder the volume. When set to "false", touch mode is enabled, and the
volume is fixed.


PROXIMITY MULTIPLIER: The proximity-multiplier feature can be used to make
the sensor more or less sensitive. Each sensor (0 to 3) is specified
separately.  A value greater than 1 increases sensitivity, and less than
one decreases it. For example 2.0 will double sensitivity; 0.5 will cut
sensitivity in half. NOTE: reducing sensitivity also reduces the total
volume you'll ever get, e.g. a multiplier of 0.5 means you'll never get
more than 50% volume.  // Note that the multiplier also affects touch mode
and the touch/release threshold.

AVERAGING (SMOOTHING) STRENGTH: You probably don't need to modify
this. This is the number of sensor readings that are averaged together to
get the sensor's value. Higher numbers mean more smoothing, but also mean a
slower response time. Lower numbers mean less smoothing (noiser signal) and
faster response. Only change this if you have a noisy situation, usually
indicated if your audio tracks "stutter" (start and stop rapidly).

---------------------------------------------------------------------------
Copyright (c) 2022, Craig A. James

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
---------------------------------------------------------------------------
