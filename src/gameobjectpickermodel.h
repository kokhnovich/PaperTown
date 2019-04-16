#ifndef GAMEOBJECTPICKERMODEL_H
#define GAMEOBJECTPICKERMODEL_H

#include <QAbstractItemModel>
#include <QItemSelectionModel>
#include <QVector>
#include "core/gamefield.h"
#include "scene/gameobjectrenderrepository.h"
#include "scene/gametextures.h"

// FIXME : support dynamic adding of new objects
class GameObjectPickerModel : public QAbstractListModel
{
    Q_OBJECT
public:
    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    int columnCount(const QModelIndex &parent) const override;
    int rowCount(const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    GameObjectKey indexToObjectKey(const QModelIndex &index) const;
    GameObjectPickerModel(GameObjectRenderRepository *repository, GameTextureRepository *textures,
                          QObject *parent = nullptr);
private:
    GameObjectRenderRepository *repository_;
    GameTextureRepository *textures_;
    QVector<GameObjectKey> keys_;
};

class GameObjectSelectionModel : public QItemSelectionModel
{
    Q_OBJECT
public:
    GameObjectSelectionModel(GameField *field, QObject *parent = nullptr);
protected slots:
    void changeSelection(const QItemSelection &selected, const QItemSelection &deselected);
private:
    GameField *field_;
    QAbstractItemModel *old_model_;
};

#endif // GAMEOBJECTPICKERMODEL_H
