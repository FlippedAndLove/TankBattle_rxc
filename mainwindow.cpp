#include "mainwindow.h"
#include <QStatusBar>
#include <QMessageBox>
#include <QKeyEvent>
#include <QRandomGenerator>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QFont>
#include <QBrush>
#include <QPen>
#include <QColor>
#include<QDateTime>
#include <QDebug>
#include"steelwall.h"
#include"explosion.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),player(nullptr),gameLoopTimer(nullptr),lastTime(0),scoreDisplay(nullptr) {
    setWindowTitle("坦克大战 - RXC");
    resize(SCENE_WIDTH + 20, SCENE_HEIGHT + 80);

    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, SCENE_WIDTH, SCENE_HEIGHT);

    view = new QGraphicsView(scene, this);
    view->setRenderHint(QPainter::Antialiasing);
    view->setFixedSize(SCENE_WIDTH, SCENE_HEIGHT);
    setCentralWidget(view);

    statusBar()->showMessage("得分: 0  生命: 3");

    initGame();
    setFocusPolicy(Qt::StrongFocus);
}

MainWindow::~MainWindow() {
    clearGame();
}

// 初始化
void MainWindow::initGame() {
    clearGame();

    score = 0;
    lives = INIT_LIVES;
    shootCooldown = 0;
    gameRunning = true;
    keyUp = keyDown = keyLeft = keyRight = false;
    spacePressed = false;

    initBackground();
    initMap();

    player = new PlayerTank(60, SCENE_HEIGHT - TANK_SIZE - 60);
    scene->addItem(player);

    initEnemies();
    updateUI();

    gameLoopTimer = new QTimer(this);
    connect(gameLoopTimer, &QTimer::timeout, this, &MainWindow::gameLoop);
    gameLoopTimer->start(20);
    //调试输出
    qDebug()<<"砖墙数量："<<walls.size();
    qDebug()<<"敌方坦克数量："<<enemies.size();
}

void MainWindow::clearGame() {
    // 1. 停止并删除所有 AI 定时器
    for (QTimer *t : enemyAITimers) {
        if (t) {           // 防止空指针
            t->stop();
            delete t;
        }
    }
    enemyAITimers.clear();   // 清空列表

    // 2. 停止并删除主循环定时器
    if (gameLoopTimer) {
        gameLoopTimer->stop();
        delete gameLoopTimer;
        gameLoopTimer = nullptr;   // 避免野指针
    }

    // 3. 从场景移除并删除所有砖墙
    for (Wall *w : walls) {
        scene->removeItem(w);
        delete w;
    }
    walls.clear();

    // 4. 从场景移除并删除所有敌方坦克
    for (EnemyTank *e : enemies) {
        scene->removeItem(e);
        delete e;
    }
    enemies.clear();

    // 5. 从场景移除并删除所有子弹
    for (Bullet *b : bullets) {
        scene->removeItem(b);
        delete b;
    }
    bullets.clear();

    // 6. 移除并删除玩家坦克
    if (player) {
        scene->removeItem(player);
        delete player;
        player = nullptr;
    }
    // 清理钢墙
    for (QGraphicsItem *item : terrainItems) {
        scene->removeItem(item);
        delete item;
    }
    terrainItems.clear();
}

