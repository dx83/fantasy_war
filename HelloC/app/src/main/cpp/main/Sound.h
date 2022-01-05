#pragma once

#include "iStd.h"

void loadSound();
void playBgmSound(int index);
void playSfxSound(int index);
void stopBgmSound(int index);
void stopSfxSound(int index);
void volumeBgmSound(float volume);
void volumeSfxSound(float volume);
void pauseBgmSound(bool pause);
