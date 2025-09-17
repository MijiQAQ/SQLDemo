#ifndef TREEVIEW_H
#define TREEVIEW_H

#include <QTreeView>

class TreeView : public QTreeView
{
    Q_OBJECT

public:
    explicit TreeView(QWidget *parent = nullptr);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
    void onRefreshAction();
    void onExpandAllAction();
    void onCollapseAllAction();
};

#endif // TREEVIEW_H