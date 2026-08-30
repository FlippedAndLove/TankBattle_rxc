QT       += widgets multimedia
CONFIG   += c++11
TARGET   = tank_game
TEMPLATE = app

RESOURCES += assets.qrc

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    tank.cpp \
    player.cpp \
    enemy.cpp \
    bullet.cpp \
    wall.cpp \
    steelwall.cpp \
    explosion.cpp

HEADERS  += \
    mainwindow.h \
    tank.h \
    player.h \
    enemy.h \
    bullet.h \
    wall.h \
    steelwall.h \
    explosion.h
