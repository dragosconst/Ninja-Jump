#include "Arduino.h"
#include "StateMachine.h"

StateMachine::StateMachine(Menu** currentMenu, Player** player, World** world, LedControl* lc, int* diff) : currentMenu(currentMenu), player(player), world(world),
lc(lc), diff(diff) {
    this->state = BrowsingMenus;
}

void StateMachine::updateState() {
    if(this->state == BrowsingMenus && (*this->currentMenu)->isPlaying()) {
        this->state = PlayingGame;
        *this->player = new Player(MAX_LIVES, START_HEIGHT);
        *this->world = new World(this->lc, *this->player, *this->diff);
        (*this->player)->setWorld(*this->world);

        // recreate display menu so it uses the memory addresses of the current player
        Menu* oldMenu = (*this->currentMenu);
        oldMenu->freeOptions();
        delete oldMenu;
        (*this->currentMenu) = createDisplayMenu();
    }
    else if(this->state == PlayingGame && (*this->player)->getLives() <= 0) {
        this->state = BrowsingMenus;
        (*this->currentMenu)->clear();
        Menu* oldMenu = (*this->currentMenu);
        if(RWHelper::getLastHigh() < (*this->player)->getHeight()) {
            (*this->currentMenu) = createCongratulationsMenu();
        }
        else {
            (*this->currentMenu) = createGameOverMenu();
        }
        oldMenu->freeOptions();
        delete oldMenu;

        delete *this->world;
        delete *this->player;
    }
}