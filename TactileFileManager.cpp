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

#include "TactileCPU.h"
#include "TactileFileManager.h"

TactileFileManager::TactileFileManager(TactileCPU *tc) {
  _tc = tc;

  // Start the SD card. These definitions are specific to the
  // Teensy Audio Shield when mounted on the Teensy 4.1 computer.
#define SDCARD_MOSI_PIN 11
#define SDCARD_SCK_PIN  13
#define SDCARD_CS_PIN   10
  _tc->log2("TactileFileManager: Initializing SD card...");
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!SD.begin(SDCARD_CS_PIN)) {
    Serial.println("SD card initialization failed!");  // always print, even of logging turned off.
    while (1);
  }
  _tc->log2("SD card initialization done.");
  _tc->log2("TactileFileManager: Reading filenames...");

  for (int i = 0; i < NUM_TRACKS; i++)
    _fileNames[i][0] = 0;
  for (int i = 0; i < NUM_SUBDIRS; i++) {
    _numSubDirFiles[i] = 0;
    for (int j = 0; j < NUM_TRACKS_IN_SUBDIR; j++)
      _subDirFileNames[i][j][0] = 0;
  }
  _tc->log2("Name arrays initialized");

  // Find WAV files in the root directory
  File dir = SD.open("/");
  _readDirIntoStringArray(&dir, -1);
  dir.close();

  // Find WAV files in the subdirectories E1-E4
  char dirName[3] = "Ex";
  for (int dirNum = 1; dirNum <= NUM_SUBDIRS; dirNum++) {
    dirName[1] = '0'+dirNum;
    dir = SD.open(dirName);
    if (dir) {
      _numSubDirFiles[dirNum-1] = _readDirIntoStringArray(&dir, dirNum-1);
    } else if (_tc->getLogLevel() > 1) {
      Serial.print("TactileFileManager: Failed to open directory: '");
      Serial.print(dirName);
      Serial.println("'");
    }
  }

  // When detailed logging enabled...
  if (_tc->getLogLevel() > 0) {
    Serial.println("TactileFileManager:: tracks found:");
    for (int i = 0; i < NUM_TRACKS; i++) {
      if (_fileNames[i])
        Serial.println(_fileNames[i]);
    }
    for (int i = 0; i < NUM_SUBDIRS; i++) {
      Serial.print("E");
      Serial.print(i+1);
      Serial.println("/");
      for (int j = 0; j < NUM_TRACKS_IN_SUBDIR; j++) {
        char *name = _subDirFileNames[i][j];
        if (name && strlen(name) > 0) {
          Serial.print("    ");
          Serial.println(name);
        }
      }
    }
  }
}

int TactileFileManager::_readDirIntoStringArray(File *dir, int subDirNum)
{
  if (_tc->getLogLevel() > 1) {
    Serial.print("TactileFileManager::_readDirIntoStringArray(");
    if (!dir)
      Serial.print("NULL-dir");
    else
      Serial.print(dir->name());
    Serial.print(", ");
    Serial.print(subDirNum);
    Serial.println(")");
  }
  File file;
  int fileNum = 0;
  char name[MAX_FILE_NAME+1];
  while (file = dir->openNextFile()) {
    strncpy(name, file.name(), MAX_FILE_NAME);
    int len = strlen(file.name());
    bool isDir = file.isDirectory();
    _tc->logAction2(name, isDir);
    file.close();
    if (len > MAX_FILE_NAME)
      continue;
    if (isDir)
      continue;
    if (   name[0] == '_'
        || name[0] == '.'
        || (strcmp(name + len - 4, ".WAV") != 0 && strcmp(name + len - 4, ".wav") != 0))
      continue;
    if (subDirNum < 0)
      strcpy(_fileNames[fileNum], name);
    else
      strcpy(_subDirFileNames[subDirNum][fileNum], name);
    fileNum++;
    if (fileNum >= NUM_TRACKS)
      break;
  }
  return fileNum;
}


const char *TactileFileManager::getFileName(int fileNum)
{
  if (fileNum < 0 || fileNum >= NUM_TRACKS) {
    _tc->log("TactileFileManager: fileNum out of range");
    return NULL;
  }
  return _fileNames[fileNum];
}


const char *TactileFileManager::getFileName(int dirNum, int fileNum)
{
  if (dirNum < 0 || dirNum >= NUM_SUBDIRS) {
    _tc->log("TactileFileManager: dirNum out of range");
    return NULL;
  }
  if (fileNum < 0 || fileNum >= NUM_TRACKS) {
    _tc->log("TactileFileManager: fileNum out of range");
    return NULL;
  }
  return _subDirFileNames[dirNum][fileNum];
}

int TactileFileManager::getNumFiles(int dirNum) {
  if (dirNum < 0 || dirNum >= NUM_SUBDIRS) {
    _tc->log("TactileFileManager: dirNum out of range");
    return -1;
  }
  return _numSubDirFiles[dirNum];
}
