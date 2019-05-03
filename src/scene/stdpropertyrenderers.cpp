#include <QLabel>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include "../scene/stdpropertyrenderers.h"
#include "../objects/stdproperties.h"
#include "../scene/progressbargraphicsitem.h"
#include "iconizedlabel.h"

const int MAX_FRAME = 5;

GamePropertyRenderer_house::GamePropertyRenderer_house(GameTextureRendererBase *renderer)
    : GameAbstractPropertyRenderer(renderer),
      image_(QStringLiteral(":/img/icon-population.png"))
{
    label_font_.setWeight(QFont::Bold);
    label_font_.setPixelSize(qRound(0.75 * image_.height()));
    label_palette_.setColor(QPalette::WindowText, QColor(137, 48, 20));
}

QWidget *GamePropertyRenderer_house::createControlWidget(GameObjectProperty *)
{
    auto widget = new IconizedLabel;
    widget->setPixmap(image_);
    widget->setFont(label_font_);
    widget->setPalette(label_palette_);
    return widget;
}

void GamePropertyRenderer_house::updateControlWidget(GameObjectProperty *a_property, QWidget *widget)
{
    auto property = qobject_cast<GameProperty_house *>(a_property);
    auto label = qobject_cast<IconizedLabel *>(widget);
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
    item->setPos({0, 0});
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
    
    auto timer_widget = new QWidget;
    auto timer_layout = new QHBoxLayout(timer_widget);
    timer_widget->setObjectName("timer-widget");
    layout->addWidget(timer_widget);
    
    timer_widget->setProperty("__last_stage", QVariant(-1));

    auto icon_label = new QLabel;
    icon_label->setObjectName(QStringLiteral("clock-image"));
    timer_layout->addWidget(icon_label);

    auto text_label = new QLabel;
    text_label->setObjectName(QStringLiteral("clock-label"));
    text_label->setFont(label_font_);
    timer_layout->addWidget(text_label);

    QTimer *timer = new QTimer(widget);
    timer->setInterval(250);
    connect(timer, &QTimer::timeout, this, [ = ]() {
        updateControlWidget(property, widget);
    });
    timer->start();

    QFont health_font;
    health_font.setPixelSize(qRound(0.75 * health_icon_.height()));
    health_font.setWeight(QFont::Bold);
    
    auto health_widget = new IconizedLabel;
    layout->addWidget(health_widget);
    health_widget->setObjectName("health-widget");
    health_widget->setPixmap(health_icon_);
    health_widget->setFont(health_font);
    health_widget->setText("?? %");
    
    auto repair_btn = new QPushButton(tr("Repair"));
    repair_btn->setObjectName(QStringLiteral("repair-btn"));
    layout->addWidget(repair_btn);
    repair_btn->setProperty("btn_style", "green");
    connect(repair_btn, &QPushButton::clicked, property, &GameProperty_building::startRepairing);
    connect(repair_btn, &QPushButton::clicked, property->gameObject(), &GameObject::unselect);
    
    return widget;
}

int GamePropertyRenderer_building::getTextureIndex(double stage) const
{
    return qBound(0, qFloor(stage * 12), 11);
}

QColor GamePropertyRenderer_building::getTextColor(double stage) const
{
    return QColor::fromHsvF(stage / 3.75, 0.8, 0.5);
}

void GamePropertyRenderer_building::updateTimerWidget(GameProperty_building *property, QWidget *widget)
{
    auto timer_widget = widget->findChild<QWidget *>(QStringLiteral("timer-widget"));
    
    if (!property->isBuildInProgress()) {
        timer_widget->setVisible(false);
        return;
    }
    
    timer_widget->setVisible(true);

    double progress = property->buildProgress();
    int last_stage = timer_widget->property("__last_stage").toInt();
    int next_stage = getTextureIndex(progress);
    auto icon_label = timer_widget->findChild<QLabel *>(QStringLiteral("clock-image"));
    auto text_label = timer_widget->findChild<QLabel *>(QStringLiteral("clock-label"));

    QPalette palette = text_label->palette();
    palette.setColor(QPalette::WindowText, getTextColor(progress));
    text_label->setPalette(palette);

    if (last_stage != next_stage) {
        icon_label->setPixmap(small_textures_[next_stage]);
        timer_widget->setProperty("__last_stage", QVariant::fromValue(next_stage));
    }

    text_label->setText(QTime::fromMSecsSinceStartOfDay(static_cast<int>(property->remainingBuildTime())).toString("HH:mm:ss"));
}

