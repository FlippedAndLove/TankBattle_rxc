#include "wall.h"
#include <QPen>
#include <QBrush>
#include <QColor>

Wall::Wall(qreal x, qreal y, qreal width, qreal height)
    : QGraphicsRectItem(x, y, width, height)
{
    setPen(QPen(Qt::red, 1));
    setBrush(QBrush(QColor(255, 140, 0))); // 橙色砖块
    setData(0, TYPE_WALL);
    setZValue(0);
}