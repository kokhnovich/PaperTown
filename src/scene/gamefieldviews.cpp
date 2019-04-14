#include <QtDebug>
#include <QGraphicsWidget>
#include "gamefieldviews.h"
#include "gamescenegeometry.h"

qreal GameFieldView::getStateZDelta(SelectionState state) const
{
    return (state == SelectionState::Selected) ? 1e6 : 0;
}

void GameFieldView::changeObjectSelectionState(GameObject *object, SelectionState state)
{
    iterateTextures(object, [ = ](const TextureInfo & info) {
        qreal opacity = 1.0;
        switch (state) {
        case SelectionState::None: {
            opacity = 1.0;
            break;
        }
        case SelectionState::Moving: {
            opacity = 0.5;
            break;
        }
        case SelectionState::Selected: {
            opacity = 0.75;
            break;
        }
        default: {
            Q_UNREACHABLE();
        }
        }
        info.item->setOpacity(opacity);

        double zdelta = getStateZDelta(state) - getStateZDelta(last_state_);
        if (!qFuzzyIsNull(zdelta)) {
            info.item->setZValue(info.item->zValue() + zdelta);
        }
    });

    if (state == SelectionState::Selected && object->active()) {
        selection_control_ = renderer_->drawControlButtons(object);
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
        auto info = repository_->getRenderInfo(object);
        for (const QString &texture_name : info->textures) {
            QGraphicsItem *item = renderer_->drawTexture(texture_name, object->position(), info->priority);
            item->setData(DATA_KEY_GAMEOBJECT, QVariant::fromValue(object));
            objects_.insert(object, {
                texture_name,
                item,
                info->priority
            });
        }
    }
    if (object->isSelected()) {
        changeObjectSelectionState(object, object->isMoving() ? SelectionState::Moving : SelectionState::Selected);
    }
}

void GameFieldView::moveObject(const Coordinate &, const Coordinate &newPosition)
{
    GameObject *object = qobject_cast<GameObject *>(sender());
    iterateTextures(object, [ = ](const TextureInfo & info) {
        renderer_->moveTexture(info.item, info.name, newPosition);
    });
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

GameFieldView::GameFieldView(QObject *parent, GameTextureRenderer *renderer, GameObjectRenderRepository *repository)
    : QObject(parent),
      renderer_(renderer),
      scene_(renderer_->scene()),
      repository_(repository),
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
        iterateTextures(object, [&](const TextureInfo & info) {
            scene_->removeItem(info.item);
            delete info.item;
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

void GameObjectRenderRepository::addRenderInfo(const QString &type, const QString &name,
        const GameObjectRenderRepository::RenderInfo &info)
{
    QString full_name = fullName(type, name);
    render_info_[full_name] = QSharedPointer<RenderInfo>(new RenderInfo(info));
}

void GameObjectRenderRepository::doLoadObject(const QString& type, const QString& name, const QJsonObject& json)
{
    GameObjectRepository::doLoadObject(type, name, json);
    RenderInfo info;
    auto texture_arr = json.value("textures").toArray();
    info.textures.resize(texture_arr.size());
    for (int i = 0; i < texture_arr.size(); ++i) {
        info.textures[i] = texture_arr[i].toString();
    }
    info.priority = json.value("priority").toDouble(type_priorities_[type]);
    info.caption = json.value("caption").toString(name);
    addRenderInfo(type, name, info);
}

const GameObjectRenderRepository::RenderInfo *GameObjectRenderRepository::getRenderInfo(const QString &type, const QString &name) const
{
    return render_info_[fullName(type, name)].data();
}

const GameObjectRenderRepository::RenderInfo *GameObjectRenderRepository::getRenderInfo(GameObject *object) const
{
    return getRenderInfo(object->type(), object->name());
}

GameObjectRenderRepository::GameObjectRenderRepository(QObject *parent)
    : GameObjectRepository(parent)
{
    type_priorities_["ground"] = 0;
    type_priorities_["static"] = 0.33;
    type_priorities_["moving"] = 0.67;
}


