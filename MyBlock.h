#ifndef BLOCKVIEW_H
#define BLOCKVIEW_H

#include <QPushButton>
#include <QGroupBox>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include <QSpinBox>
#include <QTimer>
#include <QVBoxLayout>

#include "ui_rcform.h"
#include "BtNXT.h"


enum {INFO,ERROR,DEBUG,SUCCES};

#ifdef Q_OS_WIN
#define PORT "COM%1"
#define WIN 1
#else
#define PORT "/dev/rfcomm%1"
#define WIN 0
#endif

class MyBlock;

class RcSens : public QComboBox
{
    Q_OBJECT

public:
    short port;
    int count;
    QCheckBox *update;
    RcSens(int x, int y, int port,BtNXT *Block, QWidget *parent,QWidget *parent2);
    QLabel *L1;
    QLabel *L2;
    QComboBox *SensChanged;
    QLineEdit *Value;
    void addItem(QString item);
protected:
    void changeEvent (QEvent *e );
signals:
    void onChange(int index, short port);
public slots:
};

class RcButton : public QPushButton
{
    Q_OBJECT

public:
    short command;
    short power;
    RcButton(QWidget *parent);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
signals:
    void pressed(short motor, short power);
    void released(short motor, short power);
};

class CloseButton : public QPushButton
{
    Q_OBJECT

public:
    int index;
    CloseButton(QWidget *parent, int index);

protected:
    void mouseReleaseEvent(QMouseEvent *event);
signals:
    void clicked(int index);
};

class PowerChanger : public QSpinBox
{
    Q_OBJECT
public:
    QLabel *label;
    QSlider *slide;
    PowerChanger(int value, int x, int y, QString Labe, QWidget *parent);
};

class MyBlock : public BtNXT
{
    Q_OBJECT

private:
    QTimer *timer;
    QTimer *timerUpdBat;
    int timercount;
    //QTextEdit *console;

    void fillPort(int count);
    void fillSens(int i);

    QComboBox *Port; //Список портов
    QLabel *LabePort;
    QLabel *LabeName;

    RcSens *Sens[4]; //Настройки сенсоров

    QLabel *LabeTachoPort[3];

    //Кнопки управления
    RcButton *aFw;
    RcButton *aBk;
    RcButton *bFw;
    RcButton *bBk;
    RcButton *cFw;
    RcButton *cBk;

    QPushButton *rfName; //Кнопка обновления имени

    QGroupBox *InvGroup; //Группа инверсии двигателей
    QGroupBox *SetContrlGroup; //Группа настройики управления
    QGroupBox *SensGroup; //Группа настройки сенсоров

    QGroupBox *MotorGroup; //Группа данных с мотора

    QGroupBox *PowerGroup; //Группа мощности моторов
    QGroupBox *SensContrGroup; //Группа датчиков на вкладке управления
    QGroupBox *TachoGroup; //Группа датчиков оборотов


    QLineEdit *Tacho[3]; //Значения датчиков оборотов
    QLineEdit *Name; //Значение имени
    PowerChanger *powerSet[3]; //Класс управления мощьностью

    QComboBox *SetContrl; //Выбор управления
    QCheckBox *Inv[3]; //Инверсия двигателей
    QCheckBox *UpdateTacho[3]; //Обновление датчиков оборотов
    QCheckBox *RepBC; //Менять местами двигатели B и С
    QCheckBox *RegSynchBC; //Объединить регулировку мощности двигателей В и С
    QAction *Batt; //Кнопка баттареи

public:
    CloseButton *del; //Кнопка удаления текущего блока
    QGroupBox *Block; //Основная группа настроек
    QGroupBox *ContrlGroup; //Главная группа на вкладке управления
    void setIndex(int index);
    MyBlock(int blck, Ui::rcForm *parent,QMainWindow *Form);
    ~MyBlock();
    QString name();
    QString port();
public slots:
    int setConnect();
    void setBat(float volt);
    void updBat();
    void updTacho();
    void updName();

    void controlChange(int index);
    void updSens();
    void Timer();
    // void setRegSynchBC(bool checked);
};

#endif // BLOCKVIEW_H
