#include <QDebug>
#include <QGraphicsPixmapItem>
#include <QGraphicsEffect>
#include <QGraphicsSceneMouseEvent>
#include "gamescene.h"
#include "../core/gameobjects.h"

GameScene::GameScene(QObject *parent, GameObjectRepository *repository,
                     GameField *field, GameTextureRepository *textures)
    : QGraphicsScene(parent),
      repository_(repository),
      field_(field),
      textures_(textures),
      geometry_(new GameSceneGeometry(this, field_)),
      renderer_(new GameTextureRenderer(this, geometry_, textures_, this)),
      view_(new GameFieldView(this, renderer_, repository_))
{
    connect(field_, &GameField::added, view_, &GameFieldView::addObject);
    connect(field_, &GameField::removed, view_, &GameFieldView::removeObject);
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

void GameScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (field_->selection() == nullptr) {
        return;
    }
    field_->selection()->setSelectPosition(geometry_->scenePosToCoord(event->scenePos()));
}

void GameScene::mousePressEvent(QGraphicsSceneMouseEvent *)
{
    if (field_->selection() == nullptr) {
        return;
    }
    field_->selection()->applySelectPosition();
}
