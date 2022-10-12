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

/*----------------------------------------------------------------------
 * A very simple "file manager" for the specific configuration of .WAV
 * files expected/supported for the Tactile system. On initialization,
 * the TactileFileManager class reads the contents of the SD file and
 * stores it. Subsequently, files in the root directory and the 
 * subdirectories E1-En are referred to by an integer index.
 *
 * (Note: The subdirectories are named starting with 1 (i.e. E1..EN)
 * for simplicity  with the expected use of this module, but are indexed
 * starting with zero.)
 ----------------------------------------------------------------------*/

#ifndef TactileFileManager_h
#define TactileFileManager_h 1

#include <SPI.h>
#include <SD.h>

using namespace std;

#include "TactileCPU.h"

class TactileFileManager {

 public:
  TactileFileManager(TactileCPU *tc);

  // The main methods
  const char *getFileName(int fileNum);
  const char *getFileName(int dirNum, int fileNum);
  int         getNumFiles(int dirNum);

 private:
  char _fileNames[NUM_TRACKS][MAX_FILE_NAME];
  char _subDirFileNames[NUM_SUBDIRS][NUM_TRACKS_IN_SUBDIR][MAX_FILE_NAME];
  int  _numSubDirFiles[NUM_SUBDIRS];

  int _readDirIntoStringArray(File *dir, int subDirNum);

  TactileCPU *_tc;
};

#endif
