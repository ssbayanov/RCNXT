#ifndef RCFORM_H
#define RCFORM_H
#include "ui_rcform.h"
#include "MyBlock.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QMainWindow>

namespace Ui {
class rcForm;
}
class Block : public QObject
{
    Q_OBJECT
public:
    bool addItem(Ui::rcForm *parent, QMainWindow *Form);
    int connected(int num);
    bool connectAll();
    Block();
    int count();
private:
    MyBlock *items[10];
    int c;
public slots:
    void delItem(int num);
};

class rcForm : public QMainWindow
{
    Q_OBJECT

public:

    Block *b;

    int count;
    explicit rcForm(QWidget *parent = 0);
    ~rcForm();

    QLabel *label;

private slots:

    void on_connect_triggered();

    void on_about_triggered();

    void on_toolButton_clicked();

    void on_add_triggered();

    void on_toolButton_pressed();

    void on_dockWidget_allowedAreasChanged(const Qt::DockWidgetAreas &allowedAreas);

private:
    Ui::rcForm *ui;
    void message(QString msg,int type);
    void closeEvent(QCloseEvent *event);
};

#endif // RCFORM_H
