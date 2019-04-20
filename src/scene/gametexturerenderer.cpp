#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QLayout>
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
            QPen pen(QColor(0, 0, 0, 32));
            pen.setWidth(1.0);
            scene()->addPolygon(geometry()->coordinateToPoly({i, j}), pen, brush)->setZValue(-1e9);
        }
    }
}

QGraphicsItem *GameTextureRenderer::drawTexture(const QString &name, const Coordinate &c)
{
    const GameTexture *texture = textures()->getTexture(name);
    Q_CHECK_PTR(texture);
    QGraphicsPixmapItem *item = scene()->addPixmap(texture->pixmap);
    item->setPos(texture->offset + geometry()->coordinateToTopLeft(c));
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
    auto info = repository()->getRenderInfo(object);
    QList<QGraphicsItem *> items;
    for (const QString &texture_name : info->textures) {
        items.push_back(drawTexture(texture_name, object->position()));
    }
    
    if (object->property() != nullptr) {
        auto add_items = prop_render_->drawProperty(object->property());
        items.append(add_items);
    }
    
    for (QGraphicsItem *item : qAsConst(items)) {
        item->setData(DATA_KEY_GAMEOBJECT, QVariant::fromValue(object));
        item->setData(DATA_KEY_BASE_Z_VALUE, QVariant::fromValue(item->zValue()));
        item->setZValue(item->zValue() + geometry()->zOrder(object->position(), info->priority));
    }
    
    return items;
}

void GameTextureRenderer::moveObject(GameObject *object, const Coordinate &old_pos, const QList<QGraphicsItem *> &items)
{
    for (auto item : items) {
        moveTexture(item, old_pos, object->position());
    }
}

void GameTextureRenderer::updateObject(GameObject *, const QList<QGraphicsItem *> &items)
{
    for (QGraphicsItem *item : qAsConst(items)) {
        auto property = qvariant_cast<GameObjectProperty *>(item->data(DATA_KEY_PROPERTY));
        if (property == nullptr) {
            continue;
        }
        double old_z = qvariant_cast<double>(item->data(DATA_KEY_BASE_Z_VALUE));
        double cur_z = item->zValue();
        item->setZValue(old_z);
        prop_render_->updatePropertyItem(item, property);
        double new_z = item->zValue();
        item->setData(DATA_KEY_BASE_Z_VALUE, QVariant::fromValue(new_z));
        item->setZValue(cur_z - old_z + new_z);
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

QGraphicsItem *GameTextureRenderer::drawSelectionRect(GameObject *object)
{
    QColor brush_color, pen_color;
    if (object->canApplyMovingPosition()) {
        brush_color = QColor(0, 192, 0, 160);
        pen_color = QColor(0, 128, 0, 160);
    } else {
        brush_color = QColor(255, 0, 0, 160);
        pen_color = QColor(192, 0, 0, 160);
    }

    QPen pen(pen_color);
    pen.setWidth(2);

    QPolygonF poly;
    for (Coordinate delta : object->cellsRelative()) {
        poly = poly.united(geometry()->selectionPolygon().translated(geometry()->offset(delta)));
    }

    return scene()->addPolygon(poly, pen, QBrush(brush_color));
}

QGraphicsItem *GameTextureRenderer::drawMoving(GameObject *object)
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

QGraphicsWidget *GameTextureRenderer::drawSelectionControl(const GameObject *object)
{
    QFont font;
    font.setPixelSize(2 * geometry()->cellSize());

    auto parent_widget = new QWidget();
    parent_widget->move({0, 0});
    parent_widget->setFont(font);
    parent_widget->setWindowFlags(Qt::FramelessWindowHint);
    parent_widget->setAttribute(Qt::WA_NoSystemBackground);
    parent_widget->setAttribute(Qt::WA_TranslucentBackground);
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
        
        QPushButton#delete-btn {
            border-color: #A58A87;
            background-color: rgb(248, 232, 228);
        }
        
        QPushButton#delete-btn:pressed {
            background-color: #FF6F70;
        }
        
        QPushButton#delete-btn:pressed:focus {
            border-color: #872525;
        }
        
        QPushButton#delete-btn:focus {
            border-color: #C14848;
        }
    )CSS"));

    auto layout = new QVBoxLayout(parent_widget);

    if (object->property() != nullptr) {
        auto prop_widget = prop_render_->drawControlWidget(object->property());
        if (prop_widget != nullptr) {
            layout->addWidget(prop_widget, 0, Qt::AlignCenter);
        }
    }
    
    auto move_btn = new QPushButton("Move", parent_widget);
    move_btn->setVisible(object->canMove());
    layout->addWidget(move_btn);
    connect(move_btn, &QPushButton::released, object, &GameObject::startMoving);

    auto delete_btn = new QPushButton("Delete", parent_widget);
    delete_btn->setObjectName(QStringLiteral("delete-btn"));
    layout->addWidget(delete_btn);
    connect(delete_btn, &QPushButton::clicked, object, &GameObject::removeSelf);
    
    auto widget_proxy = scene()->addWidget(parent_widget);

    Rect bound_rect = boundingRect(object->cells());
    QPointF top_left = geometry()->coordinateToRect({bound_rect.top, bound_rect.left}).topLeft();
    QPointF bottom_right = geometry()->coordinateToRect({bound_rect.bottom, bound_rect.right}).bottomRight();
    QPointF widget_middle(parent_widget->size().width() / 2, parent_widget->size().height());

    widget_proxy->setZValue(geometry()->controlZDelta());
    widget_proxy->setPos((top_left + bottom_right) / 2 - widget_middle);

    return widget_proxy;
}

GameTextureRenderer::GameTextureRenderer(QObject *parent, GameSceneGeometry *geometry,
        GameTextureRepository *textures, GameObjectRenderRepository *repository, QGraphicsScene *scene)
    : GameTextureRendererBase(parent, geometry, textures, repository, scene),
      prop_render_(new GamePropertyRenderer(this, this))
{
    prop_render_->addRenderer(QStringLiteral("GameProperty_house"), new GamePropertyRenderer_house(this));
    prop_render_->addRenderer(QStringLiteral("GameProperty_human"), new GamePropertyRenderer_human(this));
}
