#include "../include/treemodel.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDebug>
#include <QIcon>

TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_root(new TreeNode{})
{
    m_root->name = "Database";
    m_root->type = "database";
}

void TreeModel::setDatabase(QSqlDatabase db)
{
    beginResetModel();
    
    m_db = db;
    clearTree();
    buildTree();
    
    endResetModel();
}

void TreeModel::refresh()
{
    beginResetModel();
    
    clearTree();
    buildTree();
    
    endResetModel();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();
    
    TreeNode *parentNode;
    if (!parent.isValid())
        parentNode = m_root;
    else
        parentNode = static_cast<TreeNode*>(parent.internalPointer());
    
    if (row < 0 || row >= parentNode->children.size())
        return QModelIndex();
    
    TreeNode *childNode = parentNode->children[row];
    return createIndex(row, column, childNode);
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();
    
    TreeNode *childNode = static_cast<TreeNode*>(index.internalPointer());
    TreeNode *parentNode = childNode->parent;
    
    if (parentNode == m_root)
        return QModelIndex();
    
    return createIndex(parentNode->parent->children.indexOf(parentNode), 0, parentNode);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeNode *parentNode;
    if (!parent.isValid())
        parentNode = m_root;
    else
        parentNode = static_cast<TreeNode*>(parent.internalPointer());
    
    return parentNode->children.size();
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    
    TreeNode *node = static_cast<TreeNode*>(index.internalPointer());
    
    if (role == Qt::DisplayRole) {
        return node->name;
    } else if (role == Qt::DecorationRole) {
        // Return icons based on node type
        if (node->type == "database") {
            return QIcon(":/SqliteKit/icon/ic_database.svg");
        } else if (node->type == "table") {
            return QIcon(":/SqliteKit/icon/ic_table.svg");
        }
    }
    
    return QVariant();
}

void TreeModel::buildTree()
{
    if (!m_db.isOpen())
        return;
    
    // Get all tables
    QSqlQuery query(m_db);
    if (query.exec("SELECT name FROM sqlite_master WHERE type='table'")) {
        while (query.next()) {
            QString tableName = query.value(0).toString();
            TreeNode *tableNode = new TreeNode{tableName, "table", {}, m_root};
            m_root->children.append(tableNode);
        }
    }
}

void TreeModel::clearTree()
{
    qDeleteAll(m_root->children);
    m_root->children.clear();
}