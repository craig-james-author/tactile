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

// Number of touch sensors
#define FIRST_SENSOR 0
#define LAST_SENSOR 3
#define NUM_SENSORS 4

// Number of available tracks, referenced as 0 to (NUM_TRACKS-1)
// Note: should be the same as the number of sensors, above.
// This is also the number of subdirectories for selecting random tracks.
#define NUM_TRACKS 4
#define NUM_SUBDIRS 4
#define NUM_TRACKS_IN_SUBDIR 100

// Max string length of filename on SD card
#define MAX_FILE_NAME 255

