#include "player.h"
#include <QPen>

PlayerTank::PlayerTank(qreal x, qreal y) : Tank(x, y) {
    setData(0, TYPE_PLAYER);
    setPen(QPen(Qt::green, 3));
}