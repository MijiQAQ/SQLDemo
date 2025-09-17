#include "../include/treeview.h"
#include <QMenu>
#include <QContextMenuEvent>

TreeView::TreeView(QWidget *parent)
    : QTreeView(parent)
{
    setHeaderHidden(true);
    setExpandsOnDoubleClick(true);
}

void TreeView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    
    QAction *refreshAction = menu.addAction("Refresh");
    QAction *expandAllAction = menu.addAction("Expand All");
    QAction *collapseAllAction = menu.addAction("Collapse All");
    
    connect(refreshAction, &QAction::triggered, this, &TreeView::onRefreshAction);
    connect(expandAllAction, &QAction::triggered, this, &TreeView::onExpandAllAction);
    connect(collapseAllAction, &QAction::triggered, this, &TreeView::onCollapseAllAction);
    
    menu.exec(event->globalPos());
}

void TreeView::onRefreshAction()
{
    // TODO: Implement refresh functionality
}

void TreeView::onExpandAllAction()
{
    expandAll();
}

void TreeView::onCollapseAllAction()
{
    collapseAll();
}