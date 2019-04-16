#include <QLabel>
#include "../scene/stdpropertyrenderers.h"
#include "../objects/stdproperties.h"

GamePropertyRenderer_house::GamePropertyRenderer_house(GameTextureRendererBase *renderer)
    : GameAbstractPropertyRenderer(renderer)
{}

QWidget *GamePropertyRenderer_house::drawControlWidget(GameObjectProperty *a_property)
{
    auto property = qobject_cast<GameProperty_house *>(a_property);
    return new QLabel(QString::number(property->population()));
}
