#define _USE_MATH_DEFINES

#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <cmath>

using namespace std;

#define MAX_FUEL 1000
#define DEFAULT_CONSUMPTION 0.5

class Location {
    private:
        float x, y, z;

    public:
        Location(const float& _x = 0, const float& _y = 0, const float& _z = 0) : x(_x), y(_y), z(_z) {}
        float getX() const { return x; }
        float getY() const { return y; }
        float getZ() const { return z; }
        void setX(const float& _x) { this->x = _x; }
        void setY(const float& _y) { this->y = _y; }
        void setZ(const float& _z) { this->z = _z; }
        void setLocation(const float& _x, const float& _y, const float& _z);

        float getThetaTo(const Location& l);
};

class Trajectory {
    private:
        vector<Location> points;
    public:
        Trajectory(const vector<Location>& _points) : points(_points) {}

        // Get a location in the trajetory at specified time
        Location getLocationAt(const size_t& t);
        // Get a new step to move from the specified location to the first trajectory location
        Location getLocationFrom(const Location& from);
};

class Plane {
    private:
        const string name;
        Location location, destination;
        Trajectory trajectory;
        float speed, fuel, consumption;
        bool reached_destination, reached_first_traj_point;

    public:
        Plane(const string& _name, const Location& _spawn, const Trajectory& _traj);
        bool isDestinationReached() const { return reached_destination; }

        void start();
        void updateLocation();
};
