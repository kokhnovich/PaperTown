#include <QModelIndex>
#include <QIcon>
#include <QtDebug>
#include <algorithm>
#include "gameobjectpickermodel.h"

GameObjectKey GameObjectPickerModel::indexToObjectKey(const QModelIndex &index) const
{
    int idx = index.row();
    Q_ASSERT(0 <= idx && idx < keys_.size());
    return keys_[idx];
}

QVariant GameObjectPickerModel::data(const QModelIndex &index, int role) const
{
    auto key = indexToObjectKey(index);
    auto value = repository_->getRenderInfo(key.type, key.name);
    switch (role) {
    case Qt::DisplayRole: {
        return QVariant(value->caption);
    }
    case Qt::DecorationRole: {
        return QVariant(QIcon(textures_->getTexture(key.name)->pixmap));
    }
    default: {
        return QVariant();
    }
    }
}

Qt::ItemFlags GameObjectPickerModel::flags(const QModelIndex &) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

int GameObjectPickerModel::columnCount(const QModelIndex &) const
{
    return 1;
}

int GameObjectPickerModel::rowCount(const QModelIndex &) const
{
    return keys_.size();
}

QModelIndex GameObjectPickerModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

GameObjectPickerModel::GameObjectPickerModel(GameObjectRenderRepository *repository, GameTextureRepository *textures,
        QObject *parent)
    : QAbstractListModel(parent), repository_(repository), textures_(textures)
{
    keys_ = repository_->keys();
    std::sort(keys_.begin(), keys_.end(), [](const GameObjectKey &key1, const GameObjectKey &key2) {
        return std::make_pair(key1.type, key1.name) < std::make_pair(key2.type, key2.name);
    });
}

void GameObjectSelectionModel::changeSelection(const QItemSelection &selected, const QItemSelection &)
{
    auto picker_model = qobject_cast<GameObjectPickerModel *>(model());
    if (!picker_model) {
        return;
    }
    for (auto index : selected.indexes()) {
        GameObjectKey key = picker_model->indexToObjectKey(index);
        auto object = field_->add(key.type, key.name);
        
        auto object_unselect = [this, index]() {
            this->select(index, SelectionFlag::Deselect);
        };
        
        connect(object, &GameObject::placed, this, object_unselect);
        connect(object, &GameObject::declined, this, object_unselect);
    }
}

GameObjectSelectionModel::GameObjectSelectionModel(GameField *field, QObject *parent)
    : QItemSelectionModel(nullptr, parent), field_(field)
{
    connect(this, &GameObjectSelectionModel::selectionChanged, this, &GameObjectSelectionModel::changeSelection);
}
