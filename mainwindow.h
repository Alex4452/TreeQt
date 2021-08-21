#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QWidget>
#include <QtWidgets>
#include <QInputDialog>
#include <QString>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QVector>

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();


    void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_tableView_clicked(const QModelIndex &index);

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::MainWindow *ui;
    int treeCount(QTreeWidget *, QTreeWidgetItem *); //подсчёт количества элементов в QTreeWidget
    void DeleteItem (QTreeWidgetItem *currentItem); //удаление элемента из QTreeWidget
    void InsertItem (QStringList, QTreeWidgetItem *); //добавление элемента в QTreeWidget
    void showAll(void); //вывод информации из QTreeWidget

    QString getName(); // получить имя выделенного узла
    void getChildID(QTreeWidgetItem*); //

    void ConnectDB();
    void SelectDB();
    void InsertDB(QString, QString, QString);
    QString getIdItem(QTreeWidgetItem*);

    QVector<QString> *childID = new QVector<QString>();
    int count;
    QTreeWidgetItem *currentItem;
    int currentColumn;

    QSqlDatabase db;
    QSqlTableModel* model;
    //QSqlQueryModel* model;

};
#endif // MAINWINDOW_H
