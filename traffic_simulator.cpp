#include "traffic_simulator.h"
#include "ui_traffic_simulator.h"

traffic_simulator::traffic_simulator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::traffic_simulator)
{
    ui->setupUi(this);
}

traffic_simulator::~traffic_simulator()
{
    delete ui;
}
