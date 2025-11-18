#ifndef TTRANSPARENCY_EDIT_H
#define TTRANSPARENCY_EDIT_H

#include <QAbstractItemModel>

class Ttransparency_edit : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit Ttransparency_edit(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
};

#endif // TTRANSPARENCY_EDIT_H
