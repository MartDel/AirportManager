#include "Airport.hpp"

float roundWithPrecision(const float &f) {
    float value = int(f * 100 + .5);
    return float(value / 100);
}

float mapValue(const float &x, const float &in_min, const float &in_max, const float &out_min, const float &out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void updateLogs(const string &logs){
    time_t now = time(0);
    char* dt = ctime(&now);
    ofstream file;
    file.open(LOGS_FILE, ios::app);
    file << dt << logs << endl;
    file.close();
    return;
}