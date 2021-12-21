#include "Arduino.h"
#include "SoundsManager.h"

int8_t SoundsManager::ingame_theme = INGAME_THEME_1;
bool SoundsManager::inMenu = true;
bool SoundsManager::soundsOn = true;
DFRobotDFPlayerMini* SoundsManager::player = nullptr;

void SoundsManager::playTheme() {
    if(SoundsManager::soundsOn == false) {
        return;
    }

    if(SoundsManager::inMenu) {
        SoundsManager::player->loop(MENU_THEME);
    }
    else {
        if(SoundsManager::ingame_theme == INGAME_FIRST_THEME) {
            SoundsManager::player->loop(INGAME_THEME_1);
        }
        else {
            SoundsManager::player->loop(INGAME_THEME_2);
        }
    }
}

void SoundsManager::setPlayer(DFRobotDFPlayerMini* player) {
    SoundsManager::player = player;
}

void SoundsManager::setSounds(bool sounds) {
    if(sounds == false && sounds != SoundsManager::soundsOn) {
        SoundsManager::player->pause();
    }
    else if(sounds == true && sounds != SoundsManager::soundsOn) {
        SoundsManager::player->start();
    }
    SoundsManager::soundsOn = sounds;
}

void SoundsManager::changeInGame(int8_t newVal) {
    SoundsManager::ingame_theme = newVal;   
}

void SoundsManager::changeVolume(int8_t vol) {
    SoundsManager::player->volume(vol);
}

void SoundsManager::playSound(int freq, int dur) {
    if(SoundsManager::soundsOn == false) {
        return;
    }
    tone(SFXpin, freq, dur);
}

// void SoundsManager::playJump() {
//     if(SoundsManager::soundsOn == false) {
//         return;
//     }
//     tone(SFXpin, NOTE_FS3, JUMP_DUR);
// }

// void SoundsManager::playCanons() {
//     if(SoundsManager::soundsOn == false) {
//         return;
//     }

//     tone(SFXpin, NOTE_F4, CAN_DUR);
// }

// void SoundsManager::playDisappearing() {
//     if(SoundsManager::soundsOn == false) {
//         return;
//     }

//     tone(SFXpin, NOTE_FS6, DIS_DUR);
// }

// void SoundsManager::playMenuScroll() {
//     if(SoundsManager::soundsOn == false) {
//         return;
//     }

//     tone(SFXpin, NOTE_B5, MENU_DUR);
// }

// void SoundsManager::playMenuClick() {
//     if(SoundsManager::soundsOn == false) {
//         return;
//     }

//     tone(SFXpin, NOTE_DS7, MENU_DUR);
// }