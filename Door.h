#ifndef DOOR_H
#define DOOR_H

#include <QObject>
#include <QMutex>
#include <QString>
#include <QRandomGenerator>
#include <QTimer>

class Door : public QObject {
    Q_OBJECT
public:
    Door(QObject *parent = nullptr);
    bool isOpen() const;
    void open();
    void close();
    void cancelAutoClose();
    bool detectObstacle();

    QTimer *closeTimer;

signals:
    void bellRang(const QString &message);

private slots:
      void attemptClose();

private:
    bool openStatus;
    int openDuration;
    QMutex doorMutex;
    int obstacleCount = 0;
};

#endif // DOOR_H
