#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <QAbstractItemModel>


class OrderBook : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit OrderBook(QObject *parent = 0);

    virtual QModelIndex index(int, int, const QModelIndex&) const;
    virtual QModelIndex parent(const QModelIndex&) const;
    virtual int rowCount(const QModelIndex&) const;
    virtual int columnCount(const QModelIndex&) const;
    virtual QVariant data(const QModelIndex&, int) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

signals:

public slots:

};

#endif // ORDERBOOK_H
