#include "Elevator.h"


Elevator::Elevator(int id, int capacity, Door &door) : door(door), safetyControl(this) {
    connect(&door, &Door::bellRang, [this](const QString &msg) {
        displayMessage(msg.toStdString());
    });
    this->id = id;
    this->capacity = capacity;
    this->currentFloor = 1;
    this->state = "idle";
    this->isOverloaded = false;
    this->arrived = false;
    this->justStartedMoving = false;
    doorOpenSteps = 0;
}

int Elevator::getCurrentFloor() const {
    return currentFloor;
}

const string &Elevator::getState() const {
    return state;
}

const vector<Passenger> &Elevator::getPassengers() const {
    return passengers;
}

int Elevator::getId() const {
    return id;
}

vector<Passenger> &Elevator::getPassengers() {
    return passengers;
}

SafetyControl &Elevator::getSafetyControl() {
    return safetyControl;
}

void Elevator::openDoor() {
    door.open();
    doorOpenSteps = 0;
    QPointer<QTimer> closeTimer = new QTimer(this);
    closeTimer->setSingleShot(true);
    connect(closeTimer, &QTimer::timeout, this, [this, closeTimer]() {
        if (!door.isOpen()) return;
        closeDoor();
    });
    closeTimer->start(10000);
}

void Elevator::closeDoor() {
    door.close();
}

void Elevator::handleAutoClose() {
    if (door.isOpen() && !door.closeTimer->isActive()) {
        // First-time close attempt
        door.close();
    }
}

void Elevator::addRequests(int floor, bool fromInside) {
    // Check if the floor is already requested.
    queue<int> tempQueue = floorRequests;
    bool exists = false;
    while (!tempQueue.empty()) {
        if (tempQueue.front() == floor) {
            exists = true;
            break;
        }
        tempQueue.pop();
    }
    if (!exists) {
        floorRequests.push(floor);
        state = "moving";
        if (!fromInside)
            justStartedMoving = true;
        displayMessage("Received request for floor " + to_string(floor));
    }
}

void Elevator::addRequests(int floor) {
    addRequests(floor, false);
}

void Elevator::moveToFloor() {
    arrived = false;
    if (isOverloaded) {
        handleOverload();
        return;
    }
    // Check for a fire alarm.
    if (safetyControl.getActiveEvent() == "Fire") {
        if (floorRequests.empty())
            floorRequests.push(1);
        if (currentFloor > 1) {
            currentFloor--;
            state = "moving";
        } else if (currentFloor < 1) {
            currentFloor++;
            state = "moving";
        }
        if (currentFloor == 1) {
            openDoor();
            floorRequests.pop();
            arrived = true;
            state = "idle";
            safetyControl.setActiveEvent("None");
        }
        return;
    }
    if (floorRequests.empty()) {
        state = "idle";
        return;
    }
    if (justStartedMoving) {
        justStartedMoving = false;
        state = "moving";
        return;
    }
    // Normal movement
    int targetFloor = floorRequests.front();
    int step = (targetFloor > currentFloor) ? 1 : -1;
    currentFloor += step;
    if (currentFloor == targetFloor) {
        openDoor();
        floorRequests.pop();
        arrived = true;
        state = "idle";
    } else {
        state = "moving";
    }
}

void Elevator::triggerOverload() {
    if (!isOverloaded) {
        isOverloaded = true;
        displayMessage("OVERLOAD: Please reduce load!");
        stopMovement();
    }
}

void Elevator::resolveOverload() {
    if (isOverloaded) {
        isOverloaded = false;
        displayMessage("Load reduced - Operation resumed");
    }
}

bool Elevator::isInOverload() const {
    return isOverloaded;
}

void Elevator::handlePassengers(Floor &floor) {
    int arrivedFloor = getCurrentFloor();
    // Passengers exit.
    vector<int> passengersToRemove;
    for (const Passenger &passenger : passengers) {
        if (passenger.getDestinationFloor() == arrivedFloor) {
            passengersToRemove.push_back(passenger.getId());
            floor.getPassengers().push_back(passenger);
            string logMsg = "Passenger " + to_string(passenger.getId()) +
                    " exited Elevator " + to_string(id) +
                    " at Floor " + to_string(arrivedFloor);
            Logger::instance().appendLog(logMsg);
            displayMessage(logMsg);
        }
    }
    for (int id : passengersToRemove) {
        removePassenger(id);
    }
    // Passengers board.
    vector<int> passengersToAdd;
    for (Passenger &passenger : floor.getPassengers()) {
        if (!passenger.isInElevator() && passenger.getCurrentFloor() == arrivedFloor) {
            passengersToAdd.push_back(passenger.getId());
            addPassenger(passenger);
            passenger.enterElevator(arrivedFloor);
            string logMsg = "Passenger " + to_string(passenger.getId()) +
                    " entered Elevator " + to_string(id) +
                    " at Floor " + to_string(arrivedFloor);
            Logger::instance().appendLog(logMsg);
            displayMessage(logMsg);
        }
    }
    for (int id : passengersToAdd) {
        floor.removePassenger(id);
    }
    // If there are still passengers inside, keep the door open.
    if (!passengers.empty()) {
        openDoor();
    }
}


void Elevator::addPassenger(const Passenger &passenger) {
    if (isOverloaded) {
        displayMessage("Cannot board - Overload active");
        return;
    }

    passengers.push_back(passenger);

    if (passengers.size() >= static_cast<size_t>(capacity)) {
        triggerOverload();
    }
}


void Elevator::removePassenger(int passengerId) {
    auto it = find_if(passengers.begin(), passengers.end(), [passengerId](const Passenger &p) {
        return p.getId() == passengerId;
    });
    if (it != passengers.end()) {
        passengers.erase(it);
    }
}

void Elevator::displayMessage(const string &message) {
    cout << "Elevator " << id << ": " << message << endl;
}

void Elevator::pauseMovement() {
    state = "idle";
}

void Elevator::stopMovement() {
    state = "idle";
    floorRequests = queue<int>();
}

void Elevator::moveToSafeFloor(int safeFloor) {
    while (!floorRequests.empty()) floorRequests.pop();
    floorRequests.push(safeFloor);
    state = "moving";
}

void Elevator::pressHelpButton() {
    safetyControl.handleHelpRequest();
}

void Elevator::handleOverload() {
    if (safetyControl.getActiveEvent() == "Fire" ||
            safetyControl.getActiveEvent() == "PowerOut") {
        return; // Higher priority emergencies
    }
    triggerOverload();
}

bool Elevator::hasJustArrived() const {
    return arrived;
}

void Elevator::setState(string state) {
    this->state = state;
}


