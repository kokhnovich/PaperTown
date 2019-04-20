#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>
#include "../scene/stdpropertyrenderers.h"
#include "../objects/stdproperties.h"

const int MAX_FRAME = 5;

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
    : GameAbstractPropertyRenderer(renderer),
      direction_meta_(QMetaEnum::fromType<Util::Direction>()),
      textures_loaded_(false)
{}

QList<QGraphicsItem *> GamePropertyRenderer_human::doDrawProperty(GameObjectProperty* a_property)
{
    auto item = new QGraphicsPixmapItem();
    scene()->addItem(item);
    updatePropertyItem(item, a_property);
    return {item};
}

void GamePropertyRenderer_human::updatePropertyItem(QGraphicsItem *a_item, GameObjectProperty *a_property)
{
    loadTextures();
    
    auto item = qgraphicsitem_cast<QGraphicsPixmapItem *>(a_item);
    auto property = qobject_cast<GameProperty_human *>(a_property);
    
    Q_CHECK_PTR(item);
    Q_CHECK_PTR(property);
    
    Util::Direction dir = property->direction();
    const AnimatedTexture &texture = textures_[dir];
    int stage = property->stage();
    
    QPointF pos = geometry()->coordinateToTopLeft(property->gameObject()->position());
    Coordinate delta = Coordinate(0, 0).applyDirection(dir);
    QPointF scene_delta = geometry()->offset(delta);
    QPointF offs = texture.offset;
    Coordinate z_offs = texture.z_offset;
    
    int frame_id = stage;
    if (frame_id > 5) {
        frame_id -= 5;
        offs += scene_delta / 2;
    }
    
    if (stage != 0) {
        z_offs -= delta;
        offs -= scene_delta;
    }
    
    item->setPixmap(texture.frames[frame_id]);
    item->setOffset(offs);
    item->setPos(pos);
    item->setZValue(geometry()->zOrder(z_offs));
}

void GamePropertyRenderer_human::loadTextures()
{
    if (textures_loaded_) {
        return;
    }
    
    int item_count = direction_meta_.keyCount();
    for (int i = 0; i < item_count; ++i) {
        QString dir_name = QString(direction_meta_.key(i)).toLower();
        auto texture = textures()->getTexture(QStringLiteral("human-anim-%1").arg(dir_name));
        
        const int frame_count = MAX_FRAME + 1;
        const int frame_h = texture->pixmap.height();
        const int frame_w = texture->pixmap.width() / frame_count;
        
        AnimatedTexture anim_texture; 
        anim_texture.offset = texture->offset;
        anim_texture.z_offset = texture->z_offset;
        for (int j = 0; j < frame_count; ++j) {
            anim_texture.frames.append(texture->pixmap.copy(QRect(frame_w * j, 0, frame_w, frame_h)));
        }
        
        textures_.append(anim_texture);
    }
    textures_loaded_ = true;
}
