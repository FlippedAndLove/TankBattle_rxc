#include "tank.h"
#include <QPainter>
#include <QStyleOption>

Tank::Tank(qreal x, qreal y)
    : QGraphicsRectItem(0, 0, TANK_SIZE, TANK_SIZE)
{
    setPos(x, y);
    direction = Qt::Key_Up;
    alive = true;
    setPen(QPen(Qt::black, 2));
    setZValue(1);
}

void Tank::setDirection(int dir) { direction = dir; }
int Tank::getDirection() const { return direction; }
bool Tank::isAlive() const { return alive; }
void Tank::setAlive(bool a) { alive = a; }

QRectF Tank::boundingRect() const {
    return rect().adjusted(-16, -16, 16, 16);
}

QPainterPath Tank::shape() const {
    QPainterPath path;
    path.addRect(rect());
    return path;
}

void Tank::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QColor bodyColor = brush().color();
    if (!bodyColor.isValid() || bodyColor == Qt::transparent) {
        bodyColor = QColor(80, 80, 80);
    }

    const QColor dark = QColor(20, 20, 20);

    // 车身
    painter->setBrush(bodyColor);
    painter->setPen(QPen(dark, 2));
    painter->drawRect(0, 0, TANK_SIZE, TANK_SIZE);

    // 左右履带
    painter->setBrush(QColor(38, 38, 38));
    painter->setPen(Qt::NoPen);
    painter->drawRect(2, 2, 8, TANK_SIZE - 4);
    painter->drawRect(TANK_SIZE - 10, 2, 8, TANK_SIZE - 4);

    // 履带齿纹
    painter->setBrush(QColor(96, 96, 96));
    for (int y = 5; y < TANK_SIZE - 4; y += 6) {
        painter->drawRect(4, y, 4, 2);
        painter->drawRect(TANK_SIZE - 8, y, 4, 2);
    }

    // 车首装甲
    painter->setBrush(bodyColor.darker(125));
    painter->setPen(Qt::NoPen);
    switch(direction) {
    case Qt::Key_Up:    painter->drawRect(10, 0, TANK_SIZE - 20, 8); break;
    case Qt::Key_Down:  painter->drawRect(10, TANK_SIZE - 8, TANK_SIZE - 20, 8); break;
    case Qt::Key_Left:  painter->drawRect(0, 10, 8, TANK_SIZE - 20); break;
    case Qt::Key_Right: painter->drawRect(TANK_SIZE - 8, 10, 8, TANK_SIZE - 20); break;
    }

    // 炮塔
    painter->setBrush(bodyColor.lighter(135));
    painter->setPen(QPen(dark, 2));
    painter->drawEllipse(9, 9, TANK_SIZE - 18, TANK_SIZE - 18);

    // 炮管
    const int barrelLen = 16;
    const int barrelWid = 5;
    painter->setBrush(QColor(45, 45, 45));
    painter->setPen(QPen(QColor(15, 15, 15), 1));
    switch(direction) {
    case Qt::Key_Up:
        painter->drawRect((TANK_SIZE - barrelWid) / 2, -barrelLen, barrelWid, barrelLen + 8);
        break;
    case Qt::Key_Down:
        painter->drawRect((TANK_SIZE - barrelWid) / 2, TANK_SIZE - 8, barrelWid, barrelLen);
        break;
    case Qt::Key_Left:
        painter->drawRect(-barrelLen, (TANK_SIZE - barrelWid) / 2, barrelLen + 8, barrelWid);
        break;
    case Qt::Key_Right:
        painter->drawRect(TANK_SIZE - 8, (TANK_SIZE - barrelWid) / 2, barrelLen, barrelWid);
        break;
    }

    // 炮口制退器
    painter->setBrush(QColor(30, 30, 30));
    switch(direction) {
    case Qt::Key_Up:
        painter->drawRect((TANK_SIZE - barrelWid) / 2 - 1, -barrelLen - 3, barrelWid + 2, 3);
        break;
    case Qt::Key_Down:
        painter->drawRect((TANK_SIZE - barrelWid) / 2 - 1, TANK_SIZE + barrelLen - 8, barrelWid + 2, 3);
        break;
    case Qt::Key_Left:
        painter->drawRect(-barrelLen - 3, (TANK_SIZE - barrelWid) / 2 - 1, 3, barrelWid + 2);
        break;
    case Qt::Key_Right:
        painter->drawRect(TANK_SIZE + barrelLen - 8, (TANK_SIZE - barrelWid) / 2 - 1, 3, barrelWid + 2);
        break;
    }

    // 中心识别点：玩家为白点，敌方为深色点
    painter->setPen(Qt::NoPen);
    if (data(0).toInt() == TYPE_PLAYER) {
        painter->setBrush(QColor(255, 255, 255, 230));
    } else {
        painter->setBrush(QColor(25, 25, 25, 200));
    }
    painter->drawEllipse((TANK_SIZE - 8) / 2, (TANK_SIZE - 8) / 2, 8, 8);
}

QPointF Tank::getMuzzlePos() const {
    qreal cx = x() + TANK_SIZE / 2;
    qreal cy = y() + TANK_SIZE / 2;
    switch(direction) {
    case Qt::Key_Up:    return QPointF(cx - BULLET_SIZE / 2, y() - BULLET_SIZE - 2);
    case Qt::Key_Down:  return QPointF(cx - BULLET_SIZE / 2, y() + TANK_SIZE + 2);
    case Qt::Key_Left:  return QPointF(x() - BULLET_SIZE - 2, cy - BULLET_SIZE / 2);
    case Qt::Key_Right: return QPointF(x() + TANK_SIZE + 2, cy - BULLET_SIZE / 2);
    }
    return QPointF(cx - BULLET_SIZE / 2, cy - BULLET_SIZE / 2);
}