//背景（校园风格）
void MainWindow::initBackground() {
    scene->setBackgroundBrush(QBrush(QColor(34, 139, 34))); // 草地

    // 小路
    QGraphicsRectItem *road = new QGraphicsRectItem(0, 300, SCENE_WIDTH, 40);
    road->setPen(QPen(Qt::NoPen));
    road->setBrush(QBrush(QColor(160, 160, 160)));
    road->setZValue(-1);
    scene->addItem(road);

    // 花坛
    QGraphicsEllipseItem *flower1 = new QGraphicsEllipseItem(80, 480, 50, 50);
    flower1->setPen(QPen(Qt::NoPen));
    flower1->setBrush(QBrush(QColor(255, 182, 193)));
    flower1->setZValue(-1);
    scene->addItem(flower1);

    QGraphicsEllipseItem *flower2 = new QGraphicsEllipseItem(130, 470, 40, 40);
    flower2->setPen(QPen(Qt::NoPen));
    flower2->setBrush(QBrush(QColor(255, 105, 180)));
    flower2->setZValue(-1);
    scene->addItem(flower2);

    QGraphicsEllipseItem *flower3 = new QGraphicsEllipseItem(600, 100, 60, 60);
    flower3->setPen(QPen(Qt::NoPen));
    flower3->setBrush(QBrush(QColor(255, 215, 0)));
    flower3->setZValue(-1);
    scene->addItem(flower3);

    // 教学楼
    QGraphicsRectItem *building = new QGraphicsRectItem(680, 50, 100, 200);
    building->setPen(QPen(Qt::black, 1));
    building->setBrush(QBrush(QColor(200, 180, 150)));
    building->setZValue(-1);
    scene->addItem(building);
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 2; ++j) {
            QGraphicsRectItem *window = new QGraphicsRectItem(690 + j*40, 70 + i*50, 20, 20);
            window->setPen(QPen(Qt::blue, 1));
            window->setBrush(QBrush(QColor(135, 206, 250)));
            window->setZValue(-1);
            scene->addItem(window);
        }
    }

    // 右上角 "RXC" 文字标识
    idText = scene->addText("RXC");
    idText->setPos(SCENE_WIDTH - 130, 20);
    QFont font("Arial", 32, QFont::Bold);
    idText->setFont(font);
    idText->setDefaultTextColor(QColor(255, 255, 0));
    idText->setZValue(10);
    //得分和生命值的视觉展示
    QGraphicsTextItem *scoreDisplay = scene->addText("得分: 0");
    scoreDisplay->setPos(20, 20);
    scoreDisplay->setDefaultTextColor(Qt::white);
    scoreDisplay->setZValue(10);
}


// 敌方坦克
void MainWindow::initEnemies() {
    for (int i = 0; i < 3; ++i) {
        qreal x = SCENE_WIDTH - TANK_SIZE - 40 - i * 60;
        qreal y = 40 + i * 30;
        EnemyTank *enemy = new EnemyTank(x, y);
        scene->addItem(enemy);
        enemies.append(enemy);

        QTimer *aiTimer = new QTimer(this);
        aiTimer->start(400 + QRandomGenerator::global()->bounded(400));
        connect(aiTimer, &QTimer::timeout, [this, enemy]() {
            if (!gameRunning || !enemy->isAlive()) return;
            if (QRandomGenerator::global()->bounded(4) == 0) {
                int dirs[] = {Qt::Key_Up, Qt::Key_Down, Qt::Key_Left, Qt::Key_Right};
                enemy->setDirection(dirs[QRandomGenerator::global()->bounded(4)]);
            }
            if (QRandomGenerator::global()->bounded(5) == 0) {
                fireBullet(enemy);
            }
        });
        enemyAITimers.append(aiTimer);
    }
}
//UI更新
void MainWindow::updateUI() {
    statusBar()->showMessage(QString("得分: %1  生命: %2").arg(score).arg(lives));
    if(scoreDisplay){
        scoreDisplay->setPlainText(QString("得分：%1").arg(score));
    }
}

// 开火
void MainWindow::fireBullet(Tank *tank) {
    if (!gameRunning) return;
    if (tank->data(0).toInt() == TYPE_PLAYER) {
        if (shootCooldown > 0) return;
        shootCooldown = 12;
    }
    QPointF muzzle = tank->getMuzzlePos();
    Bullet *bullet = new Bullet(tank->getDirection(), muzzle.x(), muzzle.y());
    scene->addItem(bullet);
    bullets.append(bullet);
}

// 移动坦克（含碰撞）
void MainWindow::moveTank(Tank *tank, qreal dx, qreal dy) {
    if (!tank->isAlive()) return;
    qreal oldX = tank->x(), oldY = tank->y();
    tank->setPos(oldX + dx, oldY + dy);

    if (tank->x() < 0 || tank->x() > SCENE_WIDTH - TANK_SIZE ||
        tank->y() < 0 || tank->y() > SCENE_HEIGHT - TANK_SIZE) {
        tank->setPos(oldX, oldY);
        return;
    }

    QList<QGraphicsItem*> colliding = tank->collidingItems();
    for (QGraphicsItem *item : colliding) {
        int type = item->data(0).toInt();
        if (type == TYPE_WALL || type == TYPE_STEEL_WALL || type == TYPE_ENEMY || type == TYPE_PLAYER) {
            tank->setPos(oldX, oldY);
            break;
        }
    }
}

