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
    
    // Setup tree model
    m_treeModel->setDatabase(m_db);
    
    // Create tree view and add to widget_database
    m_treeView = new TreeView(ui->widget_database);
    m_treeView->setModel(m_treeModel);
    m_treeView->setObjectName("treeView_database");
    
    QVBoxLayout *layout = new QVBoxLayout(ui->widget_database);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_treeView);
    ui->widget_database->setLayout(layout);
    
    setupConnections();
}

SqliteKit::~SqliteKit()
{
    // Clean up table models
    qDeleteAll(m_tableModels);
    m_tableModels.clear();
    
    delete m_treeModel;
    delete ui;
}

void SqliteKit::setupConnections()
{
    // Connect pagination buttons
    connect(ui->toolButton_angles_left, &QToolButton::clicked, this, &SqliteKit::onFirstPage);
    connect(ui->toolButton_angle_left, &QToolButton::clicked, this, &SqliteKit::onPrevPage);
    connect(ui->toolButton_angle_right, &QToolButton::clicked, this, &SqliteKit::onNextPage);
    connect(ui->toolButton_angles_right, &QToolButton::clicked, this, &SqliteKit::onLastPage);
    
    // Connect action buttons
    connect(ui->toolButton_refresh, &QToolButton::clicked, this, &SqliteKit::onRefresh);
    connect(ui->toolButton_add, &QToolButton::clicked, this, &SqliteKit::onAddRow);
    connect(ui->toolButton_subtract, &QToolButton::clicked, this, &SqliteKit::onDeleteRow);
    connect(ui->toolButton_check, &QToolButton::clicked, this, &SqliteKit::onCheckChanges);
    connect(ui->toolButton_cancel, &QToolButton::clicked, this, &SqliteKit::onCancelChanges);
    
    // Connect page spinbox
    connect(ui->spinBox_currentPage, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &SqliteKit::onPageChanged);
    
    // Connect tree view double click
    connect(m_treeView, &QTreeView::doubleClicked,
            this, &SqliteKit::onTreeViewDoubleClicked);
    
    // Initially disable check and cancel buttons
    ui->toolButton_check->setEnabled(false);
    ui->toolButton_cancel->setEnabled(false);
}

void SqliteKit::onFirstPage()
{
    TableModel *model = currentTableModel();
    if (model && model->currentPage() > 1) {
        model->loadPage(1);
        updatePageControls();
    }
}

void SqliteKit::onPrevPage()
{
    TableModel *model = currentTableModel();
    if (model && model->currentPage() > 1) {
        model->loadPage(model->currentPage() - 1);
        updatePageControls();
    }
}

void SqliteKit::onNextPage()
{
    TableModel *model = currentTableModel();
    if (model && model->currentPage() < model->totalPages()) {
        model->loadPage(model->currentPage() + 1);
        updatePageControls();
    }
}

void SqliteKit::onLastPage()
{
    TableModel *model = currentTableModel();
    if (model) {
        model->loadPage(model->totalPages());
        updatePageControls();
    }
}

void SqliteKit::onRefresh()
{
    TableModel *model = currentTableModel();
    if (model) {
        model->loadPage(model->currentPage());
        updatePageControls();
    }
}

void SqliteKit::onAddRow()
{
    TableModel *model = currentTableModel();
    QTableView *tableView = currentTableView();
    
    if (model && tableView) {
        int row = model->rowCount();
        if (model->insertRow(row)) {
            tableView->selectRow(row);
            tableView->edit(model->index(row, 0));
        }
    }
}

void SqliteKit::onDeleteRow()
{
    TableModel *model = currentTableModel();
    QTableView *tableView = currentTableView();
    
    if (model && tableView) {
        QModelIndexList selected = tableView->selectionModel()->selectedRows();
        if (!selected.isEmpty()) {
            int row = selected.first().row();
            if (model->removeRow(row)) {
                // Changes are tracked, user can submit or cancel later
                // Pagination will be updated when changes are submitted or reverted
            }
        }
    }
}

