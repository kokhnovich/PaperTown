#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QLayout>
#include <QPointer>
#include <QGraphicsGridLayout>
#include "gametexturerenderer.h"
#include "stdpropertyrenderers.h"

void GameTextureRenderer::setupScene()
{
    scene()->setSceneRect(geometry()->fieldRect());

    QBrush light_brush(QPixmap(":/img/cell.png"));
    QBrush dark_brush(QPixmap(":/img/cell-dark.png"));

    QPen border_pen(QColor(255, 0, 0, 128));
    border_pen.setWidth(6.0);

    scene()->setBackgroundBrush(dark_brush);
    scene()->addPolygon(geometry()->fieldActivePolygon(), border_pen, light_brush)->setZValue(-1e9);

    for (int i = 0; i < geometry()->fieldHeight(); ++i) {
        for (int j = 0; j < geometry()->fieldWidth(); ++j) {
            QBrush brush(QColor(255.0 / geometry()->fieldHeight() * i, 255.0 / geometry()->fieldWidth() * j, 0, 0));
            QPen pen(QColor(224, 224, 224, 255));
            pen.setWidth(1.0);
            scene()->addPolygon(geometry()->coordinateToPoly({i, j}), pen, brush)->setZValue(-1e9);
        }
    }
}

QGraphicsItem *GameTextureRenderer::drawTexture(const QString &name)
{
    const GameTexture *texture = textures()->getTexture(name);
    Q_CHECK_PTR(texture);
    QGraphicsPixmapItem *item = scene()->addPixmap(texture->pixmap);
    item->setOffset(texture->offset);
    item->setZValue(geometry()->zOrder(texture->z_offset));
    return item;
}

void GameTextureRenderer::moveTexture(QGraphicsItem *item, const Coordinate &old_pos, const Coordinate &new_pos)
{
    item->setPos(item->pos() + geometry()->offset(new_pos - old_pos));
    item->setZValue(item->zValue() + geometry()->zOrder(new_pos) - geometry()->zOrder(old_pos));
}

QList<QGraphicsItem *> GameTextureRenderer::drawObject(GameObject *object)
{
    bool show_items = true;
    if (object->property() != nullptr) {
        show_items = mergeBooleans(show_items, prop_render_->canShowMainObject(object->property()));
    }
    
    auto info = repository()->getRenderInfo(object);
    QList<QGraphicsItem *> items;
    for (const QString &texture_name : info->textures) {
        QGraphicsItem *item = drawTexture(texture_name);
        items.push_back(item);
        item->setVisible(show_items);
        item->setData(DATA_KEY_IS_OBJECT_ITEM, QVariant::fromValue(true));
    }

    if (object->property() != nullptr) {
        auto add_items = prop_render_->drawProperty(object->property());
        items.append(add_items);
    }

    for (QGraphicsItem *item : qAsConst(items)) {
        item->setData(DATA_KEY_GAMEOBJECT, QVariant::fromValue(object));
        item->setData(DATA_KEY_BASE_Z_VALUE, QVariant::fromValue(item->zValue()));
        item->setZValue(item->zValue() + geometry()->zOrderOffset(object->position()) + info->priority);
        item->setPos(item->pos() + geometry()->coordinateToTopLeft(object->position()));
    }

    return items;
}

void GameTextureRenderer::moveObject(GameObject *object, const Coordinate &old_pos, const QList<QGraphicsItem *> &items)
{
    for (auto item : items) {
        moveTexture(item, old_pos, object->position());
    }
}

void GameTextureRenderer::moveSelectionControl(QGraphicsWidget *widget, const Coordinate &old_pos, const Coordinate &new_pos)
{
    widget->setPos(widget->pos() + geometry()->offset(new_pos - old_pos));
}

void GameTextureRenderer::updateObject(GameObject *object, const QList<QGraphicsItem *> &items)
{
    bool show_items = true;
    if (object->property() != nullptr) {
        show_items = mergeBooleans(show_items, prop_render_->canShowMainObject(object->property()));
    }
    
    for (QGraphicsItem *item : qAsConst(items)) {
        auto property = qvariant_cast<GameObjectProperty *>(item->data(DATA_KEY_PROPERTY));
        if (property != nullptr) {
            double old_z = qvariant_cast<double>(item->data(DATA_KEY_BASE_Z_VALUE));
            double cur_z = item->zValue();
            item->setZValue(old_z);
            prop_render_->updatePropertyItem(item, property);
            double new_z = item->zValue();
            item->setData(DATA_KEY_BASE_Z_VALUE, QVariant::fromValue(new_z));
            item->setZValue(cur_z - old_z + new_z);
        } else if (item->data(DATA_KEY_IS_OBJECT_ITEM).toBool()) {
            item->setVisible(show_items);
        } else {
            qCritical() << "could not detect the origin of the specified QGraphicsItem!";
            Q_UNREACHABLE();
        }
    }
}

