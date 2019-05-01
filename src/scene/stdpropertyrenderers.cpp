#include <QLabel>
#include <QWidget>
#include <QPushButton>
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

QWidget *GamePropertyRenderer_building::createControlWidget(GameObjectProperty *a_property)
{
    auto property = qobject_cast<GameProperty_building *>(a_property);
    
    auto widget = new QWidget;
    auto layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    
    auto inner_widget = new QWidget;
    auto inner_layout = new QHBoxLayout(inner_widget);
    inner_widget->setObjectName("clock-inner-widget");
    layout->addWidget(inner_widget);
    
    inner_widget->setProperty("__last_stage", QVariant(-1));

    auto icon_label = new QLabel;
    icon_label->setObjectName(QStringLiteral("clock-image"));
    inner_layout->addWidget(icon_label);

    auto text_label = new QLabel;
    text_label->setObjectName(QStringLiteral("clock-label"));
    text_label->setFont(label_font_);
    inner_layout->addWidget(text_label);

    QTimer *timer = new QTimer(widget);
    timer->setInterval(100);
    connect(timer, &QTimer::timeout, this, [ = ]() {
        updateTimerWidget(property, widget);
    });
    timer->setObjectName("clock-updater");

    auto repair_btn = new QPushButton(tr("Repair"));
    repair_btn->setObjectName(QStringLiteral("repair-btn"));
    layout->addWidget(repair_btn);
    repair_btn->setProperty("btn_style", "green");
    connect(repair_btn, &QPushButton::pressed, property, &GameProperty_building::startRepairing);
    
    return widget;
}

int GamePropertyRenderer_building::getTextureIndex(double stage) const
{
    return qBound(0, static_cast<int>(qFloor(stage * 12)), 11);
}

QColor GamePropertyRenderer_building::getTextColor(double stage) const
{
    return QColor::fromHsvF(stage / 3.75, 0.8, 0.5);
}

void GamePropertyRenderer_building::updateTimerWidget(GameObjectProperty *a_property, QWidget *widget)
{
    auto property = qobject_cast<GameProperty_building *>(a_property);
    
    auto timer = widget->findChild<QTimer *>(QStringLiteral("clock-updater"));
    auto inner_widget = widget->findChild<QWidget *>(QStringLiteral("clock-inner-widget"));
    
    if (!property->isBuildInProgress()) {
        if (timer->isActive()) {
            timer->stop();
        }
        inner_widget->setVisible(false);
        return;
    }

    if (!timer->isActive()) {
        timer->start();
    }
    inner_widget->setVisible(true);

    double progress = property->buildProgress();
    int last_stage = inner_widget->property("__last_stage").toInt();
    int next_stage = getTextureIndex(progress);
    auto icon_label = inner_widget->findChild<QLabel *>(QStringLiteral("clock-image"));
    auto text_label = inner_widget->findChild<QLabel *>(QStringLiteral("clock-label"));

    QPalette palette = text_label->palette();
    palette.setColor(QPalette::WindowText, getTextColor(progress));
    text_label->setPalette(palette);

    if (last_stage != next_stage) {
        icon_label->setPixmap(small_textures_[next_stage]);
        inner_widget->setProperty("__last_stage", QVariant::fromValue(next_stage));
    }

    text_label->setText(QTime::fromMSecsSinceStartOfDay(property->remainingBuildTime()).toString("HH:mm:ss"));
}

void GamePropertyRenderer_building::updateControlWidget(GameObjectProperty *a_property, QWidget *widget)
{
    auto property = qobject_cast<GameProperty_building *>(a_property);

    updateTimerWidget(a_property, widget);
    
    auto repair_btn = widget->findChild<QPushButton *>(QStringLiteral("repair-btn"));
    Q_CHECK_PTR(repair_btn);
    repair_btn->setVisible(property->canStartRepairing());
}

class BuildingTimerItem : public QGraphicsPixmapItem
{
public:
    enum { Type = UserType + 1 };
    
    BuildingTimerItem(GamePropertyRenderer_building *renderer, GameProperty_building *property)
        : timer_(new QTimer), property_(property), renderer_(renderer) {
        timer_->setInterval(100);
        QObject::connect(timer_, &QTimer::timeout, timer_, [ = ]() {
            updateTexture();
        });
        updateState();
        updateTexture();
    
        QSize img_size = renderer_->images_[0].size();
        setOffset(-img_size.width() / 2, -img_size.height() / 2 - renderer_->geometry()->cellSize());
    }
    
    void updateState() {
        bool is_active = timer_->isActive();
        bool needs_active = property_->isBuildInProgress();
        if (is_active && !needs_active) {
            timer_->stop();
        }
        if (!is_active && needs_active) {
            timer_->start();
        }
        setVisible(needs_active);
    }
    
    int type() const override {
        return Type;
    }
    
