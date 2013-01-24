#include "MyBlock.h"

MyBlock::MyBlock(int blck, Ui::rcForm *parent,QMainWindow *Form) : BtNXT()
{
    timer = new QTimer(this);
    timer->stop();
    timer->setInterval(500);
    timercount = 0;
    QObject::connect(timer, SIGNAL(timeout()), this , SLOT(Timer()));

    timerUpdBat = new QTimer(this);
    timerUpdBat->stop();
    timerUpdBat->setInterval(20000);
    QObject::connect(timerUpdBat, SIGNAL(timeout()), this , SLOT(updBat()));

    //console = parent->console;

    Block = new QGroupBox(parent->tabScrollContents);
    Block->setGeometry(5,10+(151*blck),745,151);
    Block->setTitle(QString("Блок %1").arg(blck));
    Block->show();
    parent->tabScrollContents->setMinimumHeight((151*(blck+1)));

    ContrlGroup = new QGroupBox(parent->tab2ScrollContents);
    ContrlGroup->setGeometry(5,10+(151*blck),745,151);
    ContrlGroup->setTitle(QString("Блок %1").arg(blck));
    ContrlGroup->show();
    parent->tab2ScrollContents->setMinimumHeight((151*(blck+1)));

    InvGroup = new QGroupBox(Block);
    InvGroup->setGeometry(205,10,91,131);
    InvGroup->setTitle("Инверсия");
    InvGroup->show();

    SetContrlGroup = new QGroupBox(Block);
    SetContrlGroup->setGeometry(300,10,191,131);
    SetContrlGroup->setTitle("Управление");
    SetContrlGroup->show();

    PowerGroup = new QGroupBox(ContrlGroup);
    PowerGroup->setGeometry(5,20,181,121);
    PowerGroup->setTitle("Мощность");
    PowerGroup->show();

    SensContrGroup = new QGroupBox(ContrlGroup);
    SensContrGroup->setGeometry(405,10,196,131);
    SensContrGroup->setTitle("Датчики");
    SensContrGroup->show();

    TachoGroup = new QGroupBox(ContrlGroup);
    TachoGroup->setGeometry(190,20,211,121);
    TachoGroup->setTitle("Обороты");
    TachoGroup->show();

    SensGroup = new QGroupBox(Block);
    SensGroup->setGeometry(495,10,217,131);
    SensGroup->setTitle("Датчики");
    SensGroup->show();

    LabePort = new QLabel(Block);
    LabePort->setGeometry(20,20,46,13);
    LabePort->setText("Порт");
    LabePort->show();

    LabeName = new QLabel(Block);
    LabeName->setGeometry(20,70,46,13);
    LabeName->setText("Имя");
    LabeName->show();

    Port = new QComboBox(Block);
    Port->setGeometry(20,40,171,22);
    if(!WIN)
        fillPort(15);
    else
        fillPort(50);
    Port->show();

    Name = new QLineEdit(Block);
    Name->setGeometry(20,90,145,22);
    Name->setText(QString("Блок %1").arg(blck));
    Name->show();

    rfName = new QPushButton(Block);
    rfName->setGeometry(170,90,22,22);
    rfName->setIcon(QIcon(":/rfName"));
    rfName->setIconSize(QSize(22,22));
    rfName->setFlat(true);
    rfName->show();
    QObject::connect(rfName, SIGNAL(clicked()), this , SLOT(updName()));

    Batt = new QAction(Form);
    Batt->setParent(parent->mainToolBar);
    Batt->setIcon(QIcon(":/batmis.png"));
    Batt->setText(QString("Блок %1").arg(blck));
    parent->mainToolBar->insertAction(parent->sep,Batt);
    QObject::connect(Batt, SIGNAL(triggered()), this , SLOT(updBat()));

    for(int i = 0,j = 20 ; i < 3; i++,j+=30){
        Inv[i] = new QCheckBox(InvGroup);
        Inv[i]->setGeometry(10,j,71,17);
        Inv[i]->setText("Порт "+QString('A'+i));
        Inv[i]->show();

        UpdateTacho[i] = new QCheckBox(TachoGroup);
        UpdateTacho[i]->setGeometry(120,j+12,100,17);
        UpdateTacho[i]->setText("Обновлять");
        UpdateTacho[i]->show();

        powerSet[i] = new PowerChanger(100,5,j+10,QString('A'+i),PowerGroup);
        powerSet[i]->show();

        LabeTachoPort[i] = new QLabel(TachoGroup);
        LabeTachoPort[i]->setGeometry(5,j+14,16,16);
        LabeTachoPort[i]->setText(QString('A'+i));
        LabeTachoPort[i]->show();

        Tacho[i] = new QLineEdit(TachoGroup);
        Tacho[i]->setGeometry(20,j+12,96,20);
        Tacho[i]->setText(QString("%1").arg(0));
        Tacho[i]->show();
    }

    SetContrl = new QComboBox(SetContrlGroup);
    SetContrl->setGeometry(10,20,171,22);
    SetContrl->addItem("Движение");
    SetContrl->addItem("Раздельное");
    SetContrl->setToolTip("<span >Выбор связки и расположения кнопок:</span><br />"\
                          "<span style=\"font-weight:600;\">Движение</span><br /><br />"\
                          "<span>Располажение кнопок</span><br /><img src=\":/dvig.png\" /><br /><br />"\
                          "<span style=\"font-weight:600;\">Раздельное</span><bt />"\
                          "<span>Расположение кнопок</span><br /><img src=\":/differenc.png\" />");
    QObject::connect(SetContrl, SIGNAL(currentIndexChanged(int)), this , SLOT(controlChange(int)));
    SetContrl->show();

    RepBC = new QCheckBox(SetContrlGroup);
    RepBC->setGeometry(10,60,121,17);
    RepBC->setText("Поменять В и С");
    RepBC->show();

    RegSynchBC = new QCheckBox(SetContrlGroup);
    RegSynchBC->setGeometry(10,85,141,17);
    RegSynchBC->setText("Объединить В и С");
    RegSynchBC->setToolTip("Объединить регулировку<br /> мощности двигателей В и С");
    RegSynchBC->show();
    //QObject::connect(RegSynchBC, SIGNAL(clicked(bool)), this , SLOT(setRegSynchBC(bool)));

    for(int i = 0,j = 20 ; i < 4; i++,j+=28){
        Sens[i] = new RcSens(10,j,i,this,SensGroup,SensContrGroup);
        fillSens(i);
    }


    aFw = new RcButton(ContrlGroup);
    aFw->setGeometry(605,80,31,27);
    aFw->command = A;
    aFw->setIcon(QIcon(":/up.png"));
    aFw->setIconSize(QSize(22,22));
    aFw->show();
    QObject::connect(aFw, SIGNAL(pressed(short,short)), this , SLOT(run(short,short)));
    QObject::connect(aFw, SIGNAL(released(short,short)), this , SLOT(run(short,short)));

    aBk = new RcButton(ContrlGroup);
    aBk->setGeometry(605,110,31,27);
    aBk->setIcon(QIcon(":/down.png"));
    aBk->command = -A;
    aBk->setIconSize(QSize(22,22));
    aBk->show();
    QObject::connect(aBk, SIGNAL(pressed(short,short)), this , SLOT(run(short,short)));
    QObject::connect(aBk, SIGNAL(released(short,short)), this , SLOT(run(short,short)));

    bFw = new RcButton(ContrlGroup);
    bFw->setIcon(QIcon(":/up.png"));
    bFw->setIconSize(QSize(22,22));
    bFw->show();
    QObject::connect(bFw, SIGNAL(pressed(short,short)), this , SLOT(run(short,short)));
    QObject::connect(bFw, SIGNAL(released(short,short)), this , SLOT(run(short,short)));

    bBk = new RcButton(ContrlGroup);
    bBk->setIcon(QIcon(":/down.png"));
    bBk->setIconSize(QSize(22,22));
    bBk->show();
    QObject::connect(bBk, SIGNAL(pressed(short,short)), this , SLOT(run(short,short)));
    QObject::connect(bBk, SIGNAL(released(short,short)), this , SLOT(run(short,short)));

    cFw = new RcButton(ContrlGroup);
    cFw->setIconSize(QSize(22,22));
    cFw->show();
    QObject::connect(cFw, SIGNAL(pressed(short,short)), this , SLOT(run(short,short)));
    QObject::connect(cFw, SIGNAL(released(short,short)), this , SLOT(run(short,short)));

    cBk = new RcButton(ContrlGroup);
    cBk->setIconSize(QSize(22,22));
    cBk->show();
    QObject::connect(cBk, SIGNAL(pressed(short,short)), this , SLOT(run(short,short)));
    QObject::connect(cBk, SIGNAL(released(short,short)), this , SLOT(run(short,short)));

    del = new CloseButton(Block,blck);
    del->setGeometry(715,15,22,22);
    del->show();
    controlChange(0);

}


