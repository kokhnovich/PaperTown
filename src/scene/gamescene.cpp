#include <QDebug>
#include <QGraphicsPixmapItem>
#include <QGraphicsEffect>
#include "gamescene.h"

GameScene::GameScene(QObject *parent)
    : RenderScene(parent), textures_(new GameTextureRepository)
{
    textures_->loadFromFile(":/img/textures.json");
    initObjects();
    setupField();
}

QPointF GameScene::coordinateToTopLeft(const Coordinate &c)
{
    return QPointF(SLOPE_WIDTH * CELL_SIZE * (c.x + c.y), SLOPE_HEIGHT * CELL_SIZE * (c.x - c.y - 1));
}

QPolygonF GameScene::coordinateToPoly(const Coordinate &c)
{
    auto base_point = coordinateToTopLeft(c);
    return QPolygonF({
        base_point + QPointF(CELL_SIZE * SLOPE_WIDTH, 0),
        base_point + QPointF(0, CELL_SIZE * SLOPE_HEIGHT),
        base_point + QPointF(CELL_SIZE * SLOPE_WIDTH, 2 * CELL_SIZE * SLOPE_HEIGHT),
        base_point + QPointF(2 * CELL_SIZE * SLOPE_WIDTH, CELL_SIZE * SLOPE_HEIGHT)
    });
}

QRectF GameScene::coordinateToRect(const Coordinate &c)
{
    auto base_point = coordinateToTopLeft(c);
    return QRectF(base_point, base_point + QPointF(2 * CELL_SIZE * SLOPE_WIDTH, 2 * CELL_SIZE * SLOPE_HEIGHT));
}

qreal GameScene::zOrder(const Coordinate &c, qreal priority) const
{
    return (c.x + 1) * FIELD_WIDTH - c.y + priority;
}

QGraphicsItem *GameScene::drawTexture(const QString &name, const Coordinate &c, qreal priority)
{
    const GameTexture *texture = textures_->getTexture(name);
    Q_CHECK_PTR(texture);
    QGraphicsPixmapItem *item = addPixmap(texture->pixmap);
    item->setOffset(texture->offset + coordinateToTopLeft(c));
    item->setZValue(zOrder(c + texture->z_offset, priority));
    return item;
}

QGraphicsItem *GameScene::moveTexture(QGraphicsItem *item, const QString &name,
        const Coordinate &c, qreal priority)
{
    const GameTexture *texture = textures_->getTexture(name);
    Q_CHECK_PTR(texture);
    item->setPos(texture->offset + coordinateToTopLeft(c));
    item->setZValue(zOrder(c + texture->z_offset, priority));
    return item;
}

void GameScene::initObjects()
{
}

void GameScene::setupField()
{
    QRectF scene_rect(
        0,
        -SLOPE_HEIGHT * CELL_SIZE * FIELD_WIDTH,
        SLOPE_WIDTH * CELL_SIZE * (FIELD_WIDTH + FIELD_HEIGHT),
        SLOPE_HEIGHT * CELL_SIZE * (FIELD_WIDTH + FIELD_HEIGHT));

    setSceneRect(scene_rect);

    QBrush light_brush(QPixmap(":/img/cell.png"));
    QBrush dark_brush(QPixmap(":/img/cell-dark.png"));
    setBackgroundBrush(dark_brush);

    QPolygonF poly({
        QPointF(0, 0),
        QPointF(SLOPE_WIDTH *CELL_SIZE * FIELD_WIDTH, -SLOPE_HEIGHT *CELL_SIZE * FIELD_WIDTH),
        QPointF(SLOPE_WIDTH *CELL_SIZE * (FIELD_HEIGHT + FIELD_WIDTH), SLOPE_HEIGHT *CELL_SIZE * (FIELD_HEIGHT - FIELD_WIDTH)),
        QPointF(SLOPE_WIDTH *CELL_SIZE * FIELD_HEIGHT, SLOPE_HEIGHT *CELL_SIZE * FIELD_HEIGHT)});

    QPen border_pen(QColor(255, 0, 0, 128));
    border_pen.setWidth(6.0);

    addPolygon(poly, border_pen, light_brush);

    for (int i = 0; i < FIELD_HEIGHT; ++i) {
        for (int j = 0; j < FIELD_WIDTH; ++j) {
            //QBrush brush(QColor(255.0 / FIELD_HEIGHT * i, 255.0 / FIELD_WIDTH * j, 0, 0));
            //QPen pen(QColor(0, 0, 0, 64));
            //pen.setWidth(1.0);
            //addPolygon(coordinateToPoly({i, j}), pen, brush);
        }
    }

    for (int i = 0; i < FIELD_HEIGHT; i ++) {
        for (int j = 0; j < FIELD_WIDTH; j ++) {
            if (i % 20 == 10 && j % 20 == 10) {
                drawTexture("cinema", {i, j});
                continue;
            }
            if (10 <= i % 20 && i % 20 < 14 && 10 <= j % 20 && j % 20 < 14) {
                continue;
            }
            if (i % 4 == 0 && j % 4 == 0) {
                drawTexture((qrand() & 1) ? "tree1" : "tree2", {i, j});
            }
        }
    }
}
