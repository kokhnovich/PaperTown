#ifndef GAMEOBJECTRENDERER_H
#define GAMEOBJECTRENDERER_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QMediaPlayer>
#include "../core/gameobjects.h"
#include "gametexturerenderer.h"
#include "gameobjectrenderrepository.h"
#include "../util/misc.h"

class GameFieldView : public QObject
{
    Q_OBJECT
public:
    GameFieldView(QObject *parent, GameTextureRenderer *renderer);
public slots:
    void playMusic(const Util::Sound& sound);
    void addObject(GameObject *object);
    void removeObject(GameObject *object);
protected:
    void putObject(GameObject *object);
    void unputObject(GameObject *object);
    void changeObjectSelectionState(GameObject *object, SelectionState state);
protected slots:
    void placeObject(const Coordinate &);
    void moveObject(const Coordinate &, const Coordinate &);
    void startMovingObject();
    void endMovingObject();
    void updateObject();
    void selectObject();
    void movingPositionChanged(const Coordinate &, const Coordinate &);
    void unselectObject();
private:
    template<typename T>
    void iterateTextures(GameObject *object, const T &func)
    {
        auto it = objects_.find(object);
        while (it != objects_.end() && it.key() == object) {
            func(it.value());
            ++it;
        }
    }

    GameTextureRenderer *renderer_;
    QGraphicsScene *scene_;
    QMultiHash<GameObject *, QGraphicsItem *> objects_;
    QGraphicsItem *moving_item_;
    QGraphicsWidget *selection_control_;
    SelectionState last_state_;
    QMediaPlayer media_player_;
};

#endif // GAMEOBJECTRENDERER_H
