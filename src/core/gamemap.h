#ifndef GAMEMAP_H
#define GAMEMAP_H

#include <QObject>
#include <QVector>
#include "gameobjects.h"
#include "gamelist.h"

class GameAbstractMap : public QObject
{
    Q_OBJECT
public:
    explicit GameAbstractMap(QObject *parent, int height, int width);
    void add(GameObject *object);
    void remove(GameObject *object);
    virtual bool canPlace(const GameObject *object, const Coordinate &position) const;
    virtual bool freeCell(const Coordinate &position) const = 0;
    virtual QVector<GameObject *> atPos(const Coordinate &position) const = 0;
    bool conflictsWith(const GameObject *object, const Coordinate &position) const;
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
    explicit GameMap(QObject *parent, int height, int width);
    GameObject *at(const Coordinate &pos) const;
    QVector<GameObject *> atPos(const Coordinate &position) const override;
    bool canPlace(const GameObject *object, const Coordinate &position) const override;
    bool freeCell(const Coordinate &position) const override;
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
    explicit GameMultimap(QObject *parent, int height, int width);
    const GameList *at(const Coordinate &pos) const;
    QVector<GameObject *> atPos(const Coordinate &position) const override;
    bool freeCell(const Coordinate &position) const override;
protected:
    GameList *atMut(const Coordinate &pos);
    void internalAdd(GameObject *object) override;
    void internalRemove(GameObject *object, const Coordinate &position) override;
private:
    QVector<QVector<GameList *>> map_;
};

#endif // GAMEMAP_H
