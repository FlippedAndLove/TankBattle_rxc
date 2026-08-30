QT       += widgets
CONFIG   += c++11
TARGET   = tank_game
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    tank.cpp \
    player.cpp \
    enemy.cpp \
    bullet.cpp \
    wall.cpp

HEADERS  += \
    mainwindow.h \
    tank.h \
    player.h \
    enemy.h \
    bullet.h \
    wall.h