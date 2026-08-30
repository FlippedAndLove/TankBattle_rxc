#ifndef PLAYER_H
#define PLAYER_H

#include "tank.h"

class PlayerTank : public Tank {
public:
    PlayerTank(qreal x, qreal y);
};

#endif