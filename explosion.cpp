#include "explosion.h"
#include <QPixmap>
#include <QPainter>
#include <QBrush>

Explosion::Explosion(qreal x, qreal y, int maxFrames, int interval)
    : QGraphicsPixmapItem(), frame(0), maxFrame(maxFrames)
{
    setPos(x - 4, y - 4);
    // 使用绘图生成双圈爆炸
    QPixmap pix(48, 48);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QColor(255, 220, 60));
    painter.setPen(QPen(QColor(255, 80, 20), 2));
    painter.drawEllipse(2, 2, 44, 44);
    painter.setBrush(QColor(255, 150, 30, 190));
    painter.setPen(QPen(QColor(255, 255, 255), 2));
    painter.drawEllipse(10, 10, 28, 28);
    setPixmap(pix);
    setZValue(3);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Explosion::nextFrame);
    timer->start(interval);
}

Explosion::~Explosion() {
    timer->stop();
}

void Explosion::nextFrame() {
    frame++;
    if (frame >= maxFrame) {
        timer->stop();
        deleteLater();  // 安全自动销毁
        return;
    }
    // 放大或改变颜色
    qreal scale = 1.0 + frame * 0.15;
    setScale(scale);
    // 透明度衰减
    setOpacity(1.0 - frame / (qreal)maxFrame);
}
