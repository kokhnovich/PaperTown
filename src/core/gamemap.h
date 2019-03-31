#ifndef GAMEMAP_H
#define GAMEMAP_H

#include <QObject>
#include <QVector>
#include "gameobjects.h"
#include "gamelist.h"

class GameAbstractMap : public QObject {
    Q_OBJECT
public:
    explicit GameAbstractMap(QObject *parent = nullptr, int height = 1, int width = 1);
    void add(GameObject *object);
    void remove(GameObject *object);
    virtual bool canPlace(GameObject *object, const Coordinate &position) const;
    int height() const;
    int width() const;
protected slots:
    void placeObject(const Coordinate &);
    void moveObject(const Coordinate &oldPosition, const Coordinate &);
protected:
    virtual void internalAdd(GameObject *object) = 0;
    virtual void internalRemove(GameObject *object, const Coordinate &position) = 0;
private:
    int height_;
    int width_;
};

class GameMap : public GameAbstractMap
{
    Q_OBJECT
public:
    explicit GameMap(QObject *parent = nullptr, int height = 1, int width = 1);
    GameObject *at(const Coordinate &pos) const;
    bool canPlace(GameObject *object, const Coordinate &position) const override;
protected:
    void internalAdd(GameObject *object) override;
    void internalRemove(GameObject *object, const Coordinate &position) override;
private:
    QVector<QVector<GameObject *>> map_;
};

class GameMultimap : public GameAbstractMap
{
    Q_OBJECT
public:
    explicit GameMultimap(QObject *parent = nullptr, int height = 1, int width = 1);
    const GameList *at(const Coordinate &pos) const;
protected:
    GameList *atMut(const Coordinate &pos);
    void internalAdd(GameObject *object) override;
    void internalRemove(GameObject *object, const Coordinate &position) override;
private:
    QVector<QVector<GameList *>> map_;
};

#endif // GAMEMAP_H
