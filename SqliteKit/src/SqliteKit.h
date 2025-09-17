#ifndef SQLITEKIT_H
#define SQLITEKIT_H

#include <QWidget>
#include <QSqlDatabase>
#include <QItemSelection>

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

 
private:
    Ui::SqliteKit *ui;
    QSqlDatabase m_db;

};

#endif // SQLITEKIT_H
