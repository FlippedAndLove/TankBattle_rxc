#include "mainwindow.h"
#include <QStatusBar>
#include <QMessageBox>
#include <QKeyEvent>
#include <QRandomGenerator>
#include <QFontMetrics>
#include <QSettings>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QFont>
#include <QBrush>
#include <QPen>
#include <QColor>
#include"steelwall.h"
#include"explosion.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), player(nullptr), highScore(0), bestWave(0), combo(0),
      paused(false), muted(false), ambiencePlaying(false), enginePlaying(false),
      pauseStartMs(0), clockOffsetMs(0),
      waveNumber(0), enemiesToSpawn(0), enemiesAliveInWave(0),
      nextEnemySpawnMs(0), waveBreakUntilMs(0), playerRespawnMs(0),
      invincibleUntilMs(0), lastBlinkMs(0), idText(nullptr), scoreDisplay(nullptr),
      livesDisplay(nullptr), enemyDisplay(nullptr), waveDisplay(nullptr), pauseText(nullptr),
      gameOverOverlay(nullptr), gameOverText(nullptr),
      gameLoopTimer(nullptr), lastFrameMs(0), lastHudUpdateMs(0),
      shootSound(nullptr), explosionSound(nullptr), waveSound(nullptr), gameOverSound(nullptr),
      playerHitSound(nullptr), milestoneSound(nullptr), respawnSound(nullptr), pauseSound(nullptr),
      engineSound(nullptr), ambienceSound(nullptr) {
    setWindowTitle("坦克大战 - RXC");
    resize(SCENE_WIDTH + 20, SCENE_HEIGHT + 80);

    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, SCENE_WIDTH, SCENE_HEIGHT);

    view = new QGraphicsView(scene, this);
    view->setRenderHint(QPainter::Antialiasing);
    view->setFixedSize(SCENE_WIDTH, SCENE_HEIGHT);
    setCentralWidget(view);

    statusBar()->showMessage("得分: 0  生命: 3");

    view->setFocusPolicy(Qt::NoFocus);
    setFocus();

    shootSound = new QSoundEffect(this);
    shootSound->setSource(QUrl("qrc:/sounds/shoot.wav"));
    shootSound->setVolume(0.45);
    explosionSound = new QSoundEffect(this);
    explosionSound->setSource(QUrl("qrc:/sounds/explosion.wav"));
    explosionSound->setVolume(0.7);
    waveSound = new QSoundEffect(this);
    waveSound->setSource(QUrl("qrc:/sounds/wave.wav"));
    waveSound->setVolume(0.6);
    gameOverSound = new QSoundEffect(this);
    gameOverSound->setSource(QUrl("qrc:/sounds/gameover.wav"));
    gameOverSound->setVolume(0.7);
    playerHitSound = new QSoundEffect(this);
    playerHitSound->setSource(QUrl("qrc:/sounds/playerhit.wav"));
    playerHitSound->setVolume(0.7);
    milestoneSound = new QSoundEffect(this);
    milestoneSound->setSource(QUrl("qrc:/sounds/milestone.wav"));
    milestoneSound->setVolume(0.7);
    respawnSound = new QSoundEffect(this);
    respawnSound->setSource(QUrl("qrc:/sounds/respawn.wav"));
    respawnSound->setVolume(0.5);
    pauseSound = new QSoundEffect(this);
    pauseSound->setSource(QUrl("qrc:/sounds/pause.wav"));
    pauseSound->setVolume(0.4);
    engineSound = new QSoundEffect(this);
    engineSound->setSource(QUrl("qrc:/sounds/engine.wav"));
    engineSound->setVolume(0.16);
    engineSound->setLoopCount(QSoundEffect::Infinite);
    ambienceSound = new QSoundEffect(this);
    ambienceSound->setSource(QUrl("qrc:/sounds/ambience.wav"));
    ambienceSound->setVolume(0.12);
    ambienceSound->setLoopCount(QSoundEffect::Infinite);

    initGame();
    statusBar()->showMessage("方向键/WASD移动，空格射击；P暂停，M静音，R重开");
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
    combo = 0;
    ambiencePlaying = false;
    enginePlaying = false;
    paused = false;
    clockOffsetMs = 0;
    waveNumber = 0;
    enemiesToSpawn = 0;
    enemiesAliveInWave = 0;
    nextEnemySpawnMs = 0;
    waveBreakUntilMs = 0;
    playerRespawnMs = 0;
    invincibleUntilMs = 0;
    lastBlinkMs = 0;
    keyUp = keyDown = keyLeft = keyRight = false;
    spacePressed = false;
    frameClock.start();
    lastFrameMs = 0;
    lastHudUpdateMs = 0;
    QSettings settings("RXC", "TankBattle");
    highScore = settings.value("highScore", 0).toInt();
    bestWave = settings.value("bestWave", 0).toInt();

    initBackground();
    initMap();

    player = new PlayerTank(160, 500);
    scene->addItem(player);

    startWave(1);
    updateUI();
    if (!muted && !ambiencePlaying) {
        ambienceSound->play();
        ambiencePlaying = true;
    }

    gameLoopTimer = new QTimer(this);
    connect(gameLoopTimer, &QTimer::timeout, this, &MainWindow::gameLoop);
    gameLoopTimer->start(16);
    // 游戏开始：显示操作提示
    QGraphicsTextItem *hint = new QGraphicsTextItem("WASD/方向键移动，空格开火；P 暂停，M 静音");
    hint->setPos(SCENE_WIDTH/2 - 200, SCENE_HEIGHT/2 - 20);
    hint->setDefaultTextColor(Qt::white);
    QFont hintFont("Arial", 18, QFont::Bold);
    hint->setFont(hintFont);
    hint->setZValue(10);
    scene->addItem(hint);
    // 3秒后淡出消失
    QTimer::singleShot(3000, [hint]() {
        hint->deleteLater();
    });
}