int MyBlock::setConnect()
{
    if(Port->currentIndex() != 0){
        if(WIN && Port->currentIndex() > 9)
            return setCon("\\\\.\\"+Port->currentText());
        return setCon(Port->currentText());
    }
    else
        return 2;
}

void MyBlock::fillSens(int i)
{
    Sens[i]->addItem("Нет");
    Sens[i]->addItem("Кнопка");
    Sens[i]->addItem("Датчик света");
    Sens[i]->addItem("Датчик звука");
    Sens[i]->addItem("Ультразвуковой");
    Sens[i]->addItem("Датчик цвета");
}

void MyBlock::fillPort(int count)
{
    Port->addItem("Нет");
    for(int i = 0; i <= count; i++){
        Port->addItem(QString(PORT).arg(i));}
}

PowerChanger::PowerChanger(int value, int x, int y, QString Labe, QWidget *parent):QSpinBox(parent)
{
    setMaximum(100);
    label = new QLabel(parent);
    label->setGeometry(x,y,16,16);
    label->setText(Labe);
    label->show();

    slide = new QSlider(parent);
    slide->setGeometry(x+15,y,101,19);
    slide->setOrientation(Qt::Horizontal);
    slide->setMaximum(100);
    slide->show();

    show();

    setGeometry(x+120,y,47 ,22);
    QObject::connect(this,SIGNAL(valueChanged(int)),slide,SLOT(setValue(int)));
    QObject::connect(slide,SIGNAL(valueChanged(int)),this,SLOT(setValue(int)));
    setValue(value);
}

