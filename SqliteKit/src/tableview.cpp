#include "../include/tableview.h"
#include <QMenu>
#include <QContextMenuEvent>
#include <QClipboard>
#include <QApplication>
#include <QHeaderView>

TableView::TableView(QWidget *parent)
    : QTableView(parent)
{
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setAlternatingRowColors(true);
    setSortingEnabled(true);
}

void TableView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    
    QAction *copyAction = menu.addAction("Copy");
    QAction *pasteAction = menu.addAction("Paste");
    QAction *deleteAction = menu.addAction("Delete");
    
    connect(copyAction, &QAction::triggered, this, &TableView::onCopyAction);
    connect(pasteAction, &QAction::triggered, this, &TableView::onPasteAction);
    connect(deleteAction, &QAction::triggered, this, &TableView::onDeleteAction);
    
    menu.exec(event->globalPos());
}

void TableView::onCopyAction()
{
    // TODO: Implement copy functionality
}

void TableView::onPasteAction()
{
    // TODO: Implement paste functionality
}

void TableView::onDeleteAction()
{
    // TODO: Implement delete functionality
}