void MainWindow::clearGame() {
    // 1. 停止并删除主循环定时器
    if (gameLoopTimer) {
        gameLoopTimer->stop();
        delete gameLoopTimer;
        gameLoopTimer = nullptr;   // 避免野指针
    }
    if (engineSound && enginePlaying) {
        engineSound->stop();
        enginePlaying = false;
    }

    // 2. 从场景移除并删除所有砖墙
    for (Wall *w : walls) {
        scene->removeItem(w);
        delete w;
    }
    walls.clear();

    // 3. 从场景移除并删除所有敌方坦克
    for (EnemyTank *e : enemies) {
        scene->removeItem(e);
        delete e;
    }
    enemies.clear();

    // 4. 从场景移除并删除所有子弹
    for (Bullet *b : bullets) {
        scene->removeItem(b);
        delete b;
    }
    bullets.clear();

    // 5. 移除并删除玩家坦克
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

    // 6. 移除并删除背景装饰和 HUD
    for (QGraphicsItem *item : backgroundItems) {
        scene->removeItem(item);
        delete item;
    }
    backgroundItems.clear();
    for (QGraphicsItem *item : decorItems) {
        scene->removeItem(item);
        delete item;
    }
    decorItems.clear();
    scoreDisplay = nullptr;
    livesDisplay = nullptr;
    enemyDisplay = nullptr;
    waveDisplay = nullptr;
    idText = nullptr;
    if (pauseText) {
        scene->removeItem(pauseText);
        delete pauseText;
        pauseText = nullptr;
    }
    if (gameOverOverlay) {
        scene->removeItem(gameOverOverlay);
        delete gameOverOverlay;
        gameOverOverlay = nullptr;
    }
    if (gameOverText) {
        scene->removeItem(gameOverText);
        delete gameOverText;
        gameOverText = nullptr;
    }
}

