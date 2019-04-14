#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QLayout>
#include "gametexturerenderer.h"

void GameTextureRenderer::setupScene()
{
    scene_->setSceneRect(geometry_->fieldRect());

    QBrush light_brush(QPixmap(":/img/cell.png"));
    QBrush dark_brush(QPixmap(":/img/cell-dark.png"));

    QPen border_pen(QColor(255, 0, 0, 128));
    border_pen.setWidth(6.0);

    scene_->setBackgroundBrush(dark_brush);
    scene_->addPolygon(geometry_->fieldActivePolygon(), border_pen, light_brush)->setZValue(-1e9);

    for (int i = 0; i < geometry_->fieldHeight(); ++i) {
        for (int j = 0; j < geometry_->fieldWidth(); ++j) {
            QBrush brush(QColor(255.0 / geometry_->fieldHeight() * i, 255.0 / geometry_->fieldWidth() * j, 0, 0));
            QPen pen(QColor(0, 0, 0, 32));
            pen.setWidth(1.0);
            scene_->addPolygon(geometry_->coordinateToPoly({i, j}), pen, brush)->setZValue(-1e9);
        }
    }
}

QGraphicsItem *GameTextureRenderer::drawTexture(const QString &name, const Coordinate &c, qreal priority)
{
    const GameTexture *texture = textures_->getTexture(name);
    Q_CHECK_PTR(texture);
    QGraphicsPixmapItem *item = scene_->addPixmap(texture->pixmap);
    item->setPos(texture->offset + geometry_->coordinateToTopLeft(c));
    item->setZValue(zOrder(c + texture->z_offset, priority));
    return item;
}

void GameTextureRenderer::moveTexture(QGraphicsItem *item, const QString &name, const Coordinate &c, qreal priority)
{
    const GameTexture *texture = textures_->getTexture(name);
    Q_CHECK_PTR(texture);
    item->setPos(texture->offset + geometry_->coordinateToTopLeft(c));
    item->setZValue(zOrder(c + texture->z_offset, priority));
}

QList<QGraphicsItem *> GameTextureRenderer::drawObject(GameObject *object)
{
    auto info = repository_->getRenderInfo(object);
    QList<QGraphicsItem *> items;
    for (const QString &texture_name : info->textures) {
        QGraphicsItem *item = drawTexture(texture_name, object->position(), info->priority);
        item->setData(DATA_KEY_GAMEOBJECT, QVariant::fromValue(object));
        item->setData(DATA_KEY_TEXTURE_NAME, QVariant::fromValue(texture_name));
        item->setData(DATA_KEY_PRIORITY, QVariant::fromValue(info->priority));
        items.push_back(item);
    }
    return items;
}

void GameTextureRenderer::moveObject(GameObject *object, const QList<QGraphicsItem *> &items)
{
    for (auto item : items) {
        moveTexture(item,
                    item->data(DATA_KEY_TEXTURE_NAME).toString(),
                    object->position(),
                    item->data(DATA_KEY_PRIORITY).toReal());
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
        poly = poly.united(geometry_->selectionPolygon().translated(geometry_->offset(delta)));
    }

    return scene_->addPolygon(poly, pen, QBrush(brush_color));
}

QGraphicsItem *GameTextureRenderer::drawMoving(GameObject *object)
{
    const GameTexture *texture = textures_->getTexture(object->name());

    QGraphicsPixmapItem *pixmap_item = scene_->addPixmap(texture->pixmap);
    pixmap_item->setOffset(texture->offset);
    pixmap_item->setZValue(1.0);

    QList<QGraphicsItem *> group_items;
    group_items.append(pixmap_item);
    group_items.append(drawSelectionRect(object));

    QGraphicsItemGroup *group = scene_->createItemGroup(group_items);
    group->setPos(geometry_->coordinateToTopLeft(object->movingPosition()));
    group->setZValue(4e6);

    return group;
}

QGraphicsWidget *GameTextureRenderer::drawControlButtons(const GameObject *object)
{
    QFont font;
    font.setPixelSize(2 * geometry_->cellSize());

    auto parent_widget = new QWidget();
    parent_widget->move({0, 0});
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

    auto move_btn = new QPushButton("Move", parent_widget);
    move_btn->setFont(font);
    move_btn->setVisible(object->canMove());
    layout->addWidget(move_btn);
    connect(move_btn, &QPushButton::released, object, &GameObject::startMoving);

    auto delete_btn = new QPushButton("Delete", parent_widget);
    delete_btn->setFont(font);
    delete_btn->setObjectName(QStringLiteral("delete-btn"));
    layout->addWidget(delete_btn);
    connect(delete_btn, &QPushButton::clicked, object, &GameObject::removeSelf);

    auto widget_proxy = scene_->addWidget(parent_widget);

    Rect bound_rect = boundingRect(object->cells());
    QPointF top_left = geometry_->coordinateToRect({bound_rect.top, bound_rect.left}).topLeft();
    QPointF bottom_right = geometry_->coordinateToRect({bound_rect.bottom, bound_rect.right}).bottomRight();
    QPointF widget_middle(parent_widget->size().width() / 2, parent_widget->size().height());

    widget_proxy->setZValue(3e6);
    widget_proxy->setPos((top_left + bottom_right) / 2 - widget_middle);

    return widget_proxy;
}

QGraphicsScene *GameTextureRenderer::scene()
{
    return scene_;
}

GameTextureRenderer::GameTextureRenderer(QObject *parent, GameSceneGeometry *geometry,
        GameTextureRepository *textures, GameObjectRenderRepository *repository, QGraphicsScene *scene)
    : QObject(parent),
      geometry_(geometry),
      textures_(textures),
      repository_(repository),
      scene_(scene)
{}

qreal GameTextureRenderer::zOrder(const Coordinate &c, qreal priority) const
{
    return (c.x + 1) * geometry_->fieldWidth() - c.y + priority;
}
