#include "ttransparency_edit.h"

Ttransparency_edit::Ttransparency_edit(QObject *parent)
    : QAbstractItemModel(parent)
{
}

QVariant Ttransparency_edit::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
}

QModelIndex Ttransparency_edit::index(int row, int column, const QModelIndex &parent) const
{
    // FIXME: Implement me!
}

QModelIndex Ttransparency_edit::parent(const QModelIndex &index) const
{
    // FIXME: Implement me!
}

int Ttransparency_edit::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

int Ttransparency_edit::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

QVariant Ttransparency_edit::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    return QVariant();
}
