#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include <QTableView>

class TableView : public QTableView
{
    Q_OBJECT

public:
    explicit TableView(QWidget *parent = nullptr);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
    void onCopyAction();
    void onPasteAction();
    void onDeleteAction();
};

#endif // TABLEVIEW_H