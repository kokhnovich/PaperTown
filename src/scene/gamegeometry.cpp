#include "gamegeometry.h"

const int CELL_SIZE = 25;
const int SLOPE_HEIGHT = 1;
const int SLOPE_WIDTH = 2;
const int TOP_MARGIN = 125;

QPolygonF GameSceneGeometry::coordinateToPoly(const Coordinate &c) const
{
    auto base_point = coordinateToTopLeft(c);
    return QPolygonF({
        base_point + QPointF(CELL_SIZE * SLOPE_WIDTH, 0),
        base_point + QPointF(0, CELL_SIZE * SLOPE_HEIGHT),
        base_point + QPointF(CELL_SIZE * SLOPE_WIDTH, 2 * CELL_SIZE * SLOPE_HEIGHT),
        base_point + QPointF(2 * CELL_SIZE * SLOPE_WIDTH, CELL_SIZE * SLOPE_HEIGHT)
    });
}

QRectF GameSceneGeometry::coordinateToRect(const Coordinate &c) const
{
    auto base_point = coordinateToTopLeft(c);
    return QRectF(base_point, base_point + QPointF(2 * CELL_SIZE * SLOPE_WIDTH, 2 * CELL_SIZE * SLOPE_HEIGHT));
}

QPointF GameSceneGeometry::coordinateToTopLeft(const Coordinate &c) const
{
    return QPointF(SLOPE_WIDTH * CELL_SIZE * (c.x + c.y), SLOPE_HEIGHT * CELL_SIZE * (c.x - c.y - 1));
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

GameSceneGeometry::GameSceneGeometry(QObject *parent, GameField *field)
    : QObject(parent), field_(field)
{}
