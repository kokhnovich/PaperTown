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
    label_palette_.setColor(QPalette::WindowText, QColor(137, 48, 20));
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

GamePropertyRenderer_human::GamePropertyRenderer_human(GameTextureRendererBase* renderer)
    : GameAbstractPropertyRenderer(renderer)
{}

QList<QGraphicsItem *> GamePropertyRenderer_human::drawProperty(GameObjectProperty* a_property)
{
    // FIXME : rewrite it!!! (for more efficiency & better code)
    auto property = qobject_cast<GameProperty_human *>(a_property);
    auto dir = property->direction();
    QString dir_name = QString(QMetaEnum::fromType<Util::Direction>().key(dir)).toLower();
    
    int stage = property->stage();
    int cell_size = geometry()->cellSize();
    
    auto delta = Coordinate(0, 0).applyDirection(dir);
    
    auto texture = textures()->getTexture(QStringLiteral("human-anim-%1").arg(dir_name));
    
    auto offs = texture->offset;
    auto z_offs = texture->z_offset;
    
    int texture_id = stage;
    if (texture_id > 5) {
        texture_id -= 5;
        offs += geometry()->offset(delta) / 2;
    }
    
    QPixmap pixmap = texture->pixmap.copy(QRect(cell_size * 3 * texture_id, 0, cell_size * 3, cell_size * 4));
    
    if (stage != 0) {
        z_offs -= delta;
        offs -= geometry()->offset(delta);
    }
    
    auto item = scene()->addPixmap(pixmap);
    item->setOffset(offs);
    item->setPos(geometry()->coordinateToTopLeft(property->gameObject()->position()));
    item->setZValue(geometry()->zOrder(z_offs));
    
    return {item};
}


