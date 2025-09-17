#include "../include/tablemodel.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>

TableModel::TableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

void TableModel::setTable(const QString &tableName, QSqlDatabase db)
{
    beginResetModel();
    
    m_tableName = tableName;
    m_db = db;
    m_data.clear();
    m_columnHeaders.clear();
    
    // 更新总页数
    updateTotalPages();
    
    // 加载列头信息
    loadColumnHeaders();
    
    // 加载第一页数据
    loadPage(1);
    
    endResetModel();
}

void TableModel::loadPage(int pageNo)
{
    if (pageNo < 1) pageNo = 1;
    if (pageNo > m_totalPages) pageNo = m_totalPages;
    m_currentPage = pageNo;

    beginResetModel();
    
    m_data.clear();
    
    if (m_db.isOpen() && !m_tableName.isEmpty()) {
        QSqlQuery query(m_db);
        QString sql = QString("SELECT * FROM %1 LIMIT %2 OFFSET %3")
                     .arg(m_tableName)
                     .arg(m_pageSize)
                     .arg((m_currentPage - 1) * m_pageSize);
        
        if (query.exec(sql)) {
            while (query.next()) {
                QVector<QVariant> row;
                for (int i = 0; i < query.record().count(); ++i) {
                    row.append(query.value(i));
                }
                m_data.append(row);
            }
        } else {
            qWarning() << "Failed to execute query:" << query.lastError().text();
        }
    }
    
    endResetModel();
}

int TableModel::currentPage() const
{
    return m_currentPage;
}

int TableModel::totalPages() const
{
    return m_totalPages;
}

int TableModel::pageSize() const
{
    return m_pageSize;
}

int TableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_data.size();
}

int TableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_columnHeaders.size();
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();
    
    if (index.row() >= m_data.size() || index.column() >= m_columnHeaders.size())
        return QVariant();
    
    return m_data[index.row()][index.column()];
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    
    if (orientation == Qt::Horizontal) {
        if (section >= 0 && section < m_columnHeaders.size())
            return m_columnHeaders[section];
    } else {
        return section + 1 + (m_currentPage - 1) * m_pageSize;
    }
    
    return QVariant();
}

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || role != Qt::EditRole)
        return false;
    
    if (index.row() >= m_data.size() || index.column() >= m_columnHeaders.size())
        return false;
    
    m_data[index.row()][index.column()] = value;
    m_hasChanges = true;
    emit dataChanged(true);
    
    return true;
}

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

bool TableModel::insertRow(int row, const QModelIndex &parent)
{
    if (parent.isValid())
        return false;
    
    beginInsertRows(parent, row, row);
    
    QVector<QVariant> newRow(m_columnHeaders.size());
    m_data.insert(row, newRow);
    
    m_hasChanges = true;
    emit dataChanged(true);
    
    endInsertRows();
    
    return true;
}

bool TableModel::removeRow(int row, const QModelIndex &parent)
{
    if (parent.isValid() || row < 0 || row >= m_data.size())
        return false;
    
    beginRemoveRows(parent, row, row);
    m_data.remove(row);
    
    m_hasChanges = true;
    emit dataChanged(true);
    
    endRemoveRows();
    
    return true;
}

bool TableModel::submitAll()
{
    if (!m_db.isOpen() || m_tableName.isEmpty()) {
        return false;
    }
    
    // TODO: Implement proper database submission logic for insert/update/delete
    // For now, this is a placeholder that just reloads the data
    loadPage(m_currentPage);
    m_hasChanges = false;
    emit dataChanged(false);
    return true;
}

void TableModel::revertAll()
{
    // Reload current page to discard changes
    loadPage(m_currentPage);
    m_hasChanges = false;
    emit dataChanged(false);
}

void TableModel::updateTotalPages()
{
    if (m_db.isOpen() && !m_tableName.isEmpty()) {
        QSqlQuery query(m_db);
        if (query.exec(QString("SELECT COUNT(*) FROM %1").arg(m_tableName))) {
            if (query.next()) {
                int totalRows = query.value(0).toInt();
                m_totalPages = qMax(1, (totalRows + m_pageSize - 1) / m_pageSize);
            }
        }
    }
}

void TableModel::loadColumnHeaders()
{
    if (m_db.isOpen() && !m_tableName.isEmpty()) {
        QSqlQuery query(m_db);
        if (query.exec(QString("PRAGMA table_info(%1)").arg(m_tableName))) {
            while (query.next()) {
                m_columnHeaders.append(query.value(1).toString()); // column name
            }
        }
    }
}
bool TableModel::hasChanges() const
{
    return m_hasChanges;
}
