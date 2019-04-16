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
    QWidget *drawControlWidget(GameObjectProperty *property) override;
private:
    QPixmap image_;
    QFont label_font_;
    QPalette label_palette_;
};

#endif // STDPROPERTYRENDERERS_H