void GamePropertyRenderer_building::updateHealthWidget(GameProperty_building *property, QWidget *widget)
{
    IconizedLabel *health_display = widget->findChild<IconizedLabel *>(QStringLiteral("health-widget"));
    Q_CHECK_PTR(health_display);
    
    QPalette palette = health_display->palette();
    palette.setColor(QPalette::WindowText, QColor::fromHsvF(property->health() / 3, 0.9, 0.5));
    health_display->setPalette(palette);
    health_display->setText(QStringLiteral("%1%").arg(property->health() * 100.0, 0, 'f', 0));
    health_display->setVisible(property->state() != GameProperty_building::UnderConstruction);
}

void GamePropertyRenderer_building::updateControlWidget(GameObjectProperty *a_property, QWidget *widget)
{
    auto property = qobject_cast<GameProperty_building *>(a_property);

    updateTimerWidget(property, widget);
    updateHealthWidget(property, widget);    
    
    auto repair_btn = widget->findChild<QPushButton *>(QStringLiteral("repair-btn"));
    Q_CHECK_PTR(repair_btn);
    repair_btn->setVisible(property->state() == GameProperty_building::Normal ||
                           property->state() == GameProperty_building::Wrecked);
    repair_btn->setEnabled(property->canStartRepairing());
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
        updateTexture();
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

class HealthBarItem : public ProgressBarGraphicsItem
{
public:
    HealthBarItem(const QPixmap &pm, GameProperty_building *property)
        : ProgressBarGraphicsItem(pm), property_(property) {
        timer_.setInterval(400);
        
        auto on_timer = [ = ]() {
            this->setProgress(property_->health());
        };
        
        QObject::connect(&timer_, &QTimer::timeout, &timer_, on_timer);
        on_timer();
        
        timer_.start();
    }
private:
    QTimer timer_;
    GameProperty_building *property_;
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
        item->setPos(geometry()->offset(border.cell));
        item->setZValue(geometry()->borderZOrder({border.cell + border_texture->z_offset, border.side}));
        item->setData(DATA_KEY_PROP_FLAGS, QVariant::fromValue(PROP_FLAG_BORDER));
        
        items.append(item);
    }
    
    {
        Rect bound_rect = boundingRect(property->gameObject()->cellsRelative());
        QPointF top_left = geometry()->coordinateToRect(bound_rect.topLeft()).topLeft();
        QPointF bottom_right = geometry()->coordinateToRect(bound_rect.bottomRight()).bottomRight();
        
        qreal z_value = geometry()->borderZOrder({bound_rect.bottomLeft(), Util::Down}) + 0.1;
        
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
        items.append(wrecked_indicator);
        
        const GameTexture *cur_object_texture = textures()->getTexture(property->objectName());
        QPointF health_bar_offs(cur_object_texture->pixmap.width() / 2, 0);
        health_bar_offs += geometry()->coordinateToTopLeft({0, 0});
        health_bar_offs += cur_object_texture->offset;
        health_bar_offs -= QPointF(progress_bar_image_.width() / 2, progress_bar_image_.height() / 4);
        
        auto health_bar = new HealthBarItem(progress_bar_image_, property);
        health_bar->setZValue(z_value);
        health_bar->setPos(health_bar_offs);
        health_bar->setData(DATA_KEY_PROP_FLAGS, QVariant::fromValue(PROP_FLAG_HEALTH));
        scene()->addItem(health_bar);
        items.append(health_bar);
    }
    
    for (auto item : qAsConst(items)) {
        updatePropertyItem(item, a_property);
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
            timer_item->updateState();
            break;
        }
        case PROP_FLAG_WRECKED: {
            item->setVisible(property->state() == GameProperty_building::Wrecked);
            break;
        }
        case PROP_FLAG_HEALTH: {
            item->setVisible(property->state() != GameProperty_building::UnderConstruction);
            break;
        }
        default: {
            // unknown item
            //Q_UNREACHABLE();
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
    Q_ASSERT(!wrecked_icon_.isNull());
    progress_bar_image_ = QPixmap(":/img/progress-bars.png");
    Q_ASSERT(!progress_bar_image_.isNull());
    health_icon_ = QPixmap(":/img/icon-health.png");
    Q_ASSERT(!health_icon_.isNull());
}

GamePropertyRenderer_building::GamePropertyRenderer_building(GameTextureRendererBase *renderer)
    : GameAbstractPropertyRenderer(renderer)
{
    loadTextures();
    label_font_.setWeight(QFont::Bold);
    label_font_.setPixelSize(qRound(0.75 * small_textures_[0].height()));
}
