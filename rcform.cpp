#include "rcform.h"
#include "ui_rcform.h"
#include <QMessageBox>
#include <QSettings>
#include <QCloseEvent>
#include <QFile>

rcForm::rcForm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::rcForm)
{
    ui->setupUi(this);

    QSettings *settings=new QSettings("rc.ini", QSettings::IniFormat);

    settings->beginGroup("Main");
    setGeometry(settings->value("geometry", QRect(100,100,800,791)).toRect());
    QVBoxLayout *LTab1 = new QVBoxLayout;
    LTab1->setMargin(0);
    ui->tab->setLayout(LTab1);
    LTab1->addWidget(ui->tabScroll);

    QVBoxLayout *LTab2 = new QVBoxLayout;
    LTab2->setMargin(0);
    ui->tab_2->setLayout(LTab2);
    LTab2->addWidget(ui->tab2Scroll);

    ui->centralWidget->setLayout(ui->verticalLayout_3);
    ui->verticalLayout_3->setMargin(5);
    ui->console->clear();

    b = new Block();
    b->addItem(ui,this); //указатель на текущий элемент
}

rcForm::~rcForm()
{
    delete ui;
}


void rcForm::on_connect_triggered()
{
    /*   b = first;
    do {
        if(ui->connect->isChecked()){
            switch(b->setConnect()){
            case 0: message(QString("Порт %1 не подключен").arg(b->port()),ERROR); break;
            case 1:message(QString("Порт %1 подключен к %2").arg(b->port()).arg(b->name()),INFO); break;
            case 2: message(QString("Порт \"%1\" не выбран").arg(b->name()),INFO); break;

            }}
            else {
                if(b->setDiscon())
                    message(QString("Порт %1 отключен").arg(b->port()),INFO);
                else
                    message(QString("Порт %1 не отключен").arg(b->port()),ERROR);}
            b=b->next;
        }
    while(b != first);
        bool connect = false;

        b = b->next;

        do{
            if(b->connected()){
                connect = true;}
            b=b->next;
        }
        while(b != first);
        if(!connect){
            ui->connect->setChecked(false);
            ui->connect->setText("Подключить");}
        else
        {
            ui->connect->setChecked(true);
            ui->connect->setText("Отключить");}
*/
}


void rcForm::on_about_triggered()
{
    QMessageBox::about(this,"О программе",
                       "<p>Программа <b>RNC</b> позволяет упралять<br />"
                       "несколькими блоками NXT и получать<br /> информацию "
                       "о блоке: имя, заряд батареи.</p>");
}
void rcForm::message(QString msg,int type = DEBUG)
{
    QString color,icontype;
    switch(type){
    case INFO:
        color = "blue";
        icontype="info";
        break;
    case ERROR:
        color = "red";
        icontype="error";
        break;
    case DEBUG:
        color = "orange";
        icontype="debug";
        break;
    case SUCCES:
        color = "green";
        icontype="info";
        break;
    }
    QDateTime dt = QDateTime::currentDateTime();
    ui->console->insertHtml("<span style = \"color:"+color+"\"><img src=\":/"+icontype+".png\" /> ["+dt.time().toString()+"] "+msg.toStdString().c_str()+"</span><br>");
    ui->console->moveCursor(ui->console->textCursor().Down);
}

void rcForm::closeEvent(QCloseEvent *event)
{
    QMessageBox *msgBox = new QMessageBox();

    msgBox->setText("<b>Сохранение настроек.</b>");
    msgBox->setInformativeText("Сохранить настройки перед выходом?");
    QPushButton *Yes = msgBox->addButton("Да",QMessageBox::YesRole);
    QPushButton *No = msgBox->addButton("Нет", QMessageBox::NoRole);
    QPushButton *Cancel = msgBox->addButton("Отмена", QMessageBox::ApplyRole);
    msgBox->exec();

    if (msgBox->clickedButton() == Yes) {
        // Да
        QSettings *settings=new QSettings("rc.ini", QSettings::IniFormat);
        settings->clear();
        //Параметры главного окна
        settings->beginGroup("Main");
        settings->setValue("geometry", geometry());
        settings->endGroup();

    }
    else
        if (msgBox->clickedButton() == No) {
            // Нет
        }
        else
            if (msgBox->clickedButton() == Cancel) {
                // Отмена}
                event->ignore();
            }
}

void rcForm::on_toolButton_clicked()
{
    if(ui->console->isHidden()){
        setFixedSize(800,773);
        ui->console->setHidden(false);}
    else{
        setFixedSize(800,650);
        ui->console->setHidden(true);}
}

void rcForm::on_add_triggered()
{
    if (b->addItem(ui,this))
        message(QString("Блок %1 добавлен").arg(b->count()),INFO);
    else
        message(QString("Блок %1 не добавлен (количество подключений ограничено 10-ю)").arg(b->count()+1),ERROR);
}

void rcForm::on_toolButton_pressed()
{

}

void rcForm::on_dockWidget_allowedAreasChanged(const Qt::DockWidgetAreas &allowedAreas)
{

}

Block::Block() : QObject()
{
    c = 0;
    for(int i = 0; i < 10; i++)
        items[i] = NULL;
}

bool Block::addItem(Ui::rcForm *parent, QMainWindow *Form)
{
    if(count() < 10)
    {
        items[count()] = new MyBlock(count(),parent,Form);
        QObject::connect((QObject *)(items[count()]->del),SIGNAL(clicked(int)),this,SLOT(delItem(int)));
        c++;
        return 1;
    }
    return 0;
}

void Block::delItem(int num)
{
    MyBlock *temp = items[num];
    for(int i = c-1;i > num; i--) {
        items[i]->Block->setGeometry(items[i-1]->Block->geometry());
        items[i]->ContrlGroup->setGeometry(items[i-1]->ContrlGroup->geometry());
        items[i]->setIndex(i-1);
    }
    for(int i = num;i < c-1; i++)
        items[i] = items[i+1];

    delete temp;
    if(c-1 != num)
        items[c-1] = NULL;
    c--;
}

int Block::count()
{
    return c;
}
