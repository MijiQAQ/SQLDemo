# 📘 SQLiteKit 需求文档

## 1. 项目简介

SQLiteKit 是一个基于 **Qt + SQLite** 的轻量级数据库管理库与工具，目标是提供 **比 SQLiteBrowser 更轻便** 的数据库管理能力，方便嵌入到桌面应用程序或单独运行。

---

## 2. 系统架构

采用 **MVVM 架构**：

* **Model 层**

  * 封装 SQLite 数据库访问
  * 提供 Schema（表、视图、索引、触发器）访问接口
  * 封装 `QSqlDatabase / QSqlQuery / QSqlTableModel`

* **ViewModel 层**

  * 负责业务逻辑和状态管理
  * 提供数据库连接、数据查询、过滤搜索、事务控制的接口
  * 向 View 层暴露只读的状态属性，提供信号/槽驱动 UI

* **View 层**

  * 提供 UI 展示，不包含数据库逻辑
  * 使用 `QTreeView` / `QTableView` / `QPlainTextEdit` 等 Qt 控件

---

## 3. 功能需求

### 3.1 数据库管理

* 打开/关闭 SQLite 数据库文件（`.db`、`.sqlite`）
* 显示当前数据库连接状态
* 支持多数据库切换

### 3.2 Schema 浏览

* 以树结构展示数据库对象：

  * Tables
  * Views
  * Indexes
  * Triggers
* 双击表名可直接查看数据

### 3.3 数据浏览

* 在 `QTableView` 中展示表数据
* 支持分页加载（避免一次性加载大表）
* 支持行内编辑（`OnFieldChange`）
* 支持新增/删除行
* 支持数据排序

### 3.4 SQL 控制台

* 输入任意 SQL 语句并执行
* 执行结果在表格中显示（SELECT）
* 非 SELECT 语句返回影响行数或错误信息
* 显示执行耗时

### 3.5 数据过滤与搜索

* 支持按列过滤（精确匹配 / 模糊匹配 / 正则匹配）
* 支持多条件组合（AND / OR）
* 过滤条件可通过 UI 输入，或由 SQL 自动生成
* 支持在分页模式下进行 SQL 下推 (`WHERE` + `LIMIT OFFSET`)

### 3.6 事务管理

* 开始事务
* 提交事务
* 回滚事务
* UI 提供事务状态指示

### 3.7 数据导入导出

* 导出：CSV / JSON / SQL 脚本
* 导入：CSV → 表

### 3.8 元数据操作

* 新建表 / 删除表
* 新建索引 / 删除索引
* 新建视图 / 删除视图
* 新建触发器 / 删除触发器

---

## 4. 非功能性需求

### 4.1 可扩展性

* 提供 C++/Qt API，可嵌入到其它项目中（不仅仅是 GUI 工具）
* View 层与 ViewModel 解耦，支持替换 UI 技术（如 QML）

### 4.2 易用性

* 提供简洁 UI，核心界面控制在 **三部分**：

  * 左侧对象树
  * 中间数据表格
  * 下方 SQL 控制台

### 4.3 性能

* 针对大表数据：分页加载、懒加载
* 过滤条件尽量下推到 SQLite 层

### 4.4 跨平台

* Windows / macOS / Linux（Qt 6.x）

---

## 5. 项目结构建议

```
SQLiteKit/
├── src/
│   ├── model/
│   │   ├── DatabaseModel.h/.cpp
│   │   ├── SchemaModel.h/.cpp
│   │   ├── TableModel.h/.cpp
│   │   ├── QueryModel.h/.cpp
│   │
│   ├── viewmodel/
│   │   ├── DatabaseViewModel.h/.cpp
│   │   ├── SchemaViewModel.h/.cpp
│   │   ├── TableViewModel.h/.cpp
│   │   ├── QueryViewModel.h/.cpp
│   │
│   ├── view/
│   │   ├── MainWindow.ui/.cpp
│   │   ├── DatabaseTreeView.h/.cpp
│   │   ├── DataTableView.h/.cpp
│   │   ├── SqlConsoleWidget.h/.cpp
│   │
│   └── utils/
│       ├── CsvExporter.h/.cpp
│       ├── JsonExporter.h/.cpp
│       ├── SqlFormatter.h/.cpp
│
├── tests/
│   ├── ModelTests.cpp
│   ├── ViewModelTests.cpp
│
├── examples/
│   └── MiniManagerDemo.cpp
│
├── CMakeLists.txt
└── README.md
```

---

## 6. 扩展功能 (未来版本)

* SQL 语法高亮（基于 QSyntaxHighlighter）
* 自动补全（表名、字段名）
* 历史 SQL 记录 + 收藏
* 连接远程 SQLite（通过 SSH/HTTP 中转）
* 插件机制（第三方扩展）

---

## 7. 交付物

1. **SQLiteKit 库**（可嵌入）
2. **SQLiteKit Manager**（GUI 工具，类似轻量版 SQLiteBrowser）
3. **开发文档 & API 文档**
4. **单元测试 & 示例项目**

---

👉 这份需求文档已经覆盖了 **数据库连接 → Schema → 数据浏览 → 搜索 → 事务 → 导入导出** 的完整链路，同时保留扩展能力。
