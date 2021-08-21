#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    count = 0;
    ui->treeWidget->setColumnCount(2);
    QStringList headers;
    headers << tr("ID") << tr("Имена");
    ui->treeWidget->setHeaderLabels(headers);
    currentItem = NULL;
    currentColumn = 0;
    ConnectDB();
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::treeCount(QTreeWidget *tree, QTreeWidgetItem *parent = 0)
{
    tree->expandAll();
    int count = 0;
    if (parent == 0)
    {
        int topCount = tree->topLevelItemCount();
        for (int i = 0; i < topCount; i++)
        {
            QTreeWidgetItem *item = tree->topLevelItem(i);
            if (item->isExpanded())
            {
                count += treeCount(tree, item);
            }
        }
        count += topCount;
    }
    else
    {
        int childCount = parent->childCount();
        for (int i = 0; i < childCount; i++)
        {
            QTreeWidgetItem *item = parent->child(i);
            if (item->isExpanded())
            {
                count += treeCount(tree, item);
            }
        }
        count += childCount;
    }
    return count;
}

void MainWindow::getChildID(QTreeWidgetItem *curItem)
{
    int childCount = curItem->childCount();
    if(childCount <= 0)
        return;
    for(int i = 0; i < childCount; i++)
    {
        childID->push_back(curItem->child(i)->text(0));
        getChildID(curItem->child(i));
    }
}

void MainWindow::DeleteItem (QTreeWidgetItem *currentItem)
{
    QTreeWidgetItem *parent = currentItem->parent();
    int index;
    if (parent)
    {
        index = parent->indexOfChild(ui->treeWidget->currentItem());
        delete parent->takeChild(index);
    }
    else
    {
        index = ui->treeWidget->indexOfTopLevelItem(ui->treeWidget->currentItem());
        delete ui->treeWidget->takeTopLevelItem(index);
    }
}

void MainWindow::InsertItem (QStringList text, QTreeWidgetItem *parent = 0)
{
    if(parent != 0)
    {
        if (parent->isExpanded()==false) parent->setExpanded(true);
        QTreeWidgetItem *newItem = new QTreeWidgetItem(parent, ui->treeWidget->currentItem());
        newItem->setText(0, (text)[0]);
        newItem->setText(1, (text)[1]);
        newItem->setExpanded(true);
    }
    else
    {
        QTreeWidgetItem *newItem = new QTreeWidgetItem(ui->treeWidget, ui->treeWidget->currentItem());
        //указываем 2-м параметром текущий элемент как предшествующий
        newItem->setText(0, (text)[0]);
        newItem->setText(1, (text)[1]);
        newItem->setExpanded(true);
    }

}

void MainWindow::showAll(void) {
    int cnt = treeCount (ui->treeWidget);
    QString str(tr("Всего: ")+QString("%1").arg(cnt));
    setWindowTitle(str);
}

QString MainWindow::getName()
{
    bool bOk;
    QString str = QInputDialog::getText( 0,
                                         "Input",
                                         "Name:",
                                         QLineEdit::Normal,
                                         "Tarja",
                                         &bOk
                                        );
    if (!bOk) {
        QMessageBox::warning(this, tr("Предупреждение!"),
                                       tr("Имя будет пустым!!!"),
                                       QMessageBox::Ok
                                       | QMessageBox::Cancel);
    }
    return str;
}

void MainWindow::ConnectDB()
{
    QString host = "DESKTOP-FCQVH20";
    QString database = "test_database";
    db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName(QString("DRIVER={SQL Server};"
                "SERVER=%1;DATABASE=%2;Persist Security inpText=true;")
              .arg(host, database));
    if (!db.open())
            QMessageBox::critical(this, tr("Ошибка"),
                    tr("Ошибка подключения к базе данных: %2")
                .arg(db.lastError().text()));
    SelectDB();
}

void MainWindow::SelectDB()
{

    model = new QSqlTableModel();
    model->setTable("TableNames");
    model->select();

    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Name");
    model->setHeaderData(2, Qt::Horizontal, "Parent_ID");

    ui->tableView->setModel(model);
}

