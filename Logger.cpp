#include "Logger.h"

Logger &Logger::instance() {
    static Logger instance;
    return instance;
}

void Logger::appendLog(const string &message) {
    lock_guard<mutex> lock(mtx);
    logs.push_back(message);
}

vector<string> Logger::retrieveLogs() {
    lock_guard<mutex> lock(mtx);
    vector<string> ret = logs;
    logs.clear();
    return ret;
}
