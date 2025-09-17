# 📘 SQLite 管理工具开发文档

## 1. 基础设计

* **分页策略**

  * 每页固定 `1000` 行。
  * `TableModel` 内部维护 `currentPage`、`totalPage`。
  * SQL 查询加 `LIMIT` 与 `OFFSET` 控制分页。
  * 例如：

    ```sql
    SELECT * FROM tableName LIMIT 1000 OFFSET (pageNo - 1) * 1000;
    ```

* **数据一致性**

  * 增删改通过 `TableModel` 的 `insertRow/removeRow/setData` 完成。
  * 编辑后数据调用 `submitAll()` 提交，或 `revertAll()` 撤销。

---

## 2. 交互逻辑

### 2.1 树视图 → 表标签页

* **操作**：用户在 `treeView_database` 双击某个表节点。
* **逻辑**：

  1. 检查该表是否已有标签页。

     * 有 → 切换到该页。
     * 无 → 新建一个标签页，内部放置 `TableView`。
  2. 为 `TableView` 创建对应的 `TableModel`，并调用 `model->setTable(tableName)`。
  3. 加载第一页数据。

---

### 2.2 分页操作（每页 1000 行）

* **控件对应**

  * `ui->toolButton_angles_left` → 跳转第一页
  * `ui->toolButton_angle_left` → 跳转上一页
  * `ui->spinBox_currentPage` → 输入页码跳转
  * `ui->label_totalPage` → 显示总页数
  * `ui->toolButton_angle_right` → 跳转下一页
  * `ui->toolButton_angles_right` → 跳转最后一页

* **操作逻辑**

  1. 获取当前活动的 `TableModel`。
  2. 修改 `currentPage` 值。
  3. 调用 `model->loadPage(currentPage)` 重新查询数据库。
  4. 更新 `ui->spinBox_currentPage` 和 `ui->label_totalPage`。

* **伪代码：**

  ```cpp
  void SqliteKit::onNextPage() {
      auto model = currentTableModel();
      if (!model) return;
      if (model->currentPage() < model->totalPages()) {
          model->loadPage(model->currentPage() + 1);
          updatePageControls(model);
      }
  }
  ```

---

### 2.3 新增行

* **控件**：`ui->toolButton_add`
* **逻辑**：

  1. 获取当前表对应的 `TableModel`。
  2. 调用 `model->insertRow(model->rowCount())`。
  3. 表格末尾出现空行，用户可输入数据。
  4. 退出编辑后，调用 `model->submitAll()` 提交数据库。

---

### 2.4 删除行

* **控件**：`ui->toolButton_subtract`
* **逻辑**：

  1. 获取当前选中的行号。
  2. 调用 `model->removeRow(row)`。
  3. 调用 `model->submitAll()` 提交更改。

---

### 2.5 刷新表

* **控件**：`ui->toolButton_refresh`
* **逻辑**：

  1. 获取当前 `TableModel`。
  2. 调用 `model->loadPage(currentPage)`，重新查询数据库。

---

## 3. 模型接口设计

### 3.1 `TableModel`

```cpp
class TableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit TableModel(QObject *parent = nullptr);

    void setTable(const QString &tableName, QSqlDatabase db);
    void loadPage(int pageNo);

    int currentPage() const;
    int totalPages() const;

    bool insertRow(int row, const QModelIndex &parent = QModelIndex()) override;
    bool removeRow(int row, const QModelIndex &parent = QModelIndex()) override;
    bool submitAll();
    void revertAll();

private:
    QString m_tableName;
    QSqlDatabase m_db;
    QVector<QVector<QVariant>> m_data; // 当前页缓存
    int m_currentPage = 1;
    int m_totalPages = 1;
    int m_pageSize = 1000;
};
```

---

## 4. 导入/导出接口（预留）

### 4.1 导入接口

```cpp
class TableModel {
public:
    bool importFromCsv(const QString &filePath);
    bool importFromExcel(const QString &filePath); // 可用 QXlsx 扩展库
};
```

* **逻辑**：

  1. 读取 CSV/Excel 文件。
  2. 按行插入到数据库表。
  3. 调用 `submitAll()`。

### 4.2 导出接口

```cpp
class TableModel {
public:
    bool exportToCsv(const QString &filePath);
    bool exportToExcel(const QString &filePath);
};
```

* **逻辑**：

  1. 查询全表数据。
  2. 写入文件。
  3. 返回操作结果。

---

## 5. 页面更新逻辑

### 5.1 获取总页数

```cpp
void TableModel::updateTotalPages() {
    QSqlQuery query(m_db);
    query.exec(QString("SELECT COUNT(*) FROM %1").arg(m_tableName));
    if (query.next()) {
        int totalRows = query.value(0).toInt();
        m_totalPages = qMax(1, (totalRows + m_pageSize - 1) / m_pageSize);
    }
}
```

### 5.2 加载指定页

```cpp
void TableModel::loadPage(int pageNo) {
    if (pageNo < 1) pageNo = 1;
    if (pageNo > m_totalPages) pageNo = m_totalPages;
    m_currentPage = pageNo;

    QSqlQuery query(m_db);
    query.exec(QString("SELECT * FROM %1 LIMIT %2 OFFSET %3")
               .arg(m_tableName)
               .arg(m_pageSize)
               .arg((m_currentPage - 1) * m_pageSize));

    m_data.clear();
    while (query.next()) {
        QVector<QVariant> row;
        for (int i = 0; i < query.record().count(); ++i)
            row.append(query.value(i));
        m_data.append(row);
    }

    beginResetModel();
    endResetModel();
}
```

---

## 6. UI 与逻辑绑定

### 6.1 页面切换

* 在 `SqliteKit.cpp` 中绑定工具栏按钮：

```cpp
connect(ui->toolButton_angles_left, &QToolButton::clicked, this, &SqliteKit::onFirstPage);
connect(ui->toolButton_angle_left, &QToolButton::clicked, this, &SqliteKit::onPrevPage);
connect(ui->toolButton_angle_right, &QToolButton::clicked, this, &SqliteKit::onNextPage);
connect(ui->toolButton_angles_right, &QToolButton::clicked, this, &SqliteKit::onLastPage);
connect(ui->toolButton_refresh, &QToolButton::clicked, this, &SqliteKit::onRefresh);
connect(ui->toolButton_add, &QToolButton::clicked, this, &SqliteKit::onAddRow);
connect(ui->toolButton_subtract, &QToolButton::clicked, this, &SqliteKit::onDeleteRow);
```

### 6.2 页面同步

```cpp
void SqliteKit::updatePageControls(TableModel *model) {
    ui->spinBox_currentPage->setValue(model->currentPage());
    ui->label_totalPage->setText(QString("/ %1").arg(model->totalPages()));
}
```

---

## 7. 扩展计划

* 支持 **SQL 编辑器**，直接运行自定义 SQL。
* 增加 **事务支持**：批量操作回滚/提交。
* 增加 **字段搜索与条件过滤**。
* 增加 **多数据库连接管理**。