//游戏主循环
void MainWindow::gameLoop() {
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    float deltaTime = (lastTime == 0) ? 0.02f : (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;
    // 限制最大步长，防止卡顿时瞬移
    if (deltaTime > 0.05f) deltaTime = 0.05f;
    if (!gameRunning) return;

    // 玩家移动
    qreal dx = 0, dy = 0;
    if (keyUp)    { dy = -PLAYER_SPEED*deltaTime; player->setDirection(Qt::Key_Up); }
    else if (keyDown)  { dy =  PLAYER_SPEED*deltaTime; player->setDirection(Qt::Key_Down); }
    else if (keyLeft)  { dx = -PLAYER_SPEED*deltaTime; player->setDirection(Qt::Key_Left); }
    else if (keyRight) { dx =  PLAYER_SPEED*deltaTime; player->setDirection(Qt::Key_Right); }
    if (dx || dy) moveTank(player, dx, dy);
    if (spacePressed) fireBullet(player);

    // 敌方移动
    for (EnemyTank *enemy : enemies) {
        if (!enemy->isAlive()) continue;
        enemy->updateAI(player->pos());
        qreal edx = 0, edy = 0;
        switch(enemy->getDirection()) {
        case Qt::Key_Up:    edy = -ENEMY_SPEED*deltaTime; break;
        case Qt::Key_Down:  edy =  ENEMY_SPEED*deltaTime; break;
        case Qt::Key_Left:  edx = -ENEMY_SPEED*deltaTime; break;
        case Qt::Key_Right: edx =  ENEMY_SPEED*deltaTime; break;
        }
        moveTank(enemy, edx, edy);
    }

    // 子弹移动及碰撞
    for (int i = bullets.size() - 1; i >= 0; --i) {
        Bullet *bullet = bullets[i];
        bullet->move(deltaTime);

        if (bullet->x() < 0 || bullet->x() > SCENE_WIDTH ||
            bullet->y() < 0 || bullet->y() > SCENE_HEIGHT) {
            scene->removeItem(bullet);
            bullets.removeAt(i);
            delete bullet;
            continue;
        }

        QList<QGraphicsItem*> colliding = bullet->collidingItems();
        bool removed = false;
        for (QGraphicsItem *item : colliding) {
            int type = item->data(0).toInt();
            if (type == TYPE_WALL) {
                Wall *wall = static_cast<Wall*>(item);
                scene->removeItem(wall);
                walls.removeOne(wall);
                delete wall;
                scene->removeItem(bullet);
                bullets.removeAt(i);
                delete bullet;
                removed = true;
                break;
            }
            else if (type == TYPE_ENEMY) {
                EnemyTank *enemy = static_cast<EnemyTank*>(item);
                if (!enemy->isAlive()) continue;
                enemy->setAlive(false);
                scene->removeItem(enemy);
                //创建爆炸特效
                Explosion*exp=new Explosion(enemy->x(),enemy->y());
                scene->addItem(exp);
                score += 10;
                updateUI();
                scene->removeItem(bullet);
                bullets.removeAt(i);
                delete bullet;
                removed = true;
                bool allDead=true;
                for(EnemyTank*e:enemies){
                    if(e->isAlive()){
                        allDead=false;
                        break;
                    }
                }
                if(allDead)gameOver(true);
                break;

            }
            else if (type == TYPE_PLAYER) {
                if (!player->isAlive()) continue;
                Explosion*exp=new Explosion(player->x(),player->y());
                scene->addItem(exp);
                lives--;
                updateUI();
                scene->removeItem(bullet);
                bullets.removeAt(i);
                delete bullet;
                removed = true;
                if (lives <= 0) {
                    player->setAlive(false);
                    gameOver(false);
                }
                break;
            }
            else if (type == TYPE_STEEL_WALL) {
                // 钢墙不可摧毁，但子弹应消失
                scene->removeItem(bullet);
                bullets.removeAt(i);
                delete bullet;
                removed = true;
                break;
            }
        }
        if (removed) continue;
    }

    if (shootCooldown > 0) shootCooldown--;
}

// 游戏结束
void MainWindow::gameOver(bool win) {
    gameRunning = false;
    gameLoopTimer->stop();
    for (QTimer *t : enemyAITimers) t->stop();

    QString title = win ? "🎉 胜利！" : "💀 游戏结束";
    QString msg = QString("%1\n最终得分: %2\n按 R 键重新开始")
                      .arg(win ? "恭喜你消灭了所有敌人！" : "你的坦克被摧毁了！")
                      .arg(score);
    QMessageBox::information(this, title, msg, QMessageBox::Ok);
}

void MainWindow::resetGame() {
    clearGame();
    initGame();
    updateUI();
}

// 键盘事件
void MainWindow::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
    case Qt::Key_Up:    keyUp = true; break;
    case Qt::Key_Down:  keyDown = true; break;
    case Qt::Key_Left:  keyLeft = true; break;
    case Qt::Key_Right: keyRight = true; break;
    case Qt::Key_Space: spacePressed = true; break;
    case Qt::Key_R:
        if (!gameRunning) resetGame();
        break;
    default: QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    switch(event->key()) {
    case Qt::Key_Up:    keyUp = false; break;
    case Qt::Key_Down:  keyDown = false; break;
    case Qt::Key_Left:  keyLeft = false; break;
    case Qt::Key_Right: keyRight = false; break;
    case Qt::Key_Space: spacePressed = false; break;
    default: QMainWindow::keyReleaseEvent(event);
    }
}