void GameTextureRenderer::changeObjectSelectionState(GameObject *, const QList<QGraphicsItem *> &items,
        SelectionState old_state, SelectionState new_state)
{
    for (QGraphicsItem *item : items) {
        qreal opacity = 1.0;
        switch (new_state) {
        case SelectionState::None: {
            opacity = 1.0;
            break;
        }
        case SelectionState::Moving: {
            opacity = 0.5;
            break;
        }
        case SelectionState::Selected: {
            opacity = 0.75;
            break;
        }
        default: {
            Q_UNREACHABLE();
        }
        }
        item->setOpacity(opacity);

        double zdelta = geometry()->selectionStateZDelta(new_state) - geometry()->selectionStateZDelta(old_state);
        if (!qFuzzyIsNull(zdelta)) {
            item->setZValue(item->zValue() + zdelta);
        }
    }
}

class QGraphicsSelectionRectItem : public QGraphicsPolygonItem
{
public:
    QGraphicsSelectionRectItem(GameObject *object);
    ~QGraphicsSelectionRectItem() override;
protected:
    void updateBrushPen();
private:
    GameObject *object_;
    QPointer<QTimer> timer_;

    const QColor SELECTION_BRUSH_COLORS[2] = {
        QColor(255, 0, 0, 160), // false
        QColor(0, 192, 0, 160)  // true
    };

    const QColor SELECTION_PEN_COLORS[2] = {
        QColor(192, 0, 0, 160), // false
        QColor(0, 128, 0, 160)  // true
    };
};

QGraphicsSelectionRectItem::QGraphicsSelectionRectItem(GameObject *object)
    : object_(object), timer_(new QTimer)
{
    updateBrushPen();
    timer_->setInterval(40);
    QObject::connect(timer_, &QTimer::timeout, timer_, [this]() {
        if (!object_->isMoving()) {
            timer_->deleteLater();
            return;
        }
        updateBrushPen();
    });
    timer_->start();
}

QGraphicsSelectionRectItem::~QGraphicsSelectionRectItem()
{
    delete timer_;
}

void QGraphicsSelectionRectItem::updateBrushPen()
{
    QBrush brush(Qt::SolidPattern);
    QPen pen;
    pen.setWidth(2);
    bool can_place = object_->canApplyMovingPosition();
    brush.setColor(SELECTION_BRUSH_COLORS[can_place]);
    pen.setColor(SELECTION_PEN_COLORS[can_place]);
    setBrush(brush);
    setPen(pen);
}

QGraphicsItem *GameTextureRenderer::drawSelectionRect(GameObject *object)
{
    QPolygonF poly;
    for (Coordinate delta : object->cellsRelative()) {
        poly = poly.united(geometry()->selectionPolygon().translated(geometry()->offset(delta)));
    }

    auto item = new QGraphicsSelectionRectItem(object);
    item->setPolygon(poly);
    scene()->addItem(item);

    return item;
}

QGraphicsItem *GameTextureRenderer::drawMovingItem(GameObject *object)
{
    const GameTexture *texture = textures()->getTexture(object->name());

    QGraphicsPixmapItem *pixmap_item = scene()->addPixmap(texture->pixmap);
    pixmap_item->setOffset(texture->offset);
    pixmap_item->setZValue(1.0);

    QList<QGraphicsItem *> group_items;
    group_items.append(pixmap_item);
    group_items.append(drawSelectionRect(object));

    QGraphicsItemGroup *group = scene()->createItemGroup(group_items);
    group->setPos(geometry()->coordinateToTopLeft(object->movingPosition()));
    group->setZValue(geometry()->movingZDelta());

    return group;
}

void GameTextureRenderer::moveMovingItem(GameObject *object, QGraphicsItem *item)
{
    item->setPos(geometry()->coordinateToTopLeft(object->movingPosition()));
}

