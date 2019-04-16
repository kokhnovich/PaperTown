#ifndef STDPROPERTYRENDERERS_H
#define STDPROPERTYRENDERERS_H

#include <QObject>
#include "gamepropertyrenderer.h"

class GamePropertyRenderer_house : public GameAbstractPropertyRenderer
{
    Q_OBJECT
public:
    GamePropertyRenderer_house(GameTextureRendererBase *renderer);
    QWidget *drawControlWidget(GameObjectProperty *property) override;
};

#endif // STDPROPERTYRENDERERS_H