// 背景（经典战场）
void MainWindow::initBackground() {
    scene->setBackgroundBrush(QBrush(QColor(86, 74, 56))); // 泥地

    // RXC 背景水印（不参与碰撞）
    QGraphicsTextItem *bgText = scene->addText("RXC");
    bgText->setPos(210, 200);
    QFont bgFont("Arial", 150, QFont::Bold);
    bgText->setFont(bgFont);
    bgText->setDefaultTextColor(QColor(255, 255, 255, 26));
    bgText->setZValue(-2);
    backgroundItems.append(bgText);

    // 右上角 "RXC" 文字标识
    idText = scene->addText("RXC");
    idText->setPos(SCENE_WIDTH - 130, 8);
    QFont logoFont("Arial", 32, QFont::Bold);
    idText->setFont(logoFont);
    idText->setDefaultTextColor(QColor(255, 255, 0));
    idText->setZValue(10);
    backgroundItems.append(idText);

    // 场景 HUD
    QFont hudFont("Arial", 14, QFont::Bold);
    scoreDisplay = scene->addText("得分: 0");
    scoreDisplay->setPos(16, 8);
    scoreDisplay->setFont(hudFont);
    scoreDisplay->setDefaultTextColor(Qt::white);
    scoreDisplay->setZValue(10);
    backgroundItems.append(scoreDisplay);

    livesDisplay = scene->addText("生命: 3");
    livesDisplay->setPos(150, 8);
    livesDisplay->setFont(hudFont);
    livesDisplay->setDefaultTextColor(Qt::white);
    livesDisplay->setZValue(10);
    backgroundItems.append(livesDisplay);

    enemyDisplay = scene->addText("敌人: 3");
    enemyDisplay->setPos(430, 8);
    enemyDisplay->setFont(hudFont);
    enemyDisplay->setDefaultTextColor(Qt::white);
    enemyDisplay->setZValue(10);
    backgroundItems.append(enemyDisplay);

    waveDisplay = scene->addText("波次: 1");
    waveDisplay->setPos(290, 8);
    waveDisplay->setFont(hudFont);
    waveDisplay->setDefaultTextColor(Qt::white);
    waveDisplay->setZValue(10);
    backgroundItems.append(waveDisplay);
}

void MainWindow::startWave(int wave) {
    waveNumber = wave;
    enemiesToSpawn = qMin(3 + (waveNumber - 1), 8);
    enemiesAliveInWave = 0;
    waveBreakUntilMs = 0;
    qint64 nowMs = frameClock.elapsed() - clockOffsetMs;
    nextEnemySpawnMs = nowMs + 600;
    applyWaveBackground(waveNumber);
    QString title;
    if (waveNumber >= 15) title = "王牌";
    else if (waveNumber >= 10) title = "老兵";
    else if (waveNumber >= 5) title = "新兵";
    QString bannerText = QString("第 %1 波").arg(waveNumber);
    if (!title.isEmpty()) bannerText += QString(" · %1").arg(title);
    showWaveBanner(bannerText, 1600);
    if (waveNumber % 5 == 0) {
        playSound(milestoneSound);
    } else {
        playSound(waveSound);
    }
    updateUI();
}

void MainWindow::applyWaveBackground(int wave) {
    for (QGraphicsItem *item : decorItems) {
        scene->removeItem(item);
        delete item;
    }
    decorItems.clear();

    int theme = (wave - 1) % 4;
    QColor ground, crater, track;
    bool night = false;
    switch (theme) {
    case 0:
        ground = QColor(108, 82, 52);
        crater = QColor(62, 48, 34);
        track = QColor(56, 44, 32);
        break;
    case 1:
        ground = QColor(34, 40, 56);
        crater = QColor(16, 20, 32);
        track = QColor(22, 26, 38);
        night = true;
        break;
    case 2:
        ground = QColor(96, 104, 72);
        crater = QColor(50, 56, 40);
        track = QColor(46, 52, 38);
        break;
    default:
        ground = QColor(88, 52, 46);
        crater = QColor(46, 28, 26);
        track = QColor(42, 28, 24);
        break;
    }
    scene->setBackgroundBrush(QBrush(ground));

    struct Crater { qreal x, y, w, h; };
    const Crater craters[4] = {
        {70, 110, 56, 40}, {640, 380, 70, 46},
        {350, 80, 46, 34}, {720, 260, 52, 38}
    };
    for (const Crater &c : craters) {
        QGraphicsEllipseItem *e = new QGraphicsEllipseItem(c.x, c.y, c.w, c.h);
        e->setPen(QPen(Qt::NoPen));
        e->setBrush(crater);
        e->setZValue(-1);
        scene->addItem(e);
        decorItems.append(e);
    }

    QGraphicsRectItem *t1 = new QGraphicsRectItem(90, 520, 190, 8);
    t1->setPen(QPen(Qt::NoPen));
    t1->setBrush(track);
    t1->setZValue(-1);
    scene->addItem(t1);
    decorItems.append(t1);

    QGraphicsRectItem *t2 = new QGraphicsRectItem(540, 540, 170, 8);
    t2->setPen(QPen(Qt::NoPen));
    t2->setBrush(track);
    t2->setZValue(-1);
    scene->addItem(t2);
    decorItems.append(t2);

    if (night) {
        const QPointF stars[6] = {
            QPointF(120, 70), QPointF(300, 120), QPointF(520, 60),
            QPointF(700, 120), QPointF(220, 240), QPointF(620, 220)
        };
        for (const QPointF &s : stars) {
            QGraphicsEllipseItem *star = new QGraphicsEllipseItem(s.x(), s.y(), 3, 3);
            star->setPen(QPen(Qt::NoPen));
            star->setBrush(QColor(255, 255, 230, 140));
            star->setZValue(-1);
            scene->addItem(star);
            decorItems.append(star);
        }
    }
}

