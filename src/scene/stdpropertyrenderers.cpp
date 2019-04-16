#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>
#include "../scene/stdpropertyrenderers.h"
#include "../objects/stdproperties.h"

GamePropertyRenderer_house::GamePropertyRenderer_house(GameTextureRendererBase *renderer)
    : GameAbstractPropertyRenderer(renderer),
      image_(QStringLiteral(":/img/icon-population.png")),
      label_font_(),
      label_palette_()
{
    label_font_.setWeight(QFont::Bold);
    label_font_.setPixelSize(0.75 * image_.height());
    label_palette_.setColor(QPalette::WindowText, QColor("#893014"));
}

QWidget *GamePropertyRenderer_house::drawControlWidget(GameObjectProperty *a_property)
{
    auto property = qobject_cast<GameProperty_house *>(a_property);
    
    auto widget = new QWidget;
    auto layout = new QHBoxLayout(widget);
    
    auto icon_label = new QLabel;
    icon_label->setPixmap(image_);
    layout->addWidget(icon_label);
    
    auto text_label = new QLabel(QString::number(property->population()));
    text_label->setFont(label_font_);
    text_label->setPalette(label_palette_);
    layout->addWidget(text_label);
    
    return widget;
}
