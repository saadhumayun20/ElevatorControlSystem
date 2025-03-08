#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), ecs(5, 2), activePassengerId(1) {
    ui->setupUi(this);
    initializeGrid();
    setupElevatorControls();
    setupOverloadControls();

    // Connect floor buttons.
    connect(ui->floor1Up, &QPushButton::clicked, [this]() { on_upButton_pressed(1); });
    connect(ui->floor2Up, &QPushButton::clicked, [this]() { on_upButton_pressed(2); });
    connect(ui->floor3Up, &QPushButton::clicked, [this]() { on_upButton_pressed(3); });
    connect(ui->floor4Up, &QPushButton::clicked, [this]() { on_upButton_pressed(4); });

    connect(ui->floor2Down, &QPushButton::clicked, [this]() { on_downButton_pressed(2); });
    connect(ui->floor3Down, &QPushButton::clicked, [this]() { on_downButton_pressed(3); });
    connect(ui->floor4Down, &QPushButton::clicked, [this]() { on_downButton_pressed(4); });
    connect(ui->floor5Down, &QPushButton::clicked, [this]() { on_downButton_pressed(5); });

    connect(ui->passengerSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
        activePassengerId = index + 1;
        ui->logConsole->appendPlainText(QString("Active Passenger: %1").arg(activePassengerId));
    });
    connect(ui->powerOutButton, &QPushButton::clicked, this, &MainWindow::on_powerOutButton_clicked);

    timer = new QTimer(this);
    timer->setInterval(10000);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateGUI);

}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::initializeGrid() {
    for (int floor = 1; floor <= 5; ++floor) {
        for (int elev = 1; elev <= 2; ++elev) {
            QString labelName = QString("E%1_Floor%2").arg(elev).arg(floor);
            QLabel *label = findChild<QLabel *>(labelName);
            if (label) {
                label->setText("○");
                label->setStyleSheet("color: black;");
            } else {
                qDebug() << "Label not found:" << labelName;
            }
        }
    }
}

void MainWindow::setupElevatorControls() {
    // Elevator 1 Controls.
    connect(ui->elev1Floor1Btn, &QPushButton::clicked, [this]() { handleElevatorFloorButton(1, 1); });
    connect(ui->elev1Floor2Btn, &QPushButton::clicked, [this]() { handleElevatorFloorButton(1, 2); });
    connect(ui->elev1Floor3Btn, &QPushButton::clicked, [this]() { handleElevatorFloorButton(1, 3); });
    connect(ui->elev1Floor4Btn, &QPushButton::clicked, [this]() { handleElevatorFloorButton(1, 4); });
    connect(ui->elev1Floor5Btn, &QPushButton::clicked, [this]() { handleElevatorFloorButton(1, 5); });
    connect(ui->elev1OpenBtn, &QPushButton::clicked, [this]() { handleOpenDoor(1); });
    connect(ui->elev1CloseBtn, &QPushButton::clicked, [this]() { handleCloseDoor(1); });

    // Elevator 2 Controls.
    connect(ui->elev2Floor1Btn, &QPushButton::clicked, [this]() { handleElevatorFloorButton(2, 1); });
    connect(ui->elev2Floor2Btn, &QPushButton::clicked, [this]() { handleElevatorFloorButton(2, 2); });
    connect(ui->elev2Floor3Btn, &QPushButton::clicked, [this]() { handleElevatorFloorButton(2, 3); });
    connect(ui->elev2Floor4Btn, &QPushButton::clicked, [this]() { handleElevatorFloorButton(2, 4); });
    connect(ui->elev2Floor5Btn, &QPushButton::clicked, [this]() { handleElevatorFloorButton(2, 5); });
    connect(ui->elev2OpenBtn, &QPushButton::clicked, [this]() { handleOpenDoor(2); });
    connect(ui->elev2CloseBtn, &QPushButton::clicked, [this]() { handleCloseDoor(2); });

    connect(ui->elev1HelpBtn, &QPushButton::clicked, [this]() { handleHelpButton(1); });
    connect(ui->elev2HelpBtn, &QPushButton::clicked, [this]() { handleHelpButton(2); });
}

void MainWindow::handleElevatorFloorButton(int elevId, int floor) {
    const auto &elevators = ecs.getElevators();
    if (elevId == 0 || static_cast<size_t>(elevId - 1) >= elevators.size())
        return;
    auto elevator = elevators[elevId - 1];
    bool passengerInElevator = false;
    for (const auto &passenger : elevator->getPassengers()) {
        if (passenger.getId() == activePassengerId) {
            passengerInElevator = true;
            break;
        }
    }
    if (passengerInElevator) {
        // Update the passenger's destination.
        for (auto &passenger : elevator->getPassengers()) {
            if (passenger.getId() == activePassengerId) {
                passenger.setDestinationFloor(floor);
                break;
            }
        }
        elevator->addRequests(floor, true);
        ui->logConsole->appendPlainText(
            QString("Passenger %1 selected Floor %2 in Elevator %3")
                .arg(activePassengerId)
                .arg(floor)
                .arg(elevId)
        );
    } else {
        ui->logConsole->appendPlainText(
            QString("Passenger %1 is not inside Elevator %2").arg(activePassengerId).arg(elevId)
        );
    }
}