void MainWindow::spawnEnemy(qint64 nowMs) {
    if (enemiesToSpawn <= 0 || nowMs < nextEnemySpawnMs) return;
    if (aliveEnemyCount() >= 4) return;

    const qreal spawnXs[] = {120.0, 400.0, 680.0};
    int start = QRandomGenerator::global()->bounded(3);
    for (int i = 0; i < 3; ++i) {
        int idx = (start + i) % 3;
        QPointF p(spawnXs[idx], 60.0);
        bool free = true;
        for (EnemyTank *e : enemies) {
            if (e->isAlive() && (e->pos() - p).manhattanLength() < 120.0) {
                free = false;
                break;
            }
        }
        if (!free) continue;

        EnemyTank *enemy = new EnemyTank(p.x(), p.y());
        scene->addItem(enemy);
        enemies.append(enemy);
        enemiesAliveInWave++;
        enemiesToSpawn--;
        qreal interval = qMax(1000.0, 2000.0 - (waveNumber - 1) * 150.0);
        nextEnemySpawnMs = nowMs + interval;
        return;
    }
    nextEnemySpawnMs = nowMs + 300;
}

void MainWindow::respawnPlayer() {
    player->setPos(160, 500);
    player->setDirection(Qt::Key_Up);
    player->setAlive(true);
    player->setVisible(true);
    player->setOpacity(1.0);
    qint64 nowMs = frameClock.elapsed() - clockOffsetMs;
    invincibleUntilMs = nowMs + 2000;
    lastBlinkMs = 0;
    playSound(respawnSound);
}

void MainWindow::togglePause() {
    if (!gameRunning) return;
    if (paused) {
        playSound(pauseSound);
        clockOffsetMs += frameClock.elapsed() - pauseStartMs;
        paused = false;
        if (pauseText) {
            scene->removeItem(pauseText);
            delete pauseText;
            pauseText = nullptr;
        }
        if (!muted && !ambiencePlaying) {
            ambienceSound->play();
            ambiencePlaying = true;
        }
        statusBar()->showMessage("已继续 (P/Esc 暂停, M 静音)");
    } else {
        playSound(pauseSound);
        if (enginePlaying) {
            engineSound->stop();
            enginePlaying = false;
        }
        if (ambiencePlaying) {
            ambienceSound->stop();
            ambiencePlaying = false;
        }
        pauseStartMs = frameClock.elapsed();
        paused = true;
        pauseText = scene->addText("已暂停 - 按 P 继续");
        QFont f("Arial", 28, QFont::Bold);
        pauseText->setFont(f);
        pauseText->setDefaultTextColor(QColor(255, 230, 120));
        pauseText->setZValue(30);
        pauseText->setPos(SCENE_WIDTH / 2 - 170, SCENE_HEIGHT / 2 - 40);
    }
}

void MainWindow::showWaveBanner(const QString &text, int durationMs) {
    QGraphicsTextItem *banner = scene->addText(text);
    QFont f("Arial", 22, QFont::Bold);
    banner->setFont(f);
    banner->setDefaultTextColor(QColor(255, 235, 150));
    banner->setZValue(20);
    QFontMetrics fm(f);
    qreal w = fm.horizontalAdvance(text);
    banner->setPos(qMax(0.0, (SCENE_WIDTH - w) / 2), SCENE_HEIGHT / 2 - 40);
    QTimer::singleShot(durationMs, [banner]() { banner->deleteLater(); });
}