QGraphicsWidget *GameTextureRenderer::createSelectionControl(const GameObject *object)
{
    QFont font;
    font.setPixelSize(2 * geometry()->cellSize());
    
    auto outer_widget = new QWidget();
    auto outer_layout = new QVBoxLayout(outer_widget);
    outer_layout->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    outer_widget->setMinimumWidth(1000);
    outer_widget->setWindowFlags(Qt::FramelessWindowHint);
    outer_widget->setAttribute(Qt::WA_NoSystemBackground);
    outer_widget->setAttribute(Qt::WA_TranslucentBackground);
    
    auto parent_widget = new QWidget();
    outer_layout->addWidget(parent_widget);
    parent_widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    parent_widget->move({0, 0});
    parent_widget->setFont(font);
    parent_widget->setWindowFlags(Qt::FramelessWindowHint);
    parent_widget->setAttribute(Qt::WA_NoSystemBackground);
    parent_widget->setAttribute(Qt::WA_TranslucentBackground);
    parent_widget->setObjectName("inner-widget");
    parent_widget->setStyleSheet(QStringLiteral(R"CSS(
        QPushButton {
            border: 5px groove #878AB5;
            border-radius: 12px;
            padding: 4px;
            background-color: rgb(242, 246, 248);
        }
        
        QPushButton:pressed {
            border-style: ridge;
            background-color: rgb(224, 228, 232);
        }
        
        QPushButton:focus {
            border-color: #5E6596;
        }
        
        QPushButton[btn_style="red"] {
            border-color: #A58A87;
            background-color: rgb(248, 232, 228);
        }
        
        QPushButton[btn_style="red"]:pressed {
            background-color: #FF6F70;
        }
        
        QPushButton[btn_style="red"]:pressed:focus {
            border-color: #872525;
        }
        
        QPushButton[btn_style="red"]:focus {
            border-color: #C14848;
        }
        
        QPushButton[btn_style="green"] {
            border-color: #8AA587;
            background-color: rgb(232, 248, 228);
        }
        
        QPushButton:disabled {
            border-color: #7A7A7A;
            background-color: #E5E5E5;
        }
        
        QPushButton[btn_style="green"]:pressed {
            background-color: #6FFF70;
        }
        
        QPushButton[btn_style="green"]:pressed:focus {
            border-color: #258725;
        }
        
        QPushButton[btn_style="green"]:focus {
            border-color: #48C148;
        }
    )CSS"));

    auto layout = new QVBoxLayout(parent_widget);
    layout->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    
    if (object->property() != nullptr) {
        auto prop_widget = prop_render_->createControlWidget(object->property());
        if (prop_widget != nullptr) {
            prop_widget->setObjectName(QStringLiteral("property-widget"));
            layout->addWidget(prop_widget, 0, Qt::AlignCenter);
        }
    }

    auto move_btn = new QPushButton(tr("Move"), parent_widget);
    move_btn->setObjectName(QStringLiteral("move-btn"));
    layout->addWidget(move_btn);
    connect(move_btn, &QPushButton::released, object, &GameObject::startMoving);

    auto delete_btn = new QPushButton(tr("Delete\n(+%1 $)").arg(object->removalCost(), 0, 'f', 2), parent_widget);
    delete_btn->setObjectName(QStringLiteral("delete-btn"));
    delete_btn->setProperty("btn_style", QVariant("red"));
    layout->addWidget(delete_btn);
    connect(delete_btn, &QPushButton::clicked, object, &GameObject::removeSelf);

    auto widget_proxy = scene()->addWidget(outer_widget);
    
    Rect bound_rect = boundingRect(object->cells());
    QPointF top_left = geometry()->coordinateToRect({bound_rect.top, bound_rect.left}).topLeft();
    QPointF bottom_right = geometry()->coordinateToRect({bound_rect.bottom, bound_rect.right}).bottomRight();
    QPointF widget_middle(outer_widget->size().width() / 2, outer_widget->size().height());
    
    widget_proxy->setZValue(geometry()->controlZDelta());
    widget_proxy->setPos((top_left + bottom_right) / 2 - widget_middle);
    
    return widget_proxy;
}

QGraphicsWidget *GameTextureRenderer::drawSelectionControl(const GameObject *object)
{
    auto widget = createSelectionControl(object);
    updateSelectionControl(object, widget);
    return widget;
}

void GameTextureRenderer::updateSelectionControl(const GameObject *object, QGraphicsWidget *a_widget)
{
    QWidget *outer_widget = qobject_cast<QGraphicsProxyWidget *>(a_widget)->widget();
    auto widget = outer_widget->findChild<QWidget *>(QStringLiteral("inner-widget"), Qt::FindDirectChildrenOnly);
    auto property_widget = widget->findChild<QWidget *>(QStringLiteral("property-widget"), Qt::FindDirectChildrenOnly);
    auto move_btn = widget->findChild<QPushButton *>(QStringLiteral("move-btn"), Qt::FindDirectChildrenOnly);
    Q_CHECK_PTR(move_btn);
    move_btn->setVisible(object->canMove());
    if (property_widget != nullptr) {
        prop_render_->updateControlWidget(object->property(), property_widget);
    }
}

GameTextureRenderer::GameTextureRenderer(QObject *parent, GameSceneGeometry *geometry,
        GameTextureRepository *textures, GameObjectRenderRepository *repository, QGraphicsScene *scene)
    : GameTextureRendererBase(parent, geometry, textures, repository, scene),
      prop_render_(new GamePropertyRenderer(this, this))
{
    prop_render_->addRenderer(QStringLiteral("GameProperty_house"), new GamePropertyRenderer_house(this));
    prop_render_->addRenderer(QStringLiteral("GameProperty_human"), new GamePropertyRenderer_human(this));
    prop_render_->addRenderer(QStringLiteral("GameProperty_passable"), nullptr);
    prop_render_->addRenderer(QStringLiteral("GameProperty_building"), new GamePropertyRenderer_building(this));
    prop_render_->addRenderer(QStringLiteral("GameProperty_ecological"), nullptr);
}
