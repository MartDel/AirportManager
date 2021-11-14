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

        // Get the angle between this point and the given point
        float getThetaTo(const Location& l) const;
        float getDistanceTo(const Location& l) const;

        // Operators
        bool operator==(const Location& l) const;
        friend ostream& operator<<(ostream& stream, const Location& l);
};

class Trajectory {
    private:
        vector<Location> points;
    public:
        Trajectory(const vector<Location>& _points) : points(_points) {}
        Location getPoint(const size_t& i) const { return points.at(i); }
        Location getLastPoint() const { return points.at(points.size() - 1); }

        // Get a location in the trajetory at specified time
        Location getLocationAt(const size_t& t) const;
        // Get a new step to move from the specified location to the first trajectory location
        Location getLocationFrom(const Location& from, const float& speed) const;
};

class Plane {
    private:
        const string name;
        Location location, destination;
        Trajectory trajectory;
        float speed, fuel, consumption;

    public:
        Plane(const string& _name, const Location& _spawn, const Trajectory& _traj);
        bool isDestinationReached() const { return this->location == this->destination; }

        void start();
        void updateLocation();
};
