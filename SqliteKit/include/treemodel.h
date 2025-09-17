#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QSqlDatabase>
#include <QVector>

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit TreeModel(QObject *parent = nullptr);
    
    void setDatabase(QSqlDatabase db);
    void refresh();

    // QAbstractItemModel interface
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    struct TreeNode {
        QString name;
        QString type;
        QVector<TreeNode*> children;
        TreeNode *parent = nullptr;
        
        ~TreeNode() {
            qDeleteAll(children);
        }
    };

private:
    void buildTree();
    void clearTree();
    
    TreeNode *m_root = nullptr;
    QSqlDatabase m_db;
};

#endif // TREEMODEL_H