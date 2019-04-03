#include "gamefield.h"

GameField::GameField(QObject *parent, GameObjectRepositoryBase *repository, int height, int width) :
    GameFieldBase(parent),
    repository_(repository),
    ground_map_(new GameMap(this, height, width)),
    static_map_(new GameMap(this, height, width)),
    moving_map_(new GameMultimap(this, height, width)),
    ground_list_(new GameList(this)),
    static_list_(new GameList(this)),
    moving_list_(new GameList(this))
{
}

GameObject *GameField::add(GameObject *object)
{
    attach(object);
    connect(object, &GameObject::moved, this, [ = ](const Coordinate &oldPos, const Coordinate &pos) {
        emit moved(qobject_cast<GameObject *>(sender()), oldPos, pos);
    });
    connect(object, &GameObject::placed, this, [ = ](const Coordinate  pos) {
        emit placed(qobject_cast<GameObject *>(sender()), pos);
    });
    connect(object, &GameObject::updated, this, [ = ]() {
        emit updated(qobject_cast<GameObject *>(sender()));
    });
    object->setParent(this);

    Q_ASSERT(!object->active() || canPlace(object, object->position()));

    if (object->type() == "ground") {
        ground_list_->add(object);
        ground_map_->add(object);
    } else if (object->type() == "moving") {
        moving_list_->add(object);
        moving_map_->add(object);
    } else if (object->type() == "static") {
        static_list_->add(object);
        static_map_->add(object);
    } else {
        Q_ASSERT(0);
    }

    emit added(object);
    return object;
}

void GameField::remove(GameObject *object)
{
    if (object->type() == "ground") {
        ground_list_->remove(object);
        ground_map_->remove(object);
    } else if (object->type() == "moving") {
        moving_list_->remove(object);
        moving_map_->remove(object);
    } else if (object->type() == "static") {
        static_list_->remove(object);
        static_map_->remove(object);
    } else {
        Q_ASSERT(0);
    }

    emit removed(object);
    delete object;
}

bool GameField::canPlace(GameObject *object, const Coordinate &pos) const
{
    /* TODO : invent something better to check compatibility between object */
    if (object->type() == "ground") {
        return ground_map_->canPlace(object, pos);
    }
    if (object->type() == "static") {
        return static_map_->canPlace(object, pos) &&
            moving_map_->freePlace(object, pos);
    } 
    if (object->type() == "moving") {
        return moving_map_->canPlace(object, pos) &&
            static_map_->freePlace(object, pos);
    }
    Q_ASSERT(0);
    return false;
}

GameList *GameField::getByType(const QString &type) const
{
    if (type == "ground") {
        return ground_list_;
    }
    if (type == "moving") {
        return moving_list_;
    }
    if (type == "static") {
        return static_list_;
    }
    Q_ASSERT(0);
    return nullptr;
}

QVector<GameObject *> GameField::getCell(const Coordinate &pos) const
{
    auto result = ground_map_->atPos(pos);
    result.append(static_map_->atPos(pos));
    result.append(moving_map_->atPos(pos));
    return result;
}

GameObjectRepositoryBase *GameField::repository() const
{
    return repository_;
}
