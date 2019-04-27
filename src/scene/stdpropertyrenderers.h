#ifndef STDPROPERTYRENDERERS_H
#define STDPROPERTYRENDERERS_H

#include <QObject>
#include <QPixmap>
#include <QFont>
#include <QPalette>
#include "gamepropertyrenderer.h"

class GamePropertyRenderer_house : public GameAbstractPropertyRenderer
{
    Q_OBJECT
public:
    GamePropertyRenderer_house(GameTextureRendererBase *renderer);
    QWidget *createControlWidget(GameObjectProperty *property) override;
    void updateControlWidget(GameObjectProperty *property, QWidget *widget) override;
private:
    QPixmap image_;
    QFont label_font_;
    QPalette label_palette_;
};

class GamePropertyRenderer_human : public GameAbstractPropertyRenderer
{
    Q_OBJECT
public:
    GamePropertyRenderer_human(GameTextureRendererBase *renderer);
    void updatePropertyItem(QGraphicsItem *item, GameObjectProperty *property) override;
protected:
    QList<QGraphicsItem *> doDrawProperty(GameObjectProperty *property) override;
private:
    struct AnimatedTexture {
        QList<QPixmap> frames;
        QPointF offset;
        Coordinate z_offset;
    };

    QList<AnimatedTexture> textures_;
    QMetaEnum direction_meta_;
    bool textures_loaded_;

    void loadTextures();
};

class GamePropertyRenderer_building : public GameAbstractPropertyRenderer
{
    Q_OBJECT
public:
    QWidget *createControlWidget(GameObjectProperty *property) override;
    void updateControlWidget(GameObjectProperty *property, QWidget *widget) override;
    void updatePropertyItem(QGraphicsItem *item, GameObjectProperty *property) override;
    Util::Bool3 canShowMainObject(GameObjectProperty *property) override;
    
    GamePropertyRenderer_building(GameTextureRendererBase *renderer);
protected:
    QList<QGraphicsItem *> doDrawProperty(GameObjectProperty *property) override;
private:
    QVector<QPixmap> textures_;
    QVector<QPixmap> small_textures_;
    QFont label_font_;
    
    int getTextureIndex(double stage) const;

    void loadTextures();
    
    friend class BuildingTimerItem;
};

#endif // STDPROPERTYRENDERERS_H
