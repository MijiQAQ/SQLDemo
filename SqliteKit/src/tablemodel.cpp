#include "tablemodel.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>

TableModel::TableModel(QObject *parent) : QAbstractTableModel(parent) { }

void TableModel::setTable(const QString &tableName, QSqlDatabase db) {
    beginResetModel();

    m_tableName = tableName;
    m_db = db;
    m_data.clear();
    m_columnHeaders.clear();

    updateTotalPages();
    loadColumnHeaders();
    loadPage(1);

    endResetModel();
}

void TableModel::loadPage(int pageNo) {
    if (pageNo < 1)
        pageNo = 1;
    if (pageNo > m_totalPages)
        pageNo = m_totalPages;
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
                RowData row;
                for (int i = 0; i < query.record().count(); ++i) {
                    row.values.append(query.value(i));
                }
                row.state = Unchanged;
                m_data.append(row);
            }
        } else {
            qWarning() << "Failed to execute query:" << query.lastError().text();
        }
    }

    endResetModel();
}

int TableModel::currentPage() const {
    return m_currentPage;
}
int TableModel::totalPages() const {
    return m_totalPages;
}
int TableModel::pageSize() const {
    return m_pageSize;
}

int TableModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return m_data.size();
}

int TableModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return m_columnHeaders.size();
}

QVariant TableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        if (index.row() >= m_data.size() || index.column() >= m_columnHeaders.size())
            return QVariant();
        return m_data[index.row()].values[index.column()];
    }

    return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const {
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

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid() || role != Qt::EditRole)
        return false;

    if (index.row() >= m_data.size() || index.column() >= m_columnHeaders.size())
        return false;

    RowData &row = m_data[index.row()];
    row.values[index.column()] = value;

    if (row.state == Unchanged) {
        row.state = Modified;
    }

    m_hasChanges = true;
    emit dataChanged(true);
    emit QAbstractTableModel::dataChanged(index, index, { Qt::DisplayRole, Qt::EditRole });

    return true;
}

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

bool TableModel::insertRow(int row, const QModelIndex &parent) {
    if (parent.isValid())
        return false;

    beginInsertRows(parent, row, row);

    RowData newRow;
    newRow.values = QVector<QVariant>(m_columnHeaders.size());
    newRow.state = Added;

    m_data.insert(row, newRow);

    m_hasChanges = true;
    emit dataChanged(true);

    endInsertRows();
    return true;
}

bool TableModel::removeRow(int row, const QModelIndex &parent) {
    if (parent.isValid() || row < 0 || row >= m_data.size())
        return false;

    RowData &r = m_data[row];
    if (r.state == Added) {
        // 新建未提交的行，直接删除
        beginRemoveRows(parent, row, row);
        m_data.removeAt(row);
        endRemoveRows();
    } else {
        r.state = Deleted;
    }

    m_hasChanges = true;
    emit dataChanged(true);
    return true;
}

bool TableModel::submitAll() {
    if (!m_db.isOpen() || m_tableName.isEmpty()) {
        return false;
    }

    QSqlQuery query(m_db);
    m_db.transaction();

    try {
        for (const auto &row : m_data) {
            if (row.state == Unchanged)
                continue;

            if (row.state == Added) {
                // INSERT
                QStringList placeholders;
                for (int i = 0; i < m_columnHeaders.size(); ++i) {
                    placeholders << "?";
                }
                QString sql = QString("INSERT INTO %1 VALUES (%2)")
                                      .arg(m_tableName)
                                      .arg(placeholders.join(", "));

                query.prepare(sql);
                for (const auto &v : row.values) {
                    query.addBindValue(v);
                }
                if (!query.exec()) {
                    throw std::runtime_error(query.lastError().text().toStdString());
                }
            } else if (row.state == Modified) {
                // UPDATE，假设第一列是主键
                QStringList sets;
                for (int i = 1; i < m_columnHeaders.size(); ++i) {
                    sets << QString("%1=?").arg(m_columnHeaders[i]);
                }
                QString sql = QString("UPDATE %1 SET %2 WHERE %3=?")
                                      .arg(m_tableName)
                                      .arg(sets.join(", "))
                                      .arg(m_columnHeaders[0]);

                query.prepare(sql);
                for (int i = 1; i < row.values.size(); ++i) {
                    query.addBindValue(row.values[i]);
                }
                query.addBindValue(row.values[0]);
                if (!query.exec()) {
                    throw std::runtime_error(query.lastError().text().toStdString());
                }
            } else if (row.state == Deleted) {
                // DELETE，假设第一列是主键
                QString sql = QString("DELETE FROM %1 WHERE %2=?")
                                      .arg(m_tableName)
                                      .arg(m_columnHeaders[0]);

                query.prepare(sql);
                query.addBindValue(row.values[0]);
                if (!query.exec()) {
                    throw std::runtime_error(query.lastError().text().toStdString());
                }
            }
        }

        m_db.commit();
        m_hasChanges = false;
        emit dataChanged(false);

        // 刷新当前页
        loadPage(m_currentPage);
        return true;

    } catch (...) {
        m_db.rollback();
        return false;
    }
}

void TableModel::revertAll() {
    loadPage(m_currentPage);
    m_hasChanges = false;
    emit dataChanged(false);
}

void TableModel::updateTotalPages() {
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

void TableModel::loadColumnHeaders() {
    if (m_db.isOpen() && !m_tableName.isEmpty()) {
        QSqlQuery query(m_db);
        if (query.exec(QString("PRAGMA table_info(%1)").arg(m_tableName))) {
            while (query.next()) {
                m_columnHeaders.append(query.value(1).toString()); // column name
            }
        }
    }
}

bool TableModel::hasChanges() const {
    return m_hasChanges;
}
