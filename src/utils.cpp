#include "Airport.hpp"

Vector2f getFrameStartPoint(const ReferenceFrame& ref) {
    Vector2f start_point;

    switch (ref) {
        case ReferenceFrame::CCR :
            start_point.x = 0;
            start_point.y = 0;
            break;
        case ReferenceFrame::APP1:
            start_point.x = WINDOW_WIDTH;
            start_point.y = 0;
            break;
        case ReferenceFrame::APP2:
            start_point.x = 0;
            start_point.y = WINDOW_HEIGHT;
            break;
        case ReferenceFrame::APP3:
            start_point.x = WINDOW_WIDTH;
            start_point.y = WINDOW_HEIGHT;
            break;
    }

    return start_point;
}

ReferenceFrame getFrameFromId(const size_t &id) {
    ReferenceFrame f = ReferenceFrame::APP1;

    switch (id) {
        case 1: f = ReferenceFrame::APP2; break;
        case 2: f = ReferenceFrame::APP3; break;
    }

    return f;
}

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

string imgAirport(const string &trigramme) {
    return IMG_FOLDER + trigramme + IMG_EXTENSION;
}

CircleShape generateDebugCircle(const Vector2f& pos) {
    CircleShape circle(5.f);
    circle.setFillColor(Color::Red);
    circle.setPosition(pos);
    return circle;
}