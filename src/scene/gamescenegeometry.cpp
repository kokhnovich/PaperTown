#include "gamescenegeometry.h"
#include <QMatrix>
#include <QtDebug>
#include <cmath>

const int CELL_SIZE = 25;
const double SELECTION_MUL = 1.8;
const int SLOPE_HEIGHT = 1;
const int SLOPE_WIDTH = 2;
const int TOP_MARGIN = 125;

QPolygonF GameSceneGeometry::coordinateToPoly(const Coordinate &c) const
{
    auto base_point = coordinateToTopLeft(c);
    return cellPolygon().translated(base_point);
}

QPolygonF GameSceneGeometry::cellPolygon() const
{
    return QPolygonF({
        QPointF(CELL_SIZE * SLOPE_WIDTH, 0),
        QPointF(0, CELL_SIZE * SLOPE_HEIGHT),
        QPointF(CELL_SIZE * SLOPE_WIDTH, 2 * CELL_SIZE * SLOPE_HEIGHT),
        QPointF(2 * CELL_SIZE * SLOPE_WIDTH, CELL_SIZE * SLOPE_HEIGHT)
    });
}

int GameSceneGeometry::cellSize() const
{
    return CELL_SIZE;
}

QPolygonF GameSceneGeometry::selectionPolygon() const
{
    auto res = cellPolygon();
    return selection_mat.map(cellPolygon());
}

QRectF GameSceneGeometry::coordinateToRect(const Coordinate &c) const
{
    auto base_point = coordinateToTopLeft(c);
    return QRectF(base_point, base_point + QPointF(2 * CELL_SIZE * SLOPE_WIDTH, 2 * CELL_SIZE * SLOPE_HEIGHT));
}

QPointF GameSceneGeometry::coordinateToTopLeft(const Coordinate &c) const
{
    return QPointF(0, -SLOPE_HEIGHT * CELL_SIZE) + offset(c);
}

QPointF GameSceneGeometry::offset(const Coordinate &c) const
{
    return QPointF(SLOPE_WIDTH * CELL_SIZE * (c.x + c.y), SLOPE_HEIGHT * CELL_SIZE * (c.x - c.y));
}


QPolygonF GameSceneGeometry::fieldActivePolygon() const
{
    return QPolygonF({
        QPointF(0, 0),
        QPointF(SLOPE_WIDTH *CELL_SIZE * fieldWidth(), -SLOPE_HEIGHT *CELL_SIZE * fieldWidth()),
        QPointF(SLOPE_WIDTH *CELL_SIZE * (fieldHeight() + fieldWidth()), SLOPE_HEIGHT *CELL_SIZE * (fieldHeight() - fieldWidth())),
        QPointF(SLOPE_WIDTH *CELL_SIZE * fieldHeight(), SLOPE_HEIGHT *CELL_SIZE * fieldHeight())});
}

QRectF GameSceneGeometry::fieldRect() const
{
    return QRectF(
               0,
               -SLOPE_HEIGHT * CELL_SIZE * fieldWidth() - TOP_MARGIN,
               SLOPE_WIDTH * CELL_SIZE * (fieldWidth() + fieldHeight()),
               SLOPE_HEIGHT * CELL_SIZE * (fieldWidth() + fieldHeight()) + TOP_MARGIN
           );
}

Coordinate GameSceneGeometry::scenePosToCoord(const QPointF& point) const
{
    double x = point.x() / (CELL_SIZE * SLOPE_WIDTH);
    double y = point.y() / (CELL_SIZE * SLOPE_HEIGHT) + 1;
    return {static_cast<int>(floor((x + y) / 2)), static_cast<int>(floor((x - y) / 2))};
}

qreal GameSceneGeometry::zOrder(const Coordinate& c, qreal priority) const
{
    return 4 * fieldWidth() + zOrderOffset(c) + priority;
}

qreal GameSceneGeometry::zOrderOffset(const Coordinate& c) const
{
    return 4 * c.x * fieldWidth() - 2 * c.y;
}

qreal GameSceneGeometry::borderZOrder(const Border &b) const
{
    qreal res = zOrder(b.cell);
    switch (b.side) {
        case Util::Up: {
            res -= 2 * fieldWidth();
            break;
        }
        case Util::Down: {
            res += 2 * fieldWidth();
            break;
        }
        case Util::Left: {
            res += 1;
            break;
        }
        case Util::Right: {
            res -= 1;
            break;
        }
        default: {
            Q_UNREACHABLE();
        }
    };
    return res;
}

qreal GameSceneGeometry::selectionStateZDelta(SelectionState state) const
{
    return (state == SelectionState::Selected) ? 1e6 : 0;
}

GameSceneGeometry::GameSceneGeometry(QObject *parent, GameField *field)
    : QObject(parent), field_(field)
{
    selection_mat.translate(CELL_SIZE * SLOPE_WIDTH, CELL_SIZE * SLOPE_HEIGHT);
    selection_mat.scale(SELECTION_MUL, SELECTION_MUL);
    selection_mat.translate(-CELL_SIZE * SLOPE_WIDTH, -CELL_SIZE * SLOPE_HEIGHT);
}