    ~BuildingTimerItem() override {
        delete timer_;
    }
protected:
    void updateTexture() {
        int new_index = renderer_->getTextureIndex(property_->buildProgress());
        if (texture_index_ != new_index) {
            texture_index_ = new_index;
            setPixmap(renderer_->images_[new_index]);
        }
    }
private:
    int texture_index_ = -1;
    QTimer *timer_;
    GameProperty_building *property_;
    GamePropertyRenderer_building *renderer_;
};

QList<QGraphicsItem *> GamePropertyRenderer_building::doDrawProperty(GameObjectProperty *a_property)
{
    auto property = qobject_cast<GameProperty_building *>(a_property);

    const QVector<Border> borders = calcBorders(property->gameObject()->cellsRelative());

    const GameTexture *horz_border = textures()->getTexture(QStringLiteral("construction-horz"));
    const GameTexture *vert_border = textures()->getTexture(QStringLiteral("construction-vert"));

    QList<QGraphicsItem *> items;
    
    for (const Border &border : borders) {
        const GameTexture *border_texture = (border.side == Util::Up || border.side == Util::Down) ? horz_border : vert_border;
        
        QPointF offset = border_texture->offset;
        if (border.side == Util::Up) {
            offset += geometry()->offset({-1, 0});
        } else if (border.side == Util::Right) {
            offset += geometry()->offset({0, 1});
        }
        
        auto item = scene()->addPixmap(border_texture->pixmap);
        item->setOffset(offset);
        item->setPos(geometry()->coordinateToTopLeft(border.cell + property->gameObject()->position()));
        item->setZValue(geometry()->borderZOrder({border.cell + border_texture->z_offset, border.side}));
        item->setVisible(property->isBuildInProgress());
        item->setData(DATA_KEY_PROP_FLAGS, QVariant::fromValue(PROP_FLAG_BORDER));
        
        items.append(item);
    }
    
    {
        Rect bound_rect = boundingRect(property->gameObject()->cells());
        QPointF top_left = geometry()->coordinateToRect(bound_rect.topLeft()).topLeft();
        QPointF bottom_right = geometry()->coordinateToRect(bound_rect.bottomRight()).bottomRight();
        
        qreal z_value = geometry()->borderZOrder({bound_rect.bottomLeft(), Util::Down}) + 0.1;
        z_value -= geometry()->zOrderOffset(property->gameObject()->position());
        
        auto timer_item = new BuildingTimerItem(this, property);
        timer_item->setPos((top_left + bottom_right) / 2);
        timer_item->setZValue(z_value);
        timer_item->setData(DATA_KEY_PROP_FLAGS, QVariant::fromValue(PROP_FLAG_TIMER));
        scene()->addItem(timer_item);
        items.append(timer_item);
        
        auto wrecked_indicator = scene()->addPixmap(wrecked_icon_);
        wrecked_indicator->setPos((top_left + bottom_right) / 2);
        wrecked_indicator->setOffset(-wrecked_icon_.width() / 2, -geometry()->cellSize() - wrecked_icon_.height() / 2);
        wrecked_indicator->setZValue(z_value);
        wrecked_indicator->setData(DATA_KEY_PROP_FLAGS, QVariant::fromValue(PROP_FLAG_WRECKED));
        wrecked_indicator->setVisible(property->state() == GameProperty_building::Wrecked);
        items.append(wrecked_indicator);
    }
        
    return items;
}

void GamePropertyRenderer_building::updatePropertyItem(QGraphicsItem *item, GameObjectProperty *a_property)
{
    auto property = qobject_cast<GameProperty_building *>(a_property);
    switch (item->data(DATA_KEY_PROP_FLAGS).toInt()) {
        case PROP_FLAG_BORDER: {
            item->setVisible(property->isBuildInProgress());
            break;
        }
        case PROP_FLAG_TIMER: {
            auto timer_item = qgraphicsitem_cast<BuildingTimerItem *>(item);
            Q_CHECK_PTR(timer_item);
            timer_item->setVisible(property->isBuildInProgress());
            timer_item->updateState();
            break;
        }
        case PROP_FLAG_WRECKED: {
            item->setVisible(property->state() == GameProperty_building::Wrecked);
            break;
        }
        default: {
            // unknown item
            Q_UNREACHABLE();
        }
    }
}

Util::Bool3 GamePropertyRenderer_building::canShowMainObject(GameObjectProperty *a_property)
{
    auto property = qobject_cast<GameProperty_building *>(a_property);
    if (property->state() == GameProperty_building::UnderConstruction) {
        return Util::False;
    }
    return Util::Dont_Care;
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
            images_.append(pixmap.copy(j * texture_width, i * texture_height, texture_width, texture_height));
            small_textures_.append(images_.back().scaled(texture_height / 2, texture_width / 2));
        }
    }
    wrecked_icon_ = QPixmap(":/img/icon-repair.png");
}

GamePropertyRenderer_building::GamePropertyRenderer_building(GameTextureRendererBase *renderer)
    : GameAbstractPropertyRenderer(renderer)
{
    loadTextures();
    label_font_.setWeight(QFont::Bold);
    label_font_.setPixelSize(0.75 * small_textures_[0].height());
}
