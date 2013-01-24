#ifndef RC_H
#define RC_H

#include <QMainWindow>

namespace Ui {
    class rc;
}

class rc : public QMainWindow
{
    Q_OBJECT

public:
    explicit rc(QWidget *parent = 0);
    ~rc();

private:
    Ui::rc *ui;
};

#endif // RC_H
