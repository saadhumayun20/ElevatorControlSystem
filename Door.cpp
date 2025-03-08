#include "Door.h"
#include <QTimer>
#include <ctime>
#include <cstdlib>

Door::Door(QObject *parent) : QObject(parent), openStatus(false), openDuration(0) {
    closeTimer = new QTimer(this);
    closeTimer->setSingleShot(true);
    connect(closeTimer, &QTimer::timeout, this, &Door::attemptClose);
}

bool Door::isOpen() const {
    return openStatus;
}

void Door::open() {
    QMutexLocker locker(&doorMutex);
    if (openStatus) return;
    openStatus = true;
    emit bellRang("Bell ringing - Doors opening");
}

void Door::close() {
    if (!openStatus) return;

    if (detectObstacle()) {
        obstacleCount++;
        emit bellRang(QString("Obstacle detected (%1/3)").arg(obstacleCount));

        if (obstacleCount >= 3) {
            emit bellRang("Permanent obstacle - Door staying open");
            cancelAutoClose();
            return;
        }

        // Retry after 2 seconds
        closeTimer->start(2000);
        return;
    }

    openStatus = false;
    obstacleCount = 0;
    emit bellRang("Doors closing");
}

void Door::attemptClose() {
    close();
}

void Door::cancelAutoClose() {
    closeTimer->stop();
    obstacleCount = 0;
}

bool Door::detectObstacle() {
    return (QRandomGenerator::global()->bounded(10)) == 0;
}

