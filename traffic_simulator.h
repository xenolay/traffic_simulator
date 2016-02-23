#ifndef TRAFFIC_SIMULATOR_H
#define TRAFFIC_SIMULATOR_H

#include <QWidget>

namespace Ui {
class traffic_simulator;
}

class traffic_simulator : public QWidget
{
    Q_OBJECT

public:
    explicit traffic_simulator(QWidget *parent = 0);
    ~traffic_simulator();

private:
    Ui::traffic_simulator *ui;
};

#endif // TRAFFIC_SIMULATOR_H
