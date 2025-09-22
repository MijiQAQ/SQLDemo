#include "SqliteKit.h"
#include "ui_SqliteKit.h"
#include <QTabWidget>
#include <QTableView>
#include <QMessageBox>
#include <QTreeView>
#include <QSqlQuery>
#include <QDebug>

SqliteKit::SqliteKit(QWidget *parent, const QSqlDatabase &db)
    : QWidget(parent), ui(new Ui::SqliteKit), m_db(db), m_treeModel(new TreeModel(this))
{
    Q_INIT_RESOURCE(SqliteKit);
    ui->setupUi(this);
    

}

SqliteKit::~SqliteKit()
{

    delete ui;
}

