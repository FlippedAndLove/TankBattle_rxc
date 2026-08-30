#include "enemy.h"
#include <QRandomGenerator>
#include <QPen>

EnemyTank::EnemyTank(qreal x, qreal y) : Tank(x, y) {
    setData(0, TYPE_ENEMY);
    setPen(QPen(Qt::red, 2));
    int dirs[] = {Qt::Key_Up, Qt::Key_Down, Qt::Key_Left, Qt::Key_Right};
    direction = dirs[QRandomGenerator::global()->bounded(4)];
}