void SqliteKit::onPageChanged(int page)
{
    TableModel *model = currentTableModel();
    if (model) {
        model->loadPage(page);
        updatePageControls();
    }
}

void SqliteKit::onTreeViewDoubleClicked(const QModelIndex &index)
{
    TreeModel *model = qobject_cast<TreeModel*>(m_treeView->model());
    if (!model) {
        return;
    }
    
    // Get the node from index
    TreeModel::TreeNode *node = static_cast<TreeModel::TreeNode*>(index.internalPointer());
    if (!node) {
        return;
    }
    
    // Check if this is a table node
    if (node->type == "table") {
        createTableTab(node->name);
    }
}

void SqliteKit::updatePageControls()
{
    TableModel *model = currentTableModel();
    if (model) {
        ui->spinBox_currentPage->blockSignals(true);
        ui->spinBox_currentPage->setValue(model->currentPage());
        ui->spinBox_currentPage->blockSignals(false);
        
        ui->label_totalPage->setText(QString("/ %1").arg(model->totalPages()));
    } else {
        ui->spinBox_currentPage->setValue(1);
        ui->label_totalPage->setText("/ 1");
    }
}

TableModel* SqliteKit::currentTableModel() const
{
    QWidget *currentTab = ui->tabWidget_database->currentWidget();
    if (currentTab) {
        QTableView *tableView = currentTab->findChild<QTableView*>();
        if (tableView) {
            return qobject_cast<TableModel*>(tableView->model());
        }
    }
    return nullptr;
}

QTableView* SqliteKit::currentTableView() const
{
    QWidget *currentTab = ui->tabWidget_database->currentWidget();
    if (currentTab) {
        return currentTab->findChild<TableView*>();
    }
    return nullptr;
}

void SqliteKit::createTableTab(const QString &tableName)
{
    if (tableTabExists(tableName)) {
        switchToTableTab(tableName);
        return;
    }
    
    // Create new tab with table view
    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);
    
    TableView *tableView = new TableView(tab);
    layout->addWidget(tableView);
    
    // Create and set model
    TableModel *model = new TableModel(this);
    model->setTable(tableName, m_db);
    tableView->setModel(model);
    
    // Connect data changed signal
    connect(model, &TableModel::dataChanged, this, &SqliteKit::onModelDataChanged);
    
    // Store model reference
    m_tableModels.insert(tableName, model);
    
    // Add tab
    int tabIndex = ui->tabWidget_database->addTab(tab, tableName);
    ui->tabWidget_database->setCurrentIndex(tabIndex);
    
    // Update page controls
    updatePageControls();
}

void SqliteKit::switchToTableTab(const QString &tableName)
{
    for (int i = 0; i < ui->tabWidget_database->count(); ++i) {
        if (ui->tabWidget_database->tabText(i) == tableName) {
            ui->tabWidget_database->setCurrentIndex(i);
            updatePageControls();
            return;
        }
    }
}

bool SqliteKit::tableTabExists(const QString &tableName) const
{
    for (int i = 0; i < ui->tabWidget_database->count(); ++i) {
        if (ui->tabWidget_database->tabText(i) == tableName) {
            return true;
        }
    }
    return false;
}

void SqliteKit::refreshTreeView()
{
    if (m_treeModel) {
        m_treeModel->setDatabase(m_db);
    }
}

void SqliteKit::onCheckChanges()
{
    TableModel *model = currentTableModel();
    if (model && model->submitAll()) {
        // Changes saved successfully
        ui->toolButton_check->setEnabled(false);
        ui->toolButton_cancel->setEnabled(false);
        refreshTreeView();
    }
}

void SqliteKit::onCancelChanges()
{
    TableModel *model = currentTableModel();
    if (model) {
        model->revertAll();
        ui->toolButton_check->setEnabled(false);
        ui->toolButton_cancel->setEnabled(false);
        refreshTreeView();
    }
}


void SqliteKit::onModelDataChanged(bool hasChanges)
{
    ui->toolButton_check->setEnabled(hasChanges);
    ui->toolButton_cancel->setEnabled(hasChanges);
}
