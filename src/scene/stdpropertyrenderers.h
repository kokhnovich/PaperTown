#ifndef STDPROPERTYRENDERERS_H
#define STDPROPERTYRENDERERS_H

#include <QObject>
#include <QPixmap>
#include <QFont>
#include <QPalette>
#include "gamepropertyrenderer.h"
#include "../objects/stdproperties.h"

const int DATA_KEY_PROP_FLAGS = 192;

const int PROP_FLAG_WRECKED = 1;
const int PROP_FLAG_TIMER = 2;
const int PROP_FLAG_BORDER = 3;
const int PROP_FLAG_HEALTH = 4;

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
    void updateTimerWidget(GameProperty_building *property, QWidget *widget);
    void updateHealthWidget(GameProperty_building *property, QWidget *widget);
    
    QList<QGraphicsItem *> doDrawProperty(GameObjectProperty *property) override;
private:
    QVector<QPixmap> images_;
    QVector<QPixmap> small_textures_;
    QPixmap wrecked_icon_;
    QPixmap progress_bar_image_;
    QPixmap health_icon_;
    QFont label_font_;
    
    int getTextureIndex(double stage) const;
    QColor getTextColor(double stage) const;

    void loadTextures();
    
    friend class BuildingTimerItem;
};

#endif // STDPROPERTYRENDERERS_H
