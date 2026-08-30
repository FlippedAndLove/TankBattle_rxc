#include "tank.h"
#include <QPainter>
#include <QStyleOption>

Tank::Tank(qreal x, qreal y)
    : QGraphicsRectItem(x, y, TANK_SIZE, TANK_SIZE)
{
    direction = Qt::Key_Up;
    alive = true;
    setPen(QPen(Qt::black, 2));
    setZValue(1);
}

void Tank::setDirection(int dir) { direction = dir; }
int Tank::getDirection() const { return direction; }
bool Tank::isAlive() const { return alive; }
void Tank::setAlive(bool a) { alive = a; }

void Tank::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // 车身
    painter->setBrush(QColor(80, 80, 80));
    painter->drawRect(0, 0, TANK_SIZE, TANK_SIZE);

    // 履带
    painter->setBrush(QColor(50, 50, 50));
    painter->drawRect(2, 2, 6, TANK_SIZE - 4);
    painter->drawRect(TANK_SIZE - 8, 2, 6, TANK_SIZE - 4);

    // 炮塔
    painter->setBrush(QColor(100, 100, 100));
    painter->drawEllipse(8, 8, TANK_SIZE - 16, TANK_SIZE - 16);

    // 炮管
    painter->setBrush(QColor(60, 60, 60));
    int barrelLen = 14, barrelWid = 4;
    switch(direction) {
    case Qt::Key_Up:
        painter->drawRect((TANK_SIZE - barrelWid)/2, -barrelLen + 4, barrelWid, barrelLen);
        break;
    case Qt::Key_Down:
        painter->drawRect((TANK_SIZE - barrelWid)/2, TANK_SIZE - 4, barrelWid, barrelLen);
        break;
    case Qt::Key_Left:
        painter->drawRect(-barrelLen + 4, (TANK_SIZE - barrelWid)/2, barrelLen, barrelWid);
        break;
    case Qt::Key_Right:
        painter->drawRect(TANK_SIZE - 4, (TANK_SIZE - barrelWid)/2, barrelLen, barrelWid);
        break;
    }

    // 玩家标记（红色圆点）
    if (data(0).toInt() == TYPE_PLAYER) {
        painter->setBrush(Qt::red);
        painter->drawEllipse((TANK_SIZE - 10)/2, (TANK_SIZE - 10)/2, 10, 10);
    }
}

QPointF Tank::getMuzzlePos() const {
    qreal cx = x() + TANK_SIZE / 2;
    qreal cy = y() + TANK_SIZE / 2;
    switch(direction) {
    case Qt::Key_Up:    return QPointF(cx - 2, y() - 8);
    case Qt::Key_Down:  return QPointF(cx - 2, y() + TANK_SIZE - 8);
    case Qt::Key_Left:  return QPointF(x() - 8, cy - 2);
    case Qt::Key_Right: return QPointF(x() + TANK_SIZE - 8, cy - 2);
    }
    return QPointF(cx, cy);
}