RcSens::RcSens(int x, int y, int Port,BtNXT *block, QWidget *parent, QWidget *parent2):QComboBox(parent)
{
    show();
    port = Port;
    setGeometry(x+15,y,100,20);
    L1 = new QLabel(parent);
    L1->setGeometry(x,y+3,9,16);
    L1->setText(QString("%1:").arg(Port));
    L1->show();
    update = new QCheckBox;
    update->setParent(parent);
    update->setGeometry(x+120,y+2,120,17);
    update->setText("Обновлять");
    update->show();

    SensChanged = new QComboBox(parent2);
    SensChanged->setGeometry(x+15,y,110,20);
    SensChanged->show();
    L2 = new QLabel(parent2);
    L2->setGeometry(x,y+3,9,16);
    L2->setText(QString("%1:").arg(Port));
    L2->show();
    Value = new QLineEdit(parent2);
    Value->setGeometry(x+130,y,50,20);
    Value->show();

    QObject::connect(SensChanged,SIGNAL(currentIndexChanged(int)),this,SLOT(setCurrentIndex(int)));
    QObject::connect(this,SIGNAL(currentIndexChanged(int)),SensChanged,SLOT(setCurrentIndex(int)));
    QObject::connect(this,SIGNAL(onChange(int,short)),block,SLOT(setSensType(int,short)));
}

RcButton::RcButton(QWidget *parent) :QPushButton(parent)
{}

