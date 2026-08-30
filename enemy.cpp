#include "enemy.h"
#include <QRandomGenerator>
#include <QPen>
#include<QPointF>

EnemyTank::EnemyTank(qreal x, qreal y) : Tank(x, y) {
    setData(0, TYPE_ENEMY);
    setPen(QPen(Qt::red, 2));
    setBrush(QBrush(QColor(200, 50, 50)));
    int dirs[] = {Qt::Key_Up, Qt::Key_Down, Qt::Key_Left, Qt::Key_Right};
    direction = dirs[QRandomGenerator::global()->bounded(4)];
}
void EnemyTank::updateAI(const QPointF &playerPos) {
    if (QRandomGenerator::global()->bounded(100) < 30) {
        QPointF dir = playerPos - pos();
        if (qAbs(dir.x()) > qAbs(dir.y())) {
            setDirection(dir.x() > 0 ? Qt::Key_Right : Qt::Key_Left);
        } else {
            setDirection(dir.y() > 0 ? Qt::Key_Down : Qt::Key_Up);
        }
    }
}