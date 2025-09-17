#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>
#include <QSqlDatabase>
#include <QVector>
#include <QVariant>

class TableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit TableModel(QObject *parent = nullptr);

    void setTable(const QString &tableName, QSqlDatabase db);
    void loadPage(int pageNo);

    int currentPage() const;
    int totalPages() const;
    int pageSize() const;

    // QAbstractTableModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool insertRow(int row, const QModelIndex &parent = QModelIndex());
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool submitAll();
    void revertAll();

private:
    void updateTotalPages();
    void loadColumnHeaders();

    QString m_tableName;
    QSqlDatabase m_db;
    QVector<QVector<QVariant>> m_data; // 当前页缓存
    QVector<QString> m_columnHeaders;
    int m_currentPage = 1;
    int m_totalPages = 1;
    int m_pageSize = 1000;
};

#endif // TABLEMODEL_H