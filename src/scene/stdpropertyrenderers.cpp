#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>
#include "../scene/stdpropertyrenderers.h"
#include "../objects/stdproperties.h"

const int MAX_FRAME = 5;

GamePropertyRenderer_house::GamePropertyRenderer_house(GameTextureRendererBase *renderer)
    : GameAbstractPropertyRenderer(renderer),
      image_(QStringLiteral(":/img/icon-population.png"))
{
    label_font_.setWeight(QFont::Bold);
    label_font_.setPixelSize(0.75 * image_.height());
    label_palette_.setColor(QPalette::WindowText, QColor(137, 48, 20));
}

QWidget *GamePropertyRenderer_house::createControlWidget(GameObjectProperty *)
{
    auto widget = new QWidget;
    auto layout = new QHBoxLayout(widget);

    auto icon_label = new QLabel;
    icon_label->setPixmap(image_);
    layout->addWidget(icon_label);

    auto text_label = new QLabel;
    text_label->setObjectName(QStringLiteral("population-label"));
    text_label->setFont(label_font_);
    text_label->setPalette(label_palette_);
    layout->addWidget(text_label);

    return widget;
}

void GamePropertyRenderer_house::updateControlWidget(GameObjectProperty *a_property, QWidget *widget)
{
    auto property = qobject_cast<GameProperty_house *>(a_property);
    auto label = widget->findChild<QLabel *>(QStringLiteral("population-label"));
    Q_CHECK_PTR(label);
    label->setText(QString::number(property->population()));
}

GamePropertyRenderer_human::GamePropertyRenderer_human(GameTextureRendererBase *renderer)
    : GameAbstractPropertyRenderer(renderer),
      direction_meta_(QMetaEnum::fromType<Util::Direction>()),
      textures_loaded_(false)
{}

QList<QGraphicsItem *> GamePropertyRenderer_human::doDrawProperty(GameObjectProperty *a_property)
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
    item->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
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

QWidget *GamePropertyRenderer_building::createControlWidget(GameObjectProperty *property)
{
    auto widget = new QWidget;
    auto layout = new QHBoxLayout(widget);

    widget->setProperty("__last_stage", QVariant(-1));

    auto icon_label = new QLabel;
    icon_label->setObjectName(QStringLiteral("clock-image"));
    layout->addWidget(icon_label);

    auto text_label = new QLabel;
    text_label->setObjectName(QStringLiteral("clock-label"));
    text_label->setFont(label_font_);
    layout->addWidget(text_label);

    QTimer *timer = new QTimer(widget);
    timer->setInterval(100);
    connect(timer, &QTimer::timeout, this, [ = ]() {
        updateControlWidget(property, widget);
    });
    timer->setObjectName("clock-updater");

    return widget;
}

int GamePropertyRenderer_building::getTextureIndex(double stage) const
{
    return qBound(0, static_cast<int>(qFloor(stage * 12)), 11);
}

void GamePropertyRenderer_building::updateControlWidget(GameObjectProperty *a_property, QWidget *widget)
{
    auto property = qobject_cast<GameProperty_building *>(a_property);

    auto timer = widget->findChild<QTimer *>(QStringLiteral("clock-updater"));

    if (property->gameObject()->isEnabled()) {
        if (timer->isActive()) {
            timer->stop();
        }
        widget->setVisible(false);
        return;
    }

    if (!timer->isActive()) {
        timer->start();
    }
    widget->setVisible(true);
    
    int last_stage = widget->property("__last_stage").toInt();
    int next_stage = getTextureIndex(property->buildProgress());
    auto icon_label = widget->findChild<QLabel *>(QStringLiteral("clock-image"));
    auto text_label = widget->findChild<QLabel *>(QStringLiteral("clock-label"));

    if (last_stage != next_stage) {
        icon_label->setPixmap(small_textures_[next_stage]);
        widget->setProperty("__last_stage", QVariant::fromValue(next_stage));
    }

    text_label->setText(QTime::fromMSecsSinceStartOfDay(property->remainingBuildTime()).toString("HH:mm:ss"));
}

QList<QGraphicsItem *> GamePropertyRenderer_building::doDrawProperty(GameObjectProperty *property)
{
    return {};
}

void GamePropertyRenderer_building::updatePropertyItem(QGraphicsItem *item, GameObjectProperty *property)
{
}

Util::Bool3 GamePropertyRenderer_building::canShowMainObject(GameObjectProperty *property)
{
    return Util::Dont_Care;
    //return property->gameObject()->isEnabled();
}

void GamePropertyRenderer_building::loadTextures()
{
    const int GRID_HEIGHT = 3, GRID_WIDTH = 4;
    QPixmap pixmap(":/img/clock.png");
    Q_ASSERT(!pixmap.isNull());
    const int texture_height = pixmap.height() / GRID_HEIGHT;
    const int texture_width = pixmap.width() / GRID_WIDTH;
    for (int i = 0; i < GRID_HEIGHT; ++i) {
        for (int j = 0; j < GRID_WIDTH; ++j) {
            textures_.append(pixmap.copy(j * texture_width, i * texture_height, texture_width, texture_height));
            small_textures_.append(textures_.back().scaled(texture_height / 2, texture_width / 2));
        }
    }
}

GamePropertyRenderer_building::GamePropertyRenderer_building(GameTextureRendererBase *renderer)
    : GameAbstractPropertyRenderer(renderer)
{
    loadTextures();
    label_font_.setWeight(QFont::Bold);
    label_font_.setPixelSize(0.75 * small_textures_[0].height());
}