void MainWindow::setupOverloadControls() {
    // Elevator 1
    connect(ui->overloadElev1Btn, &QPushButton::clicked, [this]() { handleOverloadButton(1); });
    connect(ui->reduceLoadElev1Btn, &QPushButton::clicked, [this]() { handleReduceLoadButton(1); });

    // Elevator 2
    connect(ui->overloadElev2Btn, &QPushButton::clicked, [this]() { handleOverloadButton(2); });
    connect(ui->reduceLoadElev2Btn, &QPushButton::clicked, [this]() { handleReduceLoadButton(2); });
}

void MainWindow::handleOverloadButton(int elevId) {
    ecs.handleOverload(elevId);
    ui->logConsole->appendPlainText(
        QString("Elevator %1 overloaded! Please reduce load").arg(elevId)
    );
}

void MainWindow::handleReduceLoadButton(int elevId) {
    ecs.resolveOverload(elevId);
    ui->logConsole->appendPlainText(
        QString("Load reduced for Elevator %1").arg(elevId)
    );
}

void MainWindow::handleOpenDoor(int elevId) {
    if (elevId == 0 || static_cast<size_t>(elevId - 1) >= ecs.getElevators().size())
        return;
    auto elevator = ecs.getElevators()[elevId - 1];
    elevator->openDoor();
    elevator->doorOpenSteps = 0;
    ui->logConsole->appendPlainText(QString("Doors opened for Elevator %1").arg(elevId));
}

void MainWindow::handleCloseDoor(int elevId) {
    if (elevId == 0 || static_cast<size_t>(elevId - 1) >= ecs.getElevators().size())
        return;
    ecs.getElevators()[elevId - 1]->closeDoor();
    ui->logConsole->appendPlainText(QString("Doors closed for Elevator %1").arg(elevId));
}

void MainWindow::handleHelpButton(int elevId) {
    auto &elevators = ecs.getElevators();
    if (elevId == 0 || static_cast<size_t>(elevId - 1) >= elevators.size())
        return;
    auto elevator = elevators[elevId - 1];
    bool passengerInside = false;
    for (const auto &p : elevator->getPassengers()) {
        if (p.getId() == activePassengerId) {
            passengerInside = true;
            break;
        }
    }
    if (passengerInside) {
        elevator->pressHelpButton();
        ecs.emergencyStates[elevId] = {true, 0, elevId};
        ui->logConsole->appendPlainText(
            QString("Passenger %1 pressed HELP in Elevator %2 - Connecting...")
                .arg(activePassengerId)
                .arg(elevId)
        );
        updateEmergencyStatusDisplay();
    }
}

void MainWindow::on_handleHelpButton_clicked() {
    bool handled = false;
    auto &emergencies = ecs.emergencyStates;
    for (auto &pair : emergencies) {
        int elevId = pair.first;
        auto &status = pair.second;
        if (status.helpRequested) {
            ecs.handleHelpResponse(elevId);
            ui->logConsole->appendPlainText(
                QString("Response received - Problem solved in Elevator %1").arg(elevId)
            );
            handled = true;
        }
    }
    if (!handled) {
        ui->logConsole->appendPlainText("No active emergencies to handle");
    }
    updateEmergencyStatusDisplay();
}

void MainWindow::updateEmergencyStatusDisplay() {
    QString statusText;
    const auto &emergencies = ecs.emergencyStates;
    for (const auto &pair : emergencies) {
        int elevId = pair.first;
        auto &status = pair.second;
        if (status.helpRequested) {
            statusText += QString("Elevator %1: %2/5 steps\n")
                              .arg(elevId)
                              .arg(status.timeElapsed);
        }
    }
    ui->emergencyStatusLabel->setText(statusText.isEmpty() ? "No active emergencies" : statusText);
}

void MainWindow::on_startButton_clicked() {
    ecs.startSimulation();
    timer->start(2000);
    ui->logConsole->appendPlainText("Simulation started.");
}

void MainWindow::on_pauseButton_clicked() {
    ecs.pauseSimulation();
    timer->stop();
    ui->logConsole->appendPlainText("Simulation paused.");
}

void MainWindow::on_resumeButton_clicked() {
    ecs.resumeSimulation();
    timer->start(2000);
    ui->logConsole->appendPlainText("Simulation resumed.");
}

void MainWindow::on_stopButton_clicked() {
    ecs.stopSimulation();
    timer->stop();
    ui->logConsole->appendPlainText("Simulation stopped.");
}

void MainWindow::on_testFireAlarmButton_clicked() {
    ecs.triggerFireAlarm();
    ui->logConsole->appendPlainText("Fire alarm triggered!");
}

