#ifndef SQLDEMO_H
#define SQLDEMO_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class SQLDemo;
}
QT_END_NAMESPACE

class SQLDemo : public QMainWindow
{
    Q_OBJECT

public:
    SQLDemo(QWidget *parent = nullptr);
    ~SQLDemo();

private:
    Ui::SQLDemo *ui;
};
#endif // SQLDEMO_H
