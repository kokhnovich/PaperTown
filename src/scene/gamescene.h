#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>
#include "../core/gameobjects.h"
#include "../core/gamefield.h"

const int CELL_SIZE = 25;
const int FIELD_HEIGHT = 50;
const int FIELD_WIDTH = 70;
const int SLOPE_HEIGHT = 1;
const int SLOPE_WIDTH = 2;

class GameScene : public QGraphicsScene
{
    Q_OBJECT
public:
    GameScene(QObject *parent = nullptr);
protected:
    QPolygonF coordinateToPoly(const Coordinate &c);
    QRectF coordinateToRect(const Coordinate &c);
    QPointF coordinateToTopLeft(const Coordinate &c);
private:
    
};

#endif // GAMESCENE_H
