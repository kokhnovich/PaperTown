#ifndef COORDINATES_H
#define COORDINATES_H

#include <QObject>
#include <QVector>

namespace Util
{
    Q_NAMESPACE
    
    enum Direction {
        Up,
        Down,
        Left,
        Right
    };
    Q_ENUM_NS(Direction);
}

struct Coordinate {
    int x, y;

    Coordinate(int x = 0, int y = 0);

    Coordinate applyDirection(Util::Direction dir);
};

struct Rect {
    int top, bottom, left, right;

    Rect(int top, int bottom, int left, int right);
    Rect(Coordinate a, Coordinate b);
};

Rect boundingRect(const QVector<Coordinate> &points);

Coordinate operator+(Coordinate a, const Coordinate &b);
Coordinate operator-(Coordinate a, const Coordinate &b);
Coordinate &operator+=(Coordinate &a, const Coordinate &b);
Coordinate &operator-=(Coordinate &a, const Coordinate &b);

bool inBounds(int height, int width, const Coordinate &coord);

#endif // COORDINATES_H
