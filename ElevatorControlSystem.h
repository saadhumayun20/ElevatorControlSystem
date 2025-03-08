#ifndef ELEVATORCONTROLSYSTEM_H
#define ELEVATORCONTROLSYSTEM_H

#include "Elevator.h"
#include "Floor.h"
#include <vector>
#include <memory>
#include <map>
#include <iostream>
#include <climits>
#include <algorithm>

using namespace std;

struct EmergencyStatus {
    bool helpRequested;
    int timeElapsed;
    int elevatorId;
};

class ElevatorControlSystem {
public:
    ElevatorControlSystem(int numFloors, int numElevators);
    void startSimulation();
    void pauseSimulation();
    void resumeSimulation();
    void stopSimulation();
    void addRequest(int floor, const string &direction);
    void assignElevator(Floor &floor);
    void advanceTimeStep();
    void triggerFireAlarm();
    void handleHelpResponse(int elevId);
    const vector<shared_ptr<Elevator>> &getElevators() const;
    const vector<Floor> &getFloors() const;
    int getCurrentTimeStep() const;
    bool isSimulationRunning() const;

    void handleOverload(int elevId);
    void resolveOverload(int elevId);
    void triggerPowerOut();

    map<int, EmergencyStatus> emergencyStates;

private:
    int numFloors;
    int numElevators;
    bool isRunning;
    int currentTimeStep;
    vector<Floor> floors;
    vector<shared_ptr<Elevator>> elevators;
    vector<unique_ptr<Door>> doors;
};

#endif // ELEVATORCONTROLSYSTEM_H
