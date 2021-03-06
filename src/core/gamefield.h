#ifndef GAMEFIELD_H
#define GAMEFIELD_H

#include <QObject>
#include "gamemap.h"
#include "gamelist.h"
#include "gameobjectfactory.h"
#include "eventscheduler.h"

class GameField : public GameFieldBase
{
    Q_OBJECT
public:
    GameField(QObject *parent, GameObjectRepositoryBase *repository, int height, int width);
    GameObject *add(GameObject *object) override;
    GameObject *add(const QString &type, const QString &name);
    GameObject *add(const QString &type, const QString &name, const Coordinate &pos);
    void remove(GameObject *object) override;
    bool canPutObject(const GameObject *object, const Coordinate &pos) const override;
    GameList *getByType(const QString &type) const;
    QVector<GameObject *> getCell(const Coordinate &pos) const;
    GameObjectRepositoryBase *repository() const override;
    GameObject *selection() const;
    int height() const;
    int width() const;
    GameObjectFactory *factory() const;
    GameEventScheduler *scheduler() const;
    GameIndicators *indicators() const override;
signals:
    void added(GameObject *object);
    void removed(GameObject *object);
    void placed(GameObject *object, const Coordinate &pos);
    void moved(GameObject *object, const Coordinate &oldPos, const Coordinate &newPos);
    void updated(GameObject *object);
    void selected(GameObject *object);
    void unselected(GameObject *object);
protected slots:
    void objectSelecting();
    void objectUnselected();
private:
    GameObjectRepositoryBase *repository_;
    GameIndicators *indicators_;
    GameObjectFactory *factory_;
    
    GameMap *ground_map_;
    GameMap *static_map_;
    GameMultimap *moving_map_;

    GameList *ground_list_;
    GameList *static_list_;
    GameList *moving_list_;
    
    GameObject *selection_;
    
    GameEventScheduler *scheduler_;
};

#endif // GAMEFIELD_H
