#include "ElevatorControlSystem.h"

ElevatorControlSystem::ElevatorControlSystem(int numFloors, int numElevators)
    : numFloors(numFloors), numElevators(numElevators), isRunning(false), currentTimeStep(0) {
    for (int i = 1; i <= numFloors; i++) {
        floors.emplace_back(i);
    }
    for (int i = 1; i <= numElevators; i++) {
        doors.push_back(make_unique<Door>());
        elevators.emplace_back(make_shared<Elevator>(i, 10, *doors.back()));
    }
}

void ElevatorControlSystem::startSimulation() {
    isRunning = true;
    currentTimeStep = 0;
}

void ElevatorControlSystem::pauseSimulation() {
    isRunning = false;
}

void ElevatorControlSystem::resumeSimulation() {
    isRunning = true;
}

void ElevatorControlSystem::stopSimulation() {
    isRunning = false;
    currentTimeStep = 0;
}

void ElevatorControlSystem::addRequest(int floor, const string &direction) {
    auto it = find_if(floors.begin(), floors.end(), [floor](const Floor &f) { return f.getFloorNumber() == floor; });
    if (it != floors.end()) {
        if (direction == "up") {
            it->pressUpButton();
        } else if (direction == "down") {
            it->pressDownButton();
        }
        assignElevator(*it);
    }
}

void ElevatorControlSystem::assignElevator(Floor &floor) {
    if (floor.isCallAssigned()) {
        return;
    }
    shared_ptr<Elevator> bestElevator = nullptr;
    int minDistance = INT_MAX;
    for (const auto &elevator : elevators) {
        int distance = abs(elevator->getCurrentFloor() - floor.getFloorNumber());
        if (distance < minDistance || (distance == minDistance && elevator->getState() == "idle")) {
            minDistance = distance;
            bestElevator = elevator;
        }
    }
    if (bestElevator) {
        bestElevator->addRequests(floor.getFloorNumber());
        floor.setCallAssigned(true);
    }
}

void ElevatorControlSystem::advanceTimeStep() {
    if (!isRunning) return;
    currentTimeStep++;

    // Process emergency.
    for (auto &pair : emergencyStates) {
        int elevId = pair.first;
        auto &status = pair.second;
        if (status.helpRequested) {
            status.timeElapsed++;
            if (status.timeElapsed >= 5) {
                cout << "NO RESPONSE - Calling 911 for Elevator " << elevId << endl;
                emergencyStates[elevId] = {};  // Reset state.
            }
        }
    }

    for (const auto &elevator : elevators) {
        elevator->moveToFloor();
        if (elevator->hasJustArrived()) {
            int arrivedFloor = elevator->getCurrentFloor();
            Floor &floor = floors[arrivedFloor - 1];
            elevator->handlePassengers(floor);
            floor.resetButtons();
        }
    }

    for (auto &floor : floors) {
        if (floor.isUpButtonPressed() || floor.isDownButtonPressed()) {
            assignElevator(floor);
        }
    }
    cout << "Time Step: " << currentTimeStep << endl;
}

void ElevatorControlSystem::triggerFireAlarm() {
    for (const auto &elevator : elevators) {
        elevator->getSafetyControl().handleFireAlarm();
    }
    for (auto &floor : floors) {
        floor.resetButtons();
    }
}

void ElevatorControlSystem::handleOverload(int elevId) {
    if (elevId > 0 && static_cast<size_t>(elevId - 1) < elevators.size()) {
        elevators[elevId-1]->triggerOverload();
    }
}

void ElevatorControlSystem::resolveOverload(int elevId) {
    if (elevId > 0 && static_cast<size_t>(elevId - 1) < elevators.size()) {
        elevators[elevId-1]->resolveOverload();
    }
}

void ElevatorControlSystem::triggerPowerOut() {
    for (auto& elevator : elevators) {
        elevator->getSafetyControl().handlePowerOut();
    }
}

void ElevatorControlSystem::handleHelpResponse(int elevId) {
    if (emergencyStates.count(elevId) && emergencyStates[elevId].helpRequested) {
        emergencyStates[elevId] = {};
        cout << "Response received - Problem solved in Elevator " << elevId << endl;
    }
}

const vector<shared_ptr<Elevator>> &ElevatorControlSystem::getElevators() const {
    return elevators;
}

const vector<Floor> &ElevatorControlSystem::getFloors() const {
    return floors;
}

int ElevatorControlSystem::getCurrentTimeStep() const {
    return currentTimeStep;
}

bool ElevatorControlSystem::isSimulationRunning() const {
    return isRunning;
}
