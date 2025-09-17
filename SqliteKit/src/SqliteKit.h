#ifndef SQLITEKIT_H
#define SQLITEKIT_H

#include <QWidget>
#include <QSqlDatabase>
#include <QItemSelection>
#include <QTabWidget>
#include <QTableView>
#include "../include/tablemodel.h"
#include "../include/treemodel.h"
#include "../include/tableview.h"
#include "../include/treeview.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class SqliteKit;
}
QT_END_NAMESPACE

class SqliteKit : public QWidget
{
    Q_OBJECT

public:
    explicit SqliteKit(QWidget *parent = nullptr, const QSqlDatabase &db = QSqlDatabase());
    ~SqliteKit();

private slots:
    void onFirstPage();
    void onPrevPage();
    void onNextPage();
    void onLastPage();
    void onRefresh();
    void onAddRow();
    void onDeleteRow();
    void onPageChanged(int page);
    void onTreeViewDoubleClicked(const QModelIndex &index);
    void onCheckChanges();
    void onCancelChanges();
    void onModelDataChanged(bool hasChanges);

private:
    void setupConnections();
    void updatePageControls();
    TableModel* currentTableModel() const;
    QTableView* currentTableView() const;
    void createTableTab(const QString &tableName);
    void switchToTableTab(const QString &tableName);
    bool tableTabExists(const QString &tableName) const;
    void refreshTreeView();

    Ui::SqliteKit *ui;
    QSqlDatabase m_db;
    QHash<QString, TableModel*> m_tableModels;
    TreeModel *m_treeModel;
    TreeView *m_treeView;
};

#endif // SQLITEKIT_H
