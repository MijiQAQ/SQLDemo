修改了ui
    删除了ui->treeView_database 改为 ui->widget_database 部件，使用ui->widget_database 装载treeview
    增加了ui->toolButton_check, 数据无更改时，按钮不使能，有更改时，按钮使能，点击后，将更改的数据保存到数据库中，并刷新treeview
    增加了ui->toolButton_cancel, 数据无更改时，按钮不使能，有更改时，按钮使能，点击后，将更改的数据取消，并刷新treeview

修复点击 ui->toolButton_subtract 时，没有删除对应行数据的bug

ui美化
    为treeview数据库增加图标 ":/SqliteKit/icon/ic_database.svg"
    为treeview数据表增加图标 ":/SqliteKit/icon/ic_table.svg"
