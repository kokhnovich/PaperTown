#include <QDebug>
#include <QGraphicsPixmapItem>
#include <QGraphicsEffect>
#include <QGraphicsSceneMouseEvent>
#include "gamescene.h"
#include "../core/gameobjects.h"

GameScene::GameScene(QObject *parent, GameObjectRepository *repository,
                     GameField *field, GameTextureRepository *textures)
    : RenderScene(parent),
      repository_(repository),
      field_(field),
      textures_(textures),
      renderer_(new GameObjectRenderer(this, repository_))
{
    connect(field_, &GameField::added, renderer_, &GameObjectRenderer::addObject);
    connect(field_, &GameField::removed, renderer_, &GameObjectRenderer::removeObject);
    setupField();
}

void GameScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    auto items_at_cell = this->items(event->scenePos());
    for (auto it = items_at_cell.begin(); it != items_at_cell.end(); ++it) {
        QVariant data = (*it)->data(DATA_KEY_GAMEOBJECT);
        if (data.isNull()) {
            continue;
        }
        GameObject *object = qvariant_cast<GameObject *>(data);
        if (object->isSelected()) {
            object->unselect();
        } else {
            object->select();
        }
        return;
    }
    if (field_->selection() != nullptr) {
        field_->selection()->unselect();
    }
}

GameField *GameScene::field() const
{
    return field_;
}

GameObjectRenderer *GameScene::renderer() const
{
    return renderer_;
}

GameObjectRepository *GameScene::repository() const
{
    return repository_;
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
    return (c.x + 1) * fieldWidth() - c.y + priority;
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

void GameScene::setupField()
{
    QRectF scene_rect(
        0,
        -SLOPE_HEIGHT * CELL_SIZE * fieldWidth() - TOP_MARGIN,
        SLOPE_WIDTH * CELL_SIZE * (fieldWidth() + fieldHeight()),
        SLOPE_HEIGHT * CELL_SIZE * (fieldWidth() + fieldHeight()) + TOP_MARGIN);

    setSceneRect(scene_rect);

    QBrush light_brush(QPixmap(":/img/cell.png"));
    QBrush dark_brush(QPixmap(":/img/cell-dark.png"));
    setBackgroundBrush(dark_brush);

    QPolygonF poly({
        QPointF(0, 0),
        QPointF(SLOPE_WIDTH *CELL_SIZE * fieldWidth(), -SLOPE_HEIGHT *CELL_SIZE * fieldWidth()),
        QPointF(SLOPE_WIDTH *CELL_SIZE * (fieldHeight() + fieldWidth()), SLOPE_HEIGHT *CELL_SIZE * (fieldHeight() - fieldWidth())),
        QPointF(SLOPE_WIDTH *CELL_SIZE * fieldHeight(), SLOPE_HEIGHT *CELL_SIZE * fieldHeight())});

    QPen border_pen(QColor(255, 0, 0, 128));
    border_pen.setWidth(6.0);

    addPolygon(poly, border_pen, light_brush)->setZValue(-1e9);

    for (int i = 0; i < fieldHeight(); ++i) {
        for (int j = 0; j < fieldWidth(); ++j) {
            QBrush brush(QColor(255.0 / fieldHeight() * i, 255.0 / fieldWidth() * j, 0, 0));
            QPen pen(QColor(0, 0, 0, 32));
            pen.setWidth(1.0);
            addPolygon(coordinateToPoly({i, j}), pen, brush)->setZValue(-1e9);
        }
    }
}
