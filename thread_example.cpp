#include <thread>
#include <mutex>
#include <iostream>
#include <vector>
#include <queue>
#include <random>
#include <string>
#include <optional>

using namespace std;

std::mutex airport_mutex;
std::mutex waiting_planes_mutex;

struct Plane
{
	string identification;
};

class Airport
{
	vector<Plane> planes;

public:
	void add_a_plane(Plane &plane)
	{
		airport_mutex.lock();
		planes.push_back(plane);
		airport_mutex.unlock();
	}
	friend ostream &operator<<(ostream &os, const Airport &airport);
};

ostream &operator<<(ostream &os, const Airport &airport)
{
	os << "Available planes\n";
	for (auto &plane : airport.planes)
	{
		os << plane.identification << endl;
	}
	return os;
}

class Waiting_planes
{
	queue<Plane> planes;

public:
	void add_a_plane(Plane &plane)
	{
		waiting_planes_mutex.lock();
		planes.push(plane);
		waiting_planes_mutex.unlock();
	}
	bool is_a_plane_available()
	{
		return !(planes.empty());
	}
	optional<Plane> get_a_plane()
	{
		if (is_a_plane_available())
		{
			waiting_planes_mutex.lock();
			auto plane = planes.front();
			planes.pop();
			waiting_planes_mutex.unlock();
			return plane;
		}
		return nullopt;
	}
};

void add_plane_sometimes(Waiting_planes &waiting_planes, bool &stop_thread)
{
	default_random_engine generator;
	uniform_int_distribution<int> distribution(100, 800);
	while (!stop_thread)
	{
		std::this_thread::sleep_for(3s);
		Plane plane;
		plane.identification = "AF" + to_string(distribution(generator));
		waiting_planes.add_a_plane(plane);
	}
}

void airport_control(Airport &airport, Waiting_planes &waiting_planes, bool &stop_thread)
{
	while (!stop_thread)
	{
		while (waiting_planes.is_a_plane_available())
		{
			if (auto plane = waiting_planes.get_a_plane())
			{
				cout << "Add a plane " << plane.value().identification << endl;
				airport.add_a_plane(*plane);
			}
		}
	}
}

int main()
{
	//	vector<Plane> planes;
	Airport airport;
	Waiting_planes waiting_planes;
	bool stop_thread = false;

	std::thread airport_thread(airport_control, std::ref(airport), std::ref(waiting_planes), std::ref(stop_thread));
	std::thread add_plane(add_plane_sometimes, std::ref(waiting_planes), ref(stop_thread));

	while (1)
	{
		string name;
		cin >> name;
		if (name == "0")
		{
			stop_thread = true;
			break;
		}
		Plane plane;
		plane.identification = name;

		waiting_planes.add_a_plane(plane);
	}

	if (airport_thread.joinable())
	{
		airport_thread.join();
	}

	if (add_plane.joinable())
	{
		add_plane.join();
	}

	cout<<airport;

	std::cout << "Finish\n";

	return 0;
}
