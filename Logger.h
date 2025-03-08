#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <vector>
#include <mutex>
using namespace std;

class Logger {
public:
    static Logger &instance();
    void appendLog(const string &message);
    vector<string> retrieveLogs();
private:
    Logger() {}
    vector<string> logs;
    mutex mtx;
};

#endif // LOGGER_H
