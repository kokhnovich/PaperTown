#ifndef COORDINATES_H
#define COORDINATES_H

#include <QObject>
#include <QVector>
#include <QtDebug>
#include "misc.h"

struct Coordinate {
    int x, y;

    Coordinate(int x = 0, int y = 0);

    Coordinate applyDirection(Util::Direction dir);
};

struct Border {
    Coordinate cell;
    Util::Direction side;
};

struct Rect {
    int top, bottom, left, right;
    
    inline Coordinate topLeft() const { return {top, left}; }
    inline Coordinate topRight() const { return {top, right}; }
    inline Coordinate bottomLeft() const { return {bottom, left}; }
    inline Coordinate bottomRight() const { return {bottom, right}; }

    Rect(int top, int bottom, int left, int right);
    Rect(Coordinate a, Coordinate b);
};

Rect boundingRect(const QVector<Coordinate> &points);

Coordinate operator+(Coordinate a, const Coordinate &b);
Coordinate operator-(Coordinate a, const Coordinate &b);
Coordinate &operator+=(Coordinate &a, const Coordinate &b);
Coordinate &operator-=(Coordinate &a, const Coordinate &b);

QDebug operator<<(QDebug d, const Coordinate &c);

bool inBounds(int height, int width, const Coordinate &coord);

QVector<Border> calcBorders(const QVector<Coordinate> &points);

#endif // COORDINATES_H
