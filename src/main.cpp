#include "TWR.hpp"

// Thread timeouts (in ms)
#define WORLD_INTERVAL 1000
#define AIRPORTS_INTERVAL 250

mutex cout_lock; // A thread lock

/* -------------------------------------------------------------------------- */
/*                              Thread functions                              */
/* -------------------------------------------------------------------------- */

/**
 * @brief The world thread function.
 * Manage plane locations each second.
 * @param planes Planes to manage
 * @param stop_prgm If the simulation must stoped
 */
void world(vector<Plane*>& planes, bool& stop_prgm) {
    chrono::milliseconds interval(WORLD_INTERVAL);

    while (!stop_prgm) {
        for (auto& current_plane : planes) {
            if (!current_plane->isDestinationReached())
                // Update plane location
                current_plane->updateLocation();
            else {
                // Update plane state
                switch (current_plane->getState()) {
                    case 1:
                        // The plane has token off
                        stop_prgm = true;
                        current_plane->setState(2);
                        break;
                }
            }

            // Debug plane data
            cout_lock.lock();
            cout << *current_plane << endl;
            cout_lock.unlock();
        }

        // Add 1s timeout
        this_thread::sleep_for(interval);
    }

    cout_lock.lock();
    cout << "World stop" << endl;
    cout_lock.unlock();
}

/**
 * @brief Airport thread function.
 * Check if a plane can take off or land.
 * @param twr The TWR to manage
 * @param stop_prgm If the simulation must stoped
 */
void twr_control(TWR& twr, bool& stop_prgm) {
    chrono::milliseconds interval(AIRPORTS_INTERVAL);

    while (!stop_prgm) {
        if (!twr.isRunwayUsed() && !twr.isParkingEmpty()) {
            // Take off a plane
            cout_lock.lock();
            twr.takeOffPlane();
            cout_lock.unlock();
        }

        // Add 0.25s timeout
        this_thread::sleep_for(interval);
    }
    
    cout_lock.lock();
    cout << "Airport stop" << endl;
    cout_lock.unlock();
}

/* -------------------------------------------------------------------------- */
/*                                Main function                               */
/* -------------------------------------------------------------------------- */

int main(void) {
    bool stop_prgm(false);

    // Create first TWR
    vector<Location> twr1_parking;
    for (size_t i = 0; i < NB_PARKING_SPOTS; i++)
        twr1_parking.push_back(Location(i * 10, 0, 0, 0));
    TWR twr1(
        twr1_parking,
        Location(-20, 10, 0, 5),
        Location(60, 70, 0, 100),
        Location(0, 10, 0, 5),
        Location(300, 300, 200, 250)
    );

    // Create planes
    vector<Plane*> planes;
    Plane* p = twr1.spawnPlane("test1");
    if (p == NULL) {
        cerr << "No more parking spot in Airport1..." << endl;
        return -1;
    }
    planes.push_back(p);

    // Create and start threads
    // thread world_thread(world, ref(planes), ref(stop_prgm));
    // thread twr_thread(twr_control, ref(twr1), ref(stop_prgm));

    // Check if the simulation should stop
    // char input;
    // while (!stop_prgm) {
    //     cin >> input;
    //     if (input == 'q')
    //         stop_prgm = true;
    // }

    // Set up the window
    ContextSettings settings;
    settings.antialiasingLevel = 8;
    RenderWindow app(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32), "TWR1", Style::Default, settings);
    app.setFramerateLimit(1);

    // Set up the background image
    Texture background_img;
    Sprite background_sprite;
    if (!background_img.loadFromFile(IMG_FOLDER + BACKGROUND_IMG)) {
        cerr << "Cannot load image file : " << IMG_FOLDER << BACKGROUND_IMG << endl;
        return -1;
    }
    background_sprite.setTexture(background_img);

    // Set up the circle
    Vector2f plane_pos(700, 100);
    CircleShape circle(10.f);
    circle.setFillColor(Color::Blue);
    circle.setPosition(plane_pos);

    // Set up a text
    Font font;
    if (!font.loadFromFile(FONTS_FOLDER + OPENSANS_FONT)) {
        cerr << "Cannot load font file : " << FONTS_FOLDER << OPENSANS_FONT << endl;
        return -1;
    }
    Text altitude;
    altitude.setFont(font);
    altitude.setString("500m");
    altitude.setCharacterSize(10);
    altitude.setFillColor(Color::Black);
    altitude.setPosition(Vector2f(plane_pos.x + 25, plane_pos.y - 10));

    while (app.isOpen()) {
        // Events
        Event event;
        while (app.pollEvent(event)) {
            if ((event.type == Event::KeyPressed && event.key.code == Keyboard::Escape) || event.type == Event::Closed)
                app.close();
        }

        // Display
        app.clear();
        app.draw(background_sprite);
        app.draw(circle);
        app.draw(altitude);
        app.display();
    }

    // Shut down threads
    stop_prgm = true;
    // if(world_thread.joinable()) world_thread.join();
    // if(twr_thread.joinable()) twr_thread.join();

    return 0;
}
