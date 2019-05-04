#include "coordinates.h"

Rect boundingRect(const QVector<Coordinate> &points)
{
    Q_ASSERT(!points.empty());
    int top = points[0].x, bottom = points[0].x, left = points[0].y, right = points[0].y;
    for (const Coordinate &coord : qAsConst(points)) {
        top = qMin(top, coord.x);
        bottom = qMax(bottom, coord.x);
        left = qMin(left, coord.y);
        right = qMax(right, coord.y);
    }
    return {top, bottom, left, right};
}

Coordinate::Coordinate(int x, int y)
    : x(x), y(y)
{
}

Coordinate Coordinate::applyDirection(Util::Direction dir)
{
    Coordinate res = *this;
    switch (dir) {
    case Util::Up: {
        --res.x;
        break;
    }
    case Util::Down: {
        ++res.x;
        break;
    }
    case Util::Left: {
        --res.y;
        break;
    }
    case Util::Right: {
        ++res.y;
        break;
    }
    default: {
        Q_UNREACHABLE();
    }
    }
    return res;
}

Coordinate &operator+=(Coordinate &a, const Coordinate &b)
{
    a.x += b.x;
    a.y += b.y;
    return a;
}

Coordinate &operator-=(Coordinate &a, const Coordinate &b)
{
    a.x -= b.x;
    a.y -= b.y;
    return a;
}

Coordinate operator+(Coordinate a, const Coordinate &b)
{
    return a += b;
}

Coordinate operator-(Coordinate a, const Coordinate &b)
{
    return a -= b;
}

Rect::Rect(Coordinate a, Coordinate b)
    : top(qMin(a.x, b.x)), bottom(qMax(a.x, b.x)), left(qMin(a.y, b.y)), right(qMax(a.y, b.y))
{}

Rect::Rect(int top, int bottom, int left, int right)
    : top(top), bottom(bottom), left(left), right(right)
{}

bool inBounds(int height, int width, const Coordinate &coord)
{
    return 0 <= coord.x && coord.x < height && 0 <= coord.y && coord.y < width;
}

QDebug operator<<(QDebug d, const Coordinate& c)
{
    d.nospace() << "Coordinate(" << c.x << ", " << c.y << ")";
    return d.maybeSpace();
}

QVector<Border> calcBorders(const QVector<Coordinate> &points)
{
    const int MAX_SIZE = 64;
    static bool has_point[MAX_SIZE+2][MAX_SIZE+2] = {};
    
    Rect bound = boundingRect(points);
    Q_ASSERT(bound.bottom - bound.top + 1 <= MAX_SIZE);
    Q_ASSERT(bound.right - bound.left + 1 <= MAX_SIZE);
    
    Coordinate offset = Coordinate(1, 1) - bound.topLeft();
    QVector<Border> borders;
    
    for (Coordinate point : points) {
        point += offset;
        has_point[point.x][point.y] = true;
    }
    
    for (Coordinate point : points) {
        point += offset;
        for (int dir_number = 0; dir_number < 4; ++dir_number) {
            Util::Direction dir = static_cast<Util::Direction>(dir_number);
            Coordinate neighbor_point = point.applyDirection(dir);
            if (!has_point[neighbor_point.x][neighbor_point.y]) {
                borders.append({point - offset, dir});
            }
        }
    }
    
    for (Coordinate point : points) {
        point += offset;
        has_point[point.x][point.y] = false;
    }

    return borders;
}