void MyBlock::controlChange(int index)
{
    switch(index){
    case 0:
        bFw->setGeometry(675,80,31,27);
        bFw->command = B+C;
        bBk->setGeometry(675,110,31,27);
        bBk->command = -B-C;
        cFw->setGeometry(640,110,31,27);
        cFw->setIcon(QIcon(":left.png"));
        cFw->command = B-C;
        cBk->setGeometry(710,110,31,27);
        cBk->setIcon(QIcon(":right.png"));
        cBk->command = C-B;
        break;
    case 1:
        bFw->setGeometry(640,80,31,27);
        bFw->command = B;
        bBk->setGeometry(640,110,31,27);
        bBk->command = -B;
        cFw->setGeometry(675,80,31,27);
        cFw->setIcon(QIcon(":up.png"));
        cFw->command = C;
        cBk->setGeometry(675,110,31,27);
        cBk->setIcon(QIcon(":down.png"));
        cBk->command = -C;
        break;
    }
}

void RcSens::addItem(QString item)
{
    QStringList items;
    items.insert(0,item);
    addItems(items);
    SensChanged->addItems(items);
}

void MyBlock::setBat(float volt)
{
    int percent = (volt-5.9)/ 0.023;
    Batt->setToolTip(QString("Напряжение %1 вольт - %2%").arg(volt, 2, 'L', 2).arg(percent));
    if(percent >= 80){
        Batt->setIcon(QIcon(":/bat100.png"));
        return;}
    if(percent >= 60){
        Batt->setIcon(QIcon(":/bat80.png"));
        return;}
    if(percent>= 40){
        Batt->setIcon(QIcon(":/bat60.png"));
        return;}
    if(percent >= 20){
        Batt->setIcon(QIcon(":/bat40.png"));
        return;}
    if(percent >= 10){
        Batt->setIcon(QIcon(":/bat20.png"));
        return;}
    if(percent > 0){
        Batt->setIcon(QIcon(":/batlow.png"));
        return;}
    if(percent <= 0){
        Batt->setIcon(QIcon(":/batmis.png"));
        return;
    }
}

void MyBlock::updSens()
{
    if(connected())
        for(int i = 0; i <4; i++)
        {   if(Sens[i]->update->isChecked()){
                int u;
                switch(Sens[i]->currentIndex()){
                case 1: Sens[i]->Value->setText(QString("%1").arg(getSensValue(i))); break;
                case 4: u = getUSonic(i); if(u) Sens[i]->Value->setText(QString("%1").arg(u/256)); break;}
            }}
}

void MyBlock::Timer()
{
    switch(timercount){
    case 0: updTacho(); timercount++; break;
    case 1: updSens(); timercount = 0; break;}

}

void MyBlock::updBat()
{
    setBat(getBatt());
}

void MyBlock::updName()
{
    Name->setText(getName());
}

void MyBlock::updTacho()
{
    for(int i = 0; i <3; i++)
    {
        Tacho[i]->setText(QString("%1").arg(getTacho(i,true)));
    }
}

void RcSens::changeEvent (QEvent *e)
{
    emit onChange(currentIndex(), port);
    emit currentIndexChanged(currentIndex());
}
void RcButton::mousePressEvent(QMouseEvent *event)
{
    emit pressed(command, 100);
}

void RcButton::mouseReleaseEvent(QMouseEvent *event)
{
    emit released(command, 0);
}

void MyBlock::setIndex(int index)
{
del->index = index;
}

QString MyBlock::name()
{
    return Name->text();
}

QString MyBlock::port()
{
    return Port->currentText();
}

void CloseButton::mouseReleaseEvent(QMouseEvent *event)
{
    emit released();
    emit clicked(index);
}

CloseButton::CloseButton(QWidget *parent, int ind) :QPushButton(parent)
{
index = ind;
setIconSize(QSize(16,16));
setIcon(QIcon(":/del"));
}

MyBlock::~MyBlock()
{
    timer->~QTimer();
    timerUpdBat->~QTimer();
    Block->~QGroupBox();
    ContrlGroup->~QGroupBox();
    Batt->~QAction();
}
