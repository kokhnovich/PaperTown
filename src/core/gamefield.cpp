#include "gamefield.h"

GameField::GameField(QObject *parent) :  GameFieldBase()
{
}

void GameField::add(GameObject *object)
{
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
    delete object;
}

bool GameField::move(GameObject* object, const Coordinate& new_pos)
{
    return object->setPosition(new_pos);
}

bool GameField::canPlace(GameObject *object) const
{
    if (object->type() == "ground") {
        return ground_map_->canPlace(object);
    } else if (object->type() == "static") {
        return static_map_->canPlace(object);
    } else if (object->type() == "moving") {
        //return moving_map_->canPlace(object);
        return true;
    } else {
        Q_ASSERT(0);
    }
}

GameList* GameField::get(const QString &type) const
{
    if (type == "ground") {
        return ground_list_;
    } else if (type == "moving") {
        return moving_list_;
    } else if (type == "static") {
        return static_list_;
    } else {
        Q_ASSERT(0);
    }
}

Cell GameField::getCell(const Coordinate &pos) const
{
   return Cell(ground_map_->at(pos),
               static_map_->at(pos),
               moving_map_->at(pos)->get());
}
