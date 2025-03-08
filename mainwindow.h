#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Logger.h"
#include "ElevatorControlSystem.h"
#include <QMainWindow>
#include <QTimer>
#include <QString>
#include <QLabel>
#include <QDebug>
#include <QMutex>
#include <QRegularExpression>
#include <QMutexLocker>
#include <QList>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Button handlers
    void on_startButton_clicked();
    void on_pauseButton_clicked();
    void on_resumeButton_clicked();
    void on_stopButton_clicked();
    void on_testFireAlarmButton_clicked();
    void on_upButton_pressed(int floor);
    void on_downButton_pressed(int floor);
    void on_destinationSelected(int floor);
    void handleElevatorFloorButton(int elevId, int floor);
    void handleOpenDoor(int elevId);
    void handleCloseDoor(int elevId);
    void handleHelpButton(int elevId);
    void on_handleHelpButton_clicked();
    void handleOverloadButton(int elevId);
    void handleReduceLoadButton(int elevId);
    void on_powerOutButton_clicked();

    void updateGUI();

private:
    Ui::MainWindow *ui;
    ElevatorControlSystem ecs; // Simulation system (5 floors, 2 elevators)
    QTimer* timer;             // Timer for periodic updates
    int activePassengerId;
    void initializeGrid();     // Set up grid labels
    QMutex mutex;

    void setupElevatorControls();
    void updateElevatorControls();
    void updateEmergencyStatusDisplay();
    void setupOverloadControls();
};

#endif // MAINWINDOW_H