void MainWindow::showGameOverOverlay(bool newRecord) {
    if (gameOverOverlay) return;

    gameOverOverlay = new QGraphicsRectItem(0, 0, SCENE_WIDTH, SCENE_HEIGHT);
    gameOverOverlay->setPen(QPen(Qt::NoPen));
    gameOverOverlay->setBrush(QColor(0, 0, 0, 175));
    gameOverOverlay->setZValue(40);
    scene->addItem(gameOverOverlay);

    QString encouragement;
    if (newRecord) {
        encouragement = "新纪录！你已经是 RXC 王牌！";
    } else if (score >= 1000) {
        encouragement = "顶级表现！敌人看到你都要绕路。";
    } else if (score >= 500) {
        encouragement = "优秀！距离传说只差一点。";
    } else if (score >= 200) {
        encouragement = "不错的战绩，继续推进波次！";
    } else {
        encouragement = "稳住节奏，下一局一定能走得更远！";
    }

    QString text = QString("游戏结束\n得分: %1\n坚持到第 %2 波\n最高得分: %3\n最佳波次: %4\n\n%5\n\n按 R 重新开始")
                       .arg(score)
                       .arg(waveNumber)
                       .arg(highScore)
                       .arg(bestWave)
                       .arg(encouragement);
    gameOverText = scene->addText(text);
    QFont f("Arial", 20, QFont::Bold);
    gameOverText->setFont(f);
    gameOverText->setDefaultTextColor(QColor(255, 240, 180));
    gameOverText->setZValue(41);
    gameOverText->setPos(SCENE_WIDTH / 2 - 240, SCENE_HEIGHT / 2 - 130);
}

void MainWindow::showScorePopup(const QPointF &pos, const QString &text) {
    QGraphicsTextItem *pop = scene->addText(text);
    pop->setFont(QFont("Arial", 14, QFont::Bold));
    pop->setDefaultTextColor(QColor(255, 220, 80));
    pop->setPos(pos.x() + 12, pos.y() - 12);
    pop->setZValue(15);
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [pop, timer]() {
        pop->setPos(pop->x(), pop->y() - 1.2);
        pop->setOpacity(pop->opacity() - 0.1);
        if (pop->opacity() <= 0.0) {
            timer->stop();
            pop->deleteLater();
            timer->deleteLater();
        }
    });
    timer->start(40);
}

void MainWindow::spawnMuzzleFlash(const QPointF &pos) {
    QGraphicsEllipseItem *flash = new QGraphicsEllipseItem(pos.x() - 5, pos.y() - 5, 12, 12);
    flash->setPen(QPen(Qt::NoPen));
    flash->setBrush(QColor(255, 230, 90, 210));
    flash->setZValue(4);
    scene->addItem(flash);
    QTimer::singleShot(70, [flash]() { delete flash; });
}

void MainWindow::playSound(QSoundEffect *sound) {
    if (muted || !sound) return;
    sound->play();
}

int MainWindow::aliveEnemyCount() const {
    int n = 0;
    for (EnemyTank *e : enemies) {
        if (e->isAlive()) n++;
    }
    return n;
}

void MainWindow::cleanupDeadEnemies() {
    for (int i = enemies.size() - 1; i >= 0; --i) {
        EnemyTank *enemy = enemies[i];
        if (enemy->isAlive()) continue;
        for (Bullet *b : bullets) {
            if (b->getOwner() == enemy) {
                b->clearOwner();
            }
        }
        enemies.removeAt(i);
        delete enemy;
    }
}
// UI更新
void MainWindow::updateUI() {
    QString msg = QString("得分: %1  生命: %2  波次: %3  最高: %4")
                      .arg(score).arg(lives).arg(waveNumber).arg(highScore);

    // 计算存活敌人数量
    int aliveCount = aliveEnemyCount();
    msg += QString("  敌人: %1").arg(aliveCount);
    if (combo >= 2) {
        msg += QString("  连击 x%1").arg(qMin(combo, 5));
    }

    statusBar()->showMessage(msg);

    if (scoreDisplay) {
        scoreDisplay->setPlainText(QString("得分: %1").arg(score));
    }
    if (livesDisplay) {
        livesDisplay->setPlainText(QString("生命: %1").arg(lives));
    }
    if (enemyDisplay) {
        QString enemyText = QString("敌人: %1").arg(aliveCount);
        if (combo >= 2) {
            enemyText += QString("  x%1").arg(qMin(combo, 5));
        }
        enemyDisplay->setPlainText(enemyText);
    }
    if (waveDisplay) {
        waveDisplay->setPlainText(QString("波次: %1").arg(waveNumber));
    }
}

