#include "SqliteKit.h"
#include "ui_SqliteKit.h"


SqliteKit::SqliteKit(QWidget *parent, const QSqlDatabase &db)
    : QWidget(parent), ui(new Ui::SqliteKit), m_db(db) {
    Q_INIT_RESOURCE(SqliteKit); // 如果有 qrc
    ui->setupUi(this);

}

SqliteKit::~SqliteKit() {
    delete ui;
}
