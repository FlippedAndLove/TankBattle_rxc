#include "steelwall.h"
#include <QPen>
#include <QBrush>

SteelWall::SteelWall(qreal x, qreal y, qreal width, qreal height)
    : QGraphicsRectItem(x, y, width, height)
{
    setPen(QPen(Qt::gray, 2));
    setBrush(QBrush(Qt::lightGray));
    setData(0, TYPE_STEEL_WALL);
    setZValue(0);
}