void MainWindow::on_upButton_pressed(int floor) {
    ecs.addRequest(floor, "up");
    QString logMessage = QString("Passenger %1 pressed UP button on Floor %2")
                             .arg(activePassengerId)
                             .arg(floor);
    ui->logConsole->appendPlainText(logMessage);
    auto &floors = const_cast<vector<Floor>&>(ecs.getFloors());
    if (floor - 1 < static_cast<int>(floors.size())) {
        auto activePassenger = make_unique<Passenger>(activePassengerId, floor);
        //Passenger *activePassenger = new Passenger(activePassengerId, floor);
        floors[floor - 1].addPassenger(*activePassenger);
    }
    QString buttonName = QString("floor%1Up").arg(floor);
    QPushButton *button = findChild<QPushButton *>(buttonName);
    if (button) {
        button->setStyleSheet("background-color: yellow;");
        QTimer::singleShot(5000, this, [this, buttonName]() {
            QPushButton *btn = findChild<QPushButton *>(buttonName);
            if (btn) { btn->setStyleSheet(""); }
        });
    }
}

void MainWindow::on_downButton_pressed(int floor) {
    ecs.addRequest(floor, "down");
    QString logMessage = QString("Passenger %1 pressed DOWN button on Floor %2")
                             .arg(activePassengerId)
                             .arg(floor);
    ui->logConsole->appendPlainText(logMessage);
    auto &floors = const_cast<vector<Floor>&>(ecs.getFloors());
    if (floor - 1 < static_cast<int>(floors.size())) {
        auto activePassenger = make_unique<Passenger>(activePassengerId, floor);
        //Passenger *activePassenger = new Passenger(activePassengerId, floor);
        floors[floor - 1].addPassenger(*activePassenger);
    }
    QString buttonName = QString("floor%1Down").arg(floor);
    QPushButton *button = findChild<QPushButton *>(buttonName);
    if (button) {
        button->setStyleSheet("background-color: yellow;");
        QTimer::singleShot(5000, this, [this, buttonName]() {
            QPushButton *btn = findChild<QPushButton *>(buttonName);
            if (btn) { btn->setStyleSheet(""); }
        });
    }
}

void MainWindow::on_destinationSelected(int floor) {
    QString logMessage = QString("Passenger %1 selected destination Floor %2")
                             .arg(activePassengerId)
                             .arg(floor);
    ui->logConsole->appendPlainText(logMessage);
    Logger::instance().appendLog(logMessage.toStdString());
}

void MainWindow::on_powerOutButton_clicked() {
    ecs.triggerPowerOut();
    ui->logConsole->appendPlainText("POWER OUTAGE DETECTED");
    ui->logConsole->appendPlainText("Switching to backup power");
    ui->logConsole->appendPlainText("All elevators moving to floor 1");
}


void MainWindow::updateGUI() {
    ecs.advanceTimeStep();
    updateEmergencyStatusDisplay();
    const auto &elevators = ecs.getElevators();
    const auto &floors = ecs.getFloors();
    for (size_t elevIdx = 0; elevIdx < elevators.size(); ++elevIdx) {
        int currentFloor = elevators[elevIdx]->getCurrentFloor();
        QString state = QString::fromStdString(elevators[elevIdx]->getState());
        for (int floor = 1; floor <= 5; ++floor) {
            QString labelName = QString("E%1_Floor%2").arg(elevIdx + 1).arg(floor);
            QLabel *label = findChild<QLabel *>(labelName);
            if (label) {
                if (floor == currentFloor) {
                    label->setText("●");
                    label->setStyleSheet(state == "moving" ? "color: blue;" : "color: green;");
                } else {
                    label->setText("○");
                    label->setStyleSheet("color: black;");
                }
            }
        }
        ui->logConsole->appendPlainText(
            QString("Elevator %1 is at Floor %2 (%3)")
                .arg(elevIdx + 1)
                .arg(currentFloor)
                .arg(state)
        );
    }
    for (size_t floorIdx = 0; floorIdx < floors.size(); ++floorIdx) {
        const Floor &floor = floors[floorIdx];
        int floorNumber = floor.getFloorNumber();
        QString upButtonName = QString("floor%1Up").arg(floorNumber);
        QPushButton *upButton = findChild<QPushButton *>(upButtonName);
        if (upButton) {
            upButton->setStyleSheet(floor.isUpButtonPressed() ? "background-color: yellow;" : "");
        }
        QString downButtonName = QString("floor%1Down").arg(floorNumber);
        QPushButton *downButton = findChild<QPushButton *>(downButtonName);
        if (downButton) {
            downButton->setStyleSheet(floor.isDownButtonPressed() ? "background-color: yellow;" : "");
        }
    }
    // Append simulation log messages.
    auto simLogs = Logger::instance().retrieveLogs();
    for (const auto &log : simLogs) {
        ui->logConsole->appendPlainText(QString::fromStdString(log));
    }
    ui->logConsole->appendPlainText(QString("Time Step: %1\n").arg(ecs.getCurrentTimeStep()));
}
