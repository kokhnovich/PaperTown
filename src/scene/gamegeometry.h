#ifndef GAMEGEOMETRY_H
#define GAMEGEOMETRY_H

#include <QObject>
#include <QRectF>
#include <QPolygonF>
#include <QPointF>
#include "../core/gamefield.h"

class GameSceneGeometry : public QObject
{
    Q_OBJECT
public:
    GameSceneGeometry(QObject *parent, GameField *field);
    
    QPolygonF coordinateToPoly(const Coordinate &c) const;
    QRectF coordinateToRect(const Coordinate &c) const;
    QPointF coordinateToTopLeft(const Coordinate &c) const;
    QRectF fieldRect() const;
    QPolygonF fieldActivePolygon() const;
    
    inline int fieldHeight() const { return field_->height(); }
    inline int fieldWidth() const { return field_->width(); }
private:
    GameField *field_;
};

#endif // GAMEGEOMETRY_H