//初始化地图
void MainWindow::initMap() {
    // 清空之前的地形
    for (QGraphicsItem *item : terrainItems) {
        scene->removeItem(item);
        delete item;
    }
    terrainItems.clear();

    // 1. 填充地图：0为空地，1=砖墙，2=钢墙
    // 先将全部置0
    for (int r=0; r<MAP_ROWS; ++r)
        for (int c=0; c<MAP_COLS; ++c)
            mapData[r][c] = 0;

    // 2. 绘制边界（最外一圈为钢墙）
    for (int r=0; r<MAP_ROWS; ++r) {
        mapData[r][0] = 2;
        mapData[r][MAP_COLS-1] = 2;
    }
    for (int c=0; c<MAP_COLS; ++c) {
        mapData[0][c] = 2;
        mapData[MAP_ROWS-1][c] = 2;
    }

    // 3. 在中间区域放置RXC字母（砖墙），以及一些钢墙掩体
    int baseCol = 4, baseRow = 3;
    // R
    int rShape[7][5] = {
        {1,1,1,1,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,1,1,1,1},
        {1,0,1,0,0},
        {1,0,0,1,0},
        {1,0,0,0,1}
    };
    for (int i=0;i<7;++i)
        for (int j=0;j<5;++j)
            if (rShape[i][j]) mapData[baseRow+i][baseCol+j] = 1;

    // X
    int xShape[5][5] = {
        {1,0,0,0,1},
        {0,1,0,1,0},
        {0,0,1,0,0},
        {0,1,0,1,0},
        {1,0,0,0,1}
    };
    int xCol = baseCol + 5 + 1; // gap 30/40≈1
    for (int i=0;i<5;++i)
        for (int j=0;j<5;++j)
            if (xShape[i][j]) mapData[baseRow+1+i][xCol+j] = 1;  // 行偏移1

    // C
    int cShape[5][5] = {
        {0,1,1,1,0},
        {1,0,0,0,0},
        {1,0,0,0,0},
        {1,0,0,0,0},
        {0,1,1,1,0}
    };
    int cCol = xCol + 5 + 1;
    for (int i=0;i<5;++i)
        for (int j=0;j<5;++j)
            if (cShape[i][j]) mapData[baseRow+1+i][cCol+j] = 1;

    // 4. 额外添加一些钢墙掩体（例如在左上角、右下角）
    mapData[2][2] = 2;
    mapData[2][3] = 2;
    mapData[3][2] = 2;
    mapData[MAP_ROWS-3][MAP_COLS-3] = 2;
    mapData[MAP_ROWS-3][MAP_COLS-2] = 2;
    mapData[MAP_ROWS-2][MAP_COLS-3] = 2;

    // 5. 根据mapData生成对应的图形项
    int cellSize = SCENE_WIDTH / MAP_COLS;  // 40
    for (int r=0; r<MAP_ROWS; ++r) {
        for (int c=0; c<MAP_COLS; ++c) {
            qreal x = c * cellSize;
            qreal y = r * cellSize;
            int type = mapData[r][c];
            QGraphicsItem *item = nullptr;
            if (type == 1) {  // 砖墙
                Wall *wall = new Wall(x, y, cellSize, cellSize);
                scene->addItem(wall);
                walls.append(wall);  // 仍加入walls列表，便于被子弹摧毁
            } else if (type == 2) {  // 钢墙
                SteelWall *steel = new SteelWall(x, y, cellSize, cellSize);
                scene->addItem(steel);
                terrainItems.append(steel);
            }
        }
    }
}