// 开火
void MainWindow::fireBullet(Tank *tank) {
    if (!gameRunning) return;
    if (!tank->isAlive()) return;
    if (tank->data(0).toInt() == TYPE_PLAYER) {
        if (shootCooldown > 0) return;
        shootCooldown = 18;
    }
    QPointF muzzle = tank->getMuzzlePos();
    Bullet *bullet = new Bullet(tank, tank->getDirection(), muzzle.x(), muzzle.y());
    scene->addItem(bullet);
    bullets.append(bullet);
    spawnMuzzleFlash(muzzle);
    playSound(shootSound);
}

// 移动坦克（含碰撞）
bool MainWindow::moveTank(Tank *tank, qreal dx, qreal dy) {
    if (!tank->isAlive()) return false;

    bool moved = false;
    qreal oldX = tank->x();
    qreal newX = oldX + dx;
    if (newX >= 0 && newX <= SCENE_WIDTH - TANK_SIZE) {
        tank->setX(newX);
        if (isBlocked(tank)) {
            tank->setX(oldX);
        } else {
            moved = true;
        }
    }

    qreal oldY = tank->y();
    qreal newY = oldY + dy;
    if (newY >= 0 && newY <= SCENE_HEIGHT - TANK_SIZE) {
        tank->setY(newY);
        if (isBlocked(tank)) {
            tank->setY(oldY);
        } else {
            moved = true;
        }
    }
    return moved;
}

bool MainWindow::isBlocked(Tank *tank) const {
    QList<QGraphicsItem*> colliding = tank->collidingItems();
    QRectF tankBody = tank->mapRectToScene(tank->rect()).adjusted(1.0, 1.0, -1.0, -1.0);
    for (QGraphicsItem *item : colliding) {
        int type = item->data(0).toInt();
        if (type == TYPE_WALL || type == TYPE_STEEL_WALL || type == TYPE_ENEMY || type == TYPE_PLAYER) {
            QRectF overlap = tankBody.intersected(item->sceneBoundingRect());
            if (!overlap.isValid()) continue;
            if (type == TYPE_ENEMY || type == TYPE_PLAYER) {
                Tank *other = static_cast<Tank*>(item);
                if (!other->isAlive()) continue;
            }
            return true;
        }
    }
    return false;
}

