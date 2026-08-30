#ifndef EXPLOSION_H
#define EXPLOSION_H
#include<QObject>
#include <QGraphicsPixmapItem>
#include <QTimer>

class Explosion :public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
private:
    QTimer *timer;
    int frame;
    int maxFrame;
public:
    Explosion(qreal x, qreal y, int maxFrames = 8, int interval = 80);
    ~Explosion();
private slots:
    void nextFrame();
};
#endif