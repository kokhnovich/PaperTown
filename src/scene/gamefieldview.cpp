#include <QtDebug>
#include <QGraphicsWidget>
#include "gamefieldview.h"
#include "gamescenegeometry.h"

void GameFieldView::changeObjectSelectionState(GameObject *object, SelectionState state)
{
    renderer_->changeObjectSelectionState(object, objects_.values(object), last_state_, state);

    if (state == SelectionState::Selected && object->active()) {
        selection_control_ = renderer_->drawSelectionControl(object);
    } else if (selection_control_ != nullptr) {
        selection_control_->deleteLater();
        selection_control_ = nullptr;
    }

    if (state == SelectionState::Moving) {
        moving_item_ = renderer_->drawMoving(object);
    } else if (moving_item_ != nullptr) {
        scene_->removeItem(moving_item_);
        delete moving_item_;
        moving_item_ = nullptr;
    }

    last_state_ = state;
}

void GameFieldView::movingPositionChanged(const Coordinate &, const Coordinate &)
{
    GameObject *object = qobject_cast<GameObject *>(sender());
    Q_CHECK_PTR(moving_item_);
    scene_->removeItem(moving_item_);
    delete moving_item_;
    moving_item_ = renderer_->drawMoving(object);
}

void GameFieldView::selectObject()
{
    GameObject *object = qobject_cast<GameObject *>(sender());
    changeObjectSelectionState(object, SelectionState::Selected);
}

void GameFieldView::unselectObject()
{
    GameObject *object = qobject_cast<GameObject *>(sender());
    changeObjectSelectionState(object, SelectionState::None);
}

void GameFieldView::startMovingObject()
{
    GameObject *object = qobject_cast<GameObject *>(sender());
    changeObjectSelectionState(object, SelectionState::Moving);
}

void GameFieldView::endMovingObject()
{
    GameObject *object = qobject_cast<GameObject *>(sender());
    changeObjectSelectionState(object, SelectionState::Selected);
}

void GameFieldView::addObject(GameObject *object)
{
    connect(object, &GameObject::placed, this, &GameFieldView::placeObject);
    connect(object, &GameObject::moved, this, &GameFieldView::moveObject);
    connect(object, &GameObject::updated, this, &GameFieldView::updateObject);
    connect(object, &GameObject::selected, this, &GameFieldView::selectObject);
    connect(object, &GameObject::unselected, this, &GameFieldView::unselectObject);
    connect(object, &GameObject::startedMoving, this, &GameFieldView::startMovingObject);
    connect(object, &GameObject::endedMoving, this, &GameFieldView::endMovingObject);
    connect(object, &GameObject::movingPositionChanged, this, &GameFieldView::movingPositionChanged);

    putObject(object);
}

void GameFieldView::putObject(GameObject *object)
{
    if (object->active()) {
        auto items = renderer_->drawObject(object);
        for (QGraphicsItem *item : items) {
            objects_.insert(object, item);
        }
    }
    if (object->isSelected()) {
        changeObjectSelectionState(object, object->getSelectionState());
    }
}

void GameFieldView::moveObject(const Coordinate &old_pos, const Coordinate &)
{
    GameObject *object = qobject_cast<GameObject *>(sender());
    renderer_->moveObject(object, old_pos, objects_.values(object));
}

void GameFieldView::placeObject(const Coordinate &)
{
    putObject(qobject_cast<GameObject *>(sender()));
}

void GameFieldView::updateObject()
{
    GameObject *object = qobject_cast<GameObject *>(sender());
    if (objects_.contains(object)) {
        unputObject(object);
        putObject(object);
    }
}

GameFieldView::GameFieldView(QObject *parent, GameTextureRenderer *renderer)
    : QObject(parent),
      renderer_(renderer),
      scene_(renderer_->scene()),
      objects_(),
      moving_item_(nullptr),
      selection_control_(nullptr),
      last_state_(SelectionState::None)
{
    renderer_->setupScene();
}

void GameFieldView::unputObject(GameObject *object)
{
    if (object->isSelected()) {
        changeObjectSelectionState(object, SelectionState::None);
    }
    if (objects_.contains(object)) {
        iterateTextures(object, [&](QGraphicsItem *item) {
            scene_->removeItem(item);
            delete item;
        });
        objects_.remove(object);
    }
}

void GameFieldView::removeObject(GameObject *object)
{
    disconnect(object, &GameObject::placed, this, &GameFieldView::placeObject);
    disconnect(object, &GameObject::moved, this, &GameFieldView::moveObject);
    disconnect(object, &GameObject::updated, this, &GameFieldView::updateObject);
    disconnect(object, &GameObject::selected, this, &GameFieldView::selectObject);
    disconnect(object, &GameObject::unselected, this, &GameFieldView::unselectObject);
    disconnect(object, &GameObject::startedMoving, this, &GameFieldView::startMovingObject);
    disconnect(object, &GameObject::endedMoving, this, &GameFieldView::endMovingObject);
    disconnect(object, &GameObject::movingPositionChanged, this, &GameFieldView::movingPositionChanged);

    unputObject(object);
}