//游戏主循环
void MainWindow::gameLoop() {
    if (paused || !gameRunning) return;

    qint64 nowMs = frameClock.elapsed() - clockOffsetMs;
    float deltaTime = (lastFrameMs == 0) ? 0.016f : (nowMs - lastFrameMs) / 1000.0f;
    lastFrameMs = nowMs;
    // 限制最大步长，防止卡顿时瞬移
    if (deltaTime > 0.05f) deltaTime = 0.05f;

    // 玩家复活与无敌闪烁
    if (player && !player->isAlive()) {
        if (nowMs >= playerRespawnMs) {
            respawnPlayer();
        }
    }
    if (player && player->isAlive() && nowMs < invincibleUntilMs) {
        if (nowMs - lastBlinkMs >= 100) {
            lastBlinkMs = nowMs;
            player->setOpacity(player->opacity() == 1.0 ? 0.35 : 1.0);
        }
    } else if (player && player->isAlive()) {
        player->setOpacity(1.0);
    }

    // 波次刷怪
    if (waveBreakUntilMs > 0) {
        if (nowMs >= waveBreakUntilMs) {
            waveBreakUntilMs = 0;
            startWave(waveNumber + 1);
        }
    } else {
        spawnEnemy(nowMs);
    }

    // 玩家移动
    qreal dx = 0, dy = 0;
    if (keyUp)    { dy = -PLAYER_SPEED*deltaTime; player->setDirection(Qt::Key_Up); }
    else if (keyDown)  { dy =  PLAYER_SPEED*deltaTime; player->setDirection(Qt::Key_Down); }
    else if (keyLeft)  { dx = -PLAYER_SPEED*deltaTime; player->setDirection(Qt::Key_Left); }
    else if (keyRight) { dx =  PLAYER_SPEED*deltaTime; player->setDirection(Qt::Key_Right); }
    if (dx || dy) moveTank(player, dx, dy);
    if (spacePressed) fireBullet(player);

    bool playerMoving = player->isAlive() && (keyUp || keyDown || keyLeft || keyRight);
    if (playerMoving && !muted && !enginePlaying) {
        engineSound->play();
        enginePlaying = true;
    } else if (!playerMoving && enginePlaying) {
        engineSound->stop();
        enginePlaying = false;
    }

    // 敌方移动
    qreal enemySpeed = ENEMY_SPEED + qMin(6.0 * (waveNumber - 1), 60.0);
    for (EnemyTank *enemy : enemies) {
        if (!enemy->isAlive()) continue;
        enemy->updateAI(player->pos(), nowMs);
        qreal edx = 0, edy = 0;
        switch(enemy->getDirection()) {
        case Qt::Key_Up:    edy = -enemySpeed*deltaTime; break;
        case Qt::Key_Down:  edy =  enemySpeed*deltaTime; break;
        case Qt::Key_Left:  edx = -enemySpeed*deltaTime; break;
        case Qt::Key_Right: edx =  enemySpeed*deltaTime; break;
        }
        if (!moveTank(enemy, edx, edy) && (edx != 0 || edy != 0)) {
            enemy->onBlocked(nowMs);
        }
        if (enemy->shouldFire(nowMs)) {
            fireBullet(enemy);
        }
    }

    // 子弹移动及碰撞
    for (int i = bullets.size() - 1; i >= 0; --i) {
        if (!gameRunning) break;
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
            if (type == TYPE_BULLET || type == TYPE_EXPLOSION) continue;
            if (item == static_cast<QGraphicsItem*>(bullet->getOwner())) continue;
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
                // 敌方子弹不会误伤友军；玩家子弹仍可正常击毁敌人
                if (!bullet->getOwner() || bullet->getOwner()->data(0).toInt() == TYPE_ENEMY) continue;
                enemy->setAlive(false);
                scene->removeItem(enemy);
                //创建爆炸特效
                Explosion *exp = new Explosion(enemy->x(), enemy->y());
                scene->addItem(exp);
                combo++;
                int multiplier = qMin(combo, 5);
                int gain = 10 * multiplier;
                score += gain;
                showScorePopup(enemy->pos(), QString("+%1 x%2").arg(gain).arg(multiplier));
                enemiesAliveInWave--;
                playSound(explosionSound);
                scene->removeItem(bullet);
                bullets.removeAt(i);
                delete bullet;
                removed = true;
                if (enemiesToSpawn == 0 && enemiesAliveInWave == 0 && waveBreakUntilMs == 0) {
                    int bonus = waveNumber * 50;
                    score += bonus;
                    showScorePopup(QPointF(SCENE_WIDTH / 2, SCENE_HEIGHT / 2 - 20),
                                   QString("波次奖励 +%1").arg(bonus));
                    QString title;
                    if (waveNumber >= 15) title = "王牌";
                    else if (waveNumber >= 10) title = "老兵";
                    else if (waveNumber >= 5) title = "新兵";
                    QString bannerText = QString("第 %1 波通过 +%2").arg(waveNumber).arg(bonus);
                    if (!title.isEmpty()) bannerText += QString("\n称号: %1").arg(title);
                    showWaveBanner(bannerText, 2000);
                    playSound(waveSound);
                    waveBreakUntilMs = nowMs + 2000;
                }
                updateUI();
                break;

            }
            else if (type == TYPE_PLAYER) {
                if (!player->isAlive() || player == bullet->getOwner()) continue;
                if (nowMs < invincibleUntilMs) continue;
                Explosion *exp = new Explosion(player->x(), player->y());
                scene->addItem(exp);
                lives--;
                combo = 0;
                showScorePopup(player->pos(), "连击中断");
                playSound(explosionSound);
                scene->removeItem(bullet);
                bullets.removeAt(i);
                delete bullet;
                removed = true;
                if (lives <= 0) {
                    player->setAlive(false);
                    player->setVisible(false);
                    gameOver();
                } else {
                    player->setAlive(false);
                    player->setVisible(false);
                    playerRespawnMs = nowMs + 1500;
                }
                updateUI();
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

    cleanupDeadEnemies();

    if (nowMs - lastHudUpdateMs >= 100) {
        updateUI();
        lastHudUpdateMs = nowMs;
    }
    if (shootCooldown > 0) shootCooldown--;
}

// 游戏结束
void MainWindow::gameOver() {
    gameRunning = false;
    gameLoopTimer->stop();
    if (enginePlaying) {
        engineSound->stop();
        enginePlaying = false;
    }
    if (ambiencePlaying) {
        ambienceSound->stop();
        ambiencePlaying = false;
    }

    bool newRecord = score > highScore;
    if (newRecord) highScore = score;
    if (waveNumber > bestWave) bestWave = waveNumber;
    QSettings settings("RXC", "TankBattle");
    settings.setValue("highScore", highScore);
    settings.setValue("bestWave", bestWave);

    playSound(gameOverSound);
    showGameOverOverlay(newRecord);
    updateUI();
}

void MainWindow::resetGame() {
    clearGame();
    initGame();
    updateUI();
}

// 键盘事件
void MainWindow::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
    case Qt::Key_Up:case Qt::Key_W:    keyUp = true; break;
    case Qt::Key_Down:case Qt::Key_S: keyDown = true; break;
    case Qt::Key_Left:case Qt::Key_A:  keyLeft = true; break;
    case Qt::Key_Right:case Qt::Key_D: keyRight = true; break;
    case Qt::Key_Space: spacePressed = true; break;
    case Qt::Key_P:
    case Qt::Key_Escape:
        togglePause();
        break;
    case Qt::Key_M:
        muted = !muted;
        if (muted) {
            if (enginePlaying) {
                engineSound->stop();
                enginePlaying = false;
            }
            if (ambiencePlaying) {
                ambienceSound->stop();
                ambiencePlaying = false;
            }
            statusBar()->showMessage("已静音 (M 恢复)");
        } else {
            if (!ambiencePlaying) {
                ambienceSound->play();
                ambiencePlaying = true;
            }
            statusBar()->showMessage("音效已开启 (M 静音)");
        }
        break;
    case Qt::Key_R:
        resetGame();
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
    case Qt::Key_W:     keyUp = false; break;
    case Qt::Key_S:     keyDown = false; break;
    case Qt::Key_A:     keyLeft = false; break;
    case Qt::Key_D:     keyRight = false; break;
    case Qt::Key_Space: spacePressed = false; break;
    default: QMainWindow::keyReleaseEvent(event);
    }
}
void MainWindow::initMap() {
    // 清空之前的地形
    for (QGraphicsItem *item : terrainItems) {
        scene->removeItem(item);
        delete item;
    }
    terrainItems.clear();

    // 1. 填充地图：0为空地，1=砖墙，2=钢墙
    const char layout[MAP_ROWS][MAP_COLS + 1] = {
        "22222222222222222222",
        "2..................2",
        "2..................2",
        "2.11....11....11...2",
        "2.11....11....11...2",
        "2..................2",
        "2..................2",
        "222..............222",
        "222..............222",
        "2..................2",
        "2...11.....11...11.2",
        "2...11.....11...11.2",
        "2..................2",
        "2..................2",
        "22222222222222222222"
    };
    for (int r=0; r<MAP_ROWS; ++r)
        for (int c=0; c<MAP_COLS; ++c)
            mapData[r][c] = layout[r][c] - '0';

    // 根据mapData生成对应的图形项
    int cellSize = SCENE_WIDTH / MAP_COLS;  // 40
    for (int r=0; r<MAP_ROWS; ++r) {
        for (int c=0; c<MAP_COLS; ++c) {
            qreal x = c * cellSize;
            qreal y = r * cellSize;
            int type = mapData[r][c];
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
