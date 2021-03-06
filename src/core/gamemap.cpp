#include <QDebug>
#include "gamemap.h"

void GameAbstractMap::add(GameObject *object)
{
    connect(object, SIGNAL(moved(Coordinate, Coordinate)), this, SLOT(moveObject(Coordinate, Coordinate)));
    connect(object, SIGNAL(placed(Coordinate)), this, SLOT(placeObject(Coordinate)));
    if (object->isPlaced()) {
        internalAdd(object);
    }
}

bool GameAbstractMap::canPlace(const GameObject *object, const Coordinate &position) const
{
    for (const Coordinate &delta : object->cellsRelative()) {
        if (!inBounds(height_, width_, position + delta)) {
            return false;
        }
    }
    return true;
}

bool GameAbstractMap::conflictsWith(const GameObject *object, const Coordinate &position) const
{
    for (const Coordinate &delta : object->cellsRelative()) {
        const QVector<GameObject *> objects = atPos(position + delta);
        for (const GameObject *map_object : objects) {
            if (objectsConflict(map_object, object)) {
                return true;
            }
        }
    }
    return false;
}

GameAbstractMap::GameAbstractMap(QObject *parent, int height, int width)
    : QObject(parent), height_(height), width_(width)
{
}

int GameAbstractMap::height() const
{
    return height_;
}

int GameAbstractMap::width() const
{
    return width_;
}

void GameAbstractMap::moveObject(const Coordinate &oldPosition, const Coordinate &)
{
    GameObject *object = qobject_cast<GameObject *>(sender());
    internalRemove(object, oldPosition);
    internalAdd(object);
}

void GameAbstractMap::placeObject(const Coordinate &)
{
    internalAdd(qobject_cast<GameObject *>(sender()));
}

void GameAbstractMap::remove(GameObject *object)
{
    disconnect(object, SIGNAL(moved(Coordinate, Coordinate)), this, SLOT(moveObject(Coordinate, Coordinate)));
    disconnect(object, SIGNAL(placed(Coordinate)), this, SLOT(placeObject(Coordinate)));
    if (object->isPlaced()) {
        internalRemove(object, object->position());
    }
}

GameMap::GameMap(QObject *parent, int height, int width) :
    GameAbstractMap(parent, height, width), map_(height, QVector<GameObject *>(width))
{
}

GameObject *GameMap::at(const Coordinate &pos) const
{
    return map_.at(pos.x).at(pos.y);
}

bool GameMap::canPlace(const GameObject *object, const Coordinate &position) const
{
    if (!GameAbstractMap::canPlace(object, position)) {
        return false;
    }
    for (const Coordinate &delta : object->cellsRelative()) {
        if (at(position + delta) != nullptr && at(position + delta) != object) {
            return false;
        }
    }
    return true;
}

void GameMap::internalAdd(GameObject *object)
{
    Q_ASSERT(canPlace(object, object->position()));
    for (const Coordinate &cell : object->cells()) {
        map_[cell.x][cell.y] = object;
    }
}

void GameMap::internalRemove(GameObject *object, const Coordinate &position)
{
    for (const Coordinate &delta : object->cellsRelative()) {
        map_[position.x + delta.x][position.y + delta.y] = nullptr;
    }
}

bool GameMap::freeCell(const Coordinate &position) const
{
    return map_[position.x][position.y] == nullptr;
}

QVector<GameObject *> GameMap::atPos(const Coordinate &position) const
{
    if (map_[position.x][position.y] == nullptr) {
        return {};
    } else {
        return {map_[position.x][position.y]};
    }
}

QVector<GameObject *> GameMultimap::atPos(const Coordinate &position) const
{
    return map_[position.x][position.y]->get();
}

const GameList *GameMultimap::at(const Coordinate &pos) const
{
    return map_[pos.x][pos.y];
}

GameList *GameMultimap::atMut(const Coordinate &pos)
{
    return map_[pos.x][pos.y];
}

void GameMultimap::internalAdd(GameObject *object)
{
    Q_ASSERT(canPlace(object, object->position()));
    for (const Coordinate &cell : object->cells()) {
        atMut(cell)->add(object);
    }
}

void GameMultimap::internalRemove(GameObject *object, const Coordinate &position)
{
    for (const Coordinate &delta : object->cellsRelative()) {
        atMut(position + delta)->remove(object);
    }
}

bool GameMultimap::freeCell(const Coordinate &position) const
{
    return map_[position.x][position.y]->empty();
}

GameMultimap::GameMultimap(QObject *parent, int height, int width)
    : GameAbstractMap(parent, height, width), map_(height, QVector<GameList *>(width))
{
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            map_[i][j] = new GameList(this);
        }
    }
}
