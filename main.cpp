#include "SQLDemo.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SQLDemo w;
    w.show();
    return a.exec();
}
