#ifndef GAMEGEOMETRY_H
#define GAMEGEOMETRY_H

#include <QObject>
#include <QRectF>
#include <QPolygonF>
#include <QPointF>
#include <QMatrix>
#include "../core/gamefield.h"

class GameSceneGeometry : public QObject
{
    Q_OBJECT
public:
    GameSceneGeometry(QObject *parent, GameField *field);
    
    QPolygonF coordinateToPoly(const Coordinate &c) const;
    QRectF coordinateToRect(const Coordinate &c) const;
    QPolygonF cellPolygon() const;
    QPolygonF selectionPolygon() const;
    QPointF coordinateToTopLeft(const Coordinate &c) const;
    QPointF offset(const Coordinate &c) const;
    QRectF fieldRect() const;
    QPolygonF fieldActivePolygon() const;
    
    int cellSize() const;
    
    Coordinate scenePosToCoord(const QPointF &point) const;
    
    qreal zOrder(const Coordinate &c, qreal priority = 0.0) const;
    qreal borderZOrder(const Border &b) const;
    qreal selectionStateZDelta(SelectionState state) const;
    
    inline qreal movingZDelta() const { return 1e8; }
    inline qreal controlZDelta() const { return 1e9; }
    
    inline int fieldHeight() const { return field_->height(); }
    inline int fieldWidth() const { return field_->width(); }
private:
    GameField *field_;
    QMatrix selection_mat;
};

#endif // GAMEGEOMETRY_H
