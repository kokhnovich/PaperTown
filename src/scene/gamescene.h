#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>
#include <QMap>
#include <QPixmap>
#include <QPointF>
#include "../core/gameobjects.h"
#include "../core/gamefield.h"

const int CELL_SIZE = 25;
const int FIELD_HEIGHT = 42;
const int FIELD_WIDTH = 42;
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
    void setupField();
    void initObjects();
};

#endif // GAMESCENE_H