void MainWindow::InsertDB(QString id, QString name, QString parId)
{
    QSqlQuery *query = new QSqlQuery();
    query->prepare("SELECT * FROM TableNames WHERE ID=:id");
    query->bindValue(":id", id);

    if(query->exec() && query->next())
        if(query->value(0).toString() == id) return;

    query->prepare("INSERT INTO TableNames(ID, Name, Parent_ID) VALUES(:id, :name, :parId)");
    query->bindValue(":id", id);
    query->bindValue(":name", name);
    query->bindValue(":parId", parId);
    query->exec();
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    currentItem = item;
    currentColumn = column;
}

QString MainWindow::getIdItem(QTreeWidgetItem *curItem)
{
    QString word = curItem->text(0);
    QString par_id = "";
    for(int i = 0; i < word.size(); i++)
    {
        if(word[i] == QString("%1").arg(" "))
            break;
        par_id += word[i];
    }
    return par_id;
}

void MainWindow::on_pushButton_clicked()    // Добавить
{
    QString name = getName();
    QStringList inpText;
    inpText << QString("%1").arg(++count) << name;
    if (currentItem)
    {
        InsertItem(inpText, currentItem);
        InsertDB(QString("%1").arg(count), name, currentItem->text(0));
    }
    else    // верхний узел дерева
    {
        InsertItem(inpText);
        InsertDB(QString("%1").arg(count), name, "0");
    }
    currentItem = NULL;
    SelectDB();
    showAll();
}


void MainWindow::on_pushButton_2_clicked()  //Удалить
{
    if (currentItem)
    {
        QSqlQuery* query = new QSqlQuery();
        QString id = currentItem->text(0);
        getChildID(currentItem);
        query->prepare("DELETE FROM TableNames WHERE ID=:id");
        for(auto i = childID->begin(); i != childID->end(); i++)
        {
            QString cID = *i;
            query->bindValue(":id", cID);
            if(!query->exec())
                QMessageBox::warning(this, tr("Предупреждение!"),
                                               tr("Проверьте правильность запроса!!!"),
                                               QMessageBox::Ok
                                               | QMessageBox::Cancel);
        }

        query->bindValue(":id", id);
        if(!query->exec())
            QMessageBox::warning(this, tr("Предупреждение!"),
                                           tr("Проверьте правильность запроса!!!"),
                                           QMessageBox::Ok
                                           | QMessageBox::Cancel);
        childID->clear();
        DeleteItem (currentItem);
        currentItem = NULL;
    }
    SelectDB();
    showAll();
}


void MainWindow::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{

}


void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
}


void MainWindow::on_pushButton_3_clicked()  // Редактирование
{
    if (currentItem)
    {
        QString newName = getName();
        currentItem->setText(1, newName);
        QSqlQuery* query = new QSqlQuery();
        query->prepare("UPDATE TableNames SET Name = :name WHERE ID = :id");
        query->bindValue(":name", newName);
        query->bindValue(":id", currentItem->text(0));
        if(!query->exec())
            QMessageBox::warning(this, tr("Предупреждение!"),
                                           tr("Проверьте правильность запроса!!!"),
                                           QMessageBox::Ok
                                           | QMessageBox::Cancel);
    }
    SelectDB();
    showAll();
}


void MainWindow::on_pushButton_4_clicked()  // Вывод из базы в дерево
{
    QSqlQuery *query = new QSqlQuery();
    query->prepare("SELECT * FROM TableNames ORDER BY Parent_ID ASC");
    ui->treeWidget->clear();
    if(!query->exec())
    {
        QMessageBox::warning(this, tr("Предупреждение!"),
                                       tr("Проверьте правильность запроса!!!"),
                                       QMessageBox::Ok
                                       | QMessageBox::Cancel);
    }
    while(query->next())
    {
        QStringList inpText;
        inpText << QString("%1").arg(query->value(0).toString()) << QString("%1").arg(query->value(1).toString());
        if(query->value(2).toInt() != 0)
        {
            ui->treeWidget->expandAll();
            QList<QTreeWidgetItem*> fItems = ui->treeWidget->findItems(query->value(2).toString(), Qt::MatchContains | Qt::MatchRecursive, 0);
            if(!fItems.isEmpty())
            {
                InsertItem(inpText, fItems.first());
            }
            else
            {
                InsertItem(inpText);
            }
        }
        else
        {
            InsertItem(inpText);
        }
        count = (query->value(0).toInt() > count) ? query->value(0).toInt() : count;
    }

    showAll();
}

