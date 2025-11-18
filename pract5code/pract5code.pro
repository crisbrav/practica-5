TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        Box.cpp \
        Obstacle.cpp \
        Particle.cpp \
        Vec2.cpp \
        main.cpp

HEADERS += \
    Box.h \
    Obstacle.h \
    Particle.h \
    Vec2.h
