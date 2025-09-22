#include "SQLDemo.h"
#include "ui_SQLDemo.h"

#include <QFile>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlerror>
#include <QRandomGenerator>
#include "SqliteKit/src/SqliteKit.h"

SQLDemo::SQLDemo(QWidget *parent) : QMainWindow(parent), ui(new Ui::SQLDemo) {
    ui->setupUi(this);

    
    if (QFile::exists("test.db")) {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "dbConnection");
        db.setDatabaseName("test.db");
        SqliteKit *kit = new SqliteKit(nullptr, db);
        kit->show(); // 非模态显示，不会阻塞
    }
    else {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "dbConnection");
        db.setDatabaseName("test.db");
        // db.setPassword("password"); // 密码会在 open() 时传入 sqlite3_key
    
        if (!db.open()) {
            qWarning() << "数据库打开失败:" << db.lastError().text();
            return;
        }
    
        QSqlQuery query(db);
    
        // 建表 1: 用户
        if (!query.exec("CREATE TABLE IF NOT EXISTS users ("
                        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                        "name TEXT, "
                        "age INTEGER)")) {
            qWarning() << "建表 users 失败:" << query.lastError().text();
        }
    
        // 建表 2: 产品
        if (!query.exec("CREATE TABLE IF NOT EXISTS products ("
                        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                        "title TEXT, "
                        "price REAL)")) {
            qWarning() << "建表 products 失败:" << query.lastError().text();
        }
    
        // 建表 3: 订单
        if (!query.exec("CREATE TABLE IF NOT EXISTS orders ("
                        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                        "user_id INTEGER, "
                        "product_id INTEGER, "
                        "quantity INTEGER, "
                        "FOREIGN KEY(user_id) REFERENCES users(id), "
                        "FOREIGN KEY(product_id) REFERENCES products(id))")) {
            qWarning() << "建表 orders 失败:" << query.lastError().text();
        }
    
        // 插入随机用户
        query.prepare("INSERT INTO users (name, age) VALUES (?, ?)");
        for (int i = 0; i < 10; ++i) {
            query.addBindValue(QString("User_%1").arg(i + 1));
            query.addBindValue(QRandomGenerator::global()->bounded(18, 60));
            if (!query.exec()) {
                qWarning() << "插入 users 失败:" << query.lastError().text();
            }
        }
    
        // 插入随机产品
        query.prepare("INSERT INTO products (title, price) VALUES (?, ?)");
        for (int i = 0; i < 5; ++i) {
            query.addBindValue(QString("Product_%1").arg(i + 1));
            query.addBindValue(QRandomGenerator::global()->bounded(10, 100) * 1.0);
            if (!query.exec()) {
                qWarning() << "插入 products 失败:" << query.lastError().text();
            }
        }
    
        // 插入随机订单
        query.prepare("INSERT INTO orders (user_id, product_id, quantity) VALUES (?, ?, ?)");
        for (int i = 0; i < 2345; ++i) {
            int userId = QRandomGenerator::global()->bounded(1, 11); // 1~10
            int productId = QRandomGenerator::global()->bounded(1, 6); // 1~5
            int qty = QRandomGenerator::global()->bounded(1, 5);
            query.addBindValue(userId);
            query.addBindValue(productId);
            query.addBindValue(qty);
            if (!query.exec()) {
                qWarning() << "插入 orders 失败:" << query.lastError().text();
            }
        }
    
        qDebug() << "数据库初始化完成！";
    
        SqliteKit *kit = new SqliteKit(nullptr, db);
        kit->show(); // 非模态显示，不会阻塞
    }
    //===============================================

}

SQLDemo::~SQLDemo() {
    delete ui;
}
