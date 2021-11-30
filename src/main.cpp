#include "APP.hpp"

/* -------------------------------------------------------------------------- */
/*                                Main function                               */
/* -------------------------------------------------------------------------- */

int main(void) {
    bool stop_prgm(false);

    /* ------------------------------ Init log file ----------------------------- */

    time_t now = time(0);
    char* dt = ctime(&now);
    ofstream file;
    file.open(LOGS_FILE,std::ofstream::out | std::ofstream::trunc);
    file << dt << "Roll the dice, the program has begun" << endl;
    file.close();

    /* ------------------------------- Init window ------------------------------ */

    // Set up the window
    ContextSettings settings;
    settings.antialiasingLevel = 8;
    RenderWindow app(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32), "TWR1", Style::Default, settings);
    app.setFramerateLimit(30);

    // Set up the background image
    Texture background_img;
    Sprite background_sprite;
    if (!background_img.loadFromFile(IMG_FOLDER + "airport1.jpg")) {
        cerr << "Cannot load image file : " << IMG_FOLDER << "airport1.jpg" << endl;
        return -1;
    }
    background_sprite.setTexture(background_img);

    // Set up the font
    Font font;
    if (!font.loadFromFile(FONTS_FOLDER + OPENSANS_FONT)) {
        cerr << "Cannot load font file : " << FONTS_FOLDER << OPENSANS_FONT << endl;
        return -1;
    }
    Plane::default_font = font;

    /* ------------------------ Init planes and airports ------------------------ */

    vector<APP*> airports;

    // Create first TWR
    // vector<Location> twr1_parking;
    // twr1_parking.push_back(Location(1440, 910, 0, 0));
    // twr1_parking.push_back(Location(1380, 890, 0, 0));
    // twr1_parking.push_back(Location(1330, 860, 0, 0));
    // Location l1(1000, 600, 0), l2(1800, 920, 0), l3(1405, 830, 0), l4(2900, 1300, 200, 250);
    // TWR twr1(twr1_parking,l1,l2,l3,l4);
    // twr1.setBackground(background_sprite);

    // // Create first APP
    // APP app1(&twr1, Location(100, 100, 1000, CIRCULAR_TRAJ_SPEED), Location(WINDOW_REAL_WIDTH/2, WINDOW_REAL_HEIGHT/2), 800);

    // // Create planes
    vector<Plane*> planes;
    // Plane* p1 = twr1.spawnPlane();
    // if (p1 == NULL) {
    //     cerr << "No more parking spot in Airport1..." << endl;
    //     return -1;
    // }
    // planes.push_back(p1);
    // Plane *p2 = twr1.spawnPlane();
    // if (p2 == NULL) {
    //     cerr << "No more parking spot in Airport1..." << endl;
    //     return -1;
    // }
    // planes.push_back(p2);
    // Plane *p3 = twr1.spawnPlane();
    // if (p3 == NULL) {
    //     cerr << "No more parking spot in Airport1..." << endl;
    //     return -1;
    // }
    // planes.push_back(p3);
    // planes.push_back(app1.spawnPlane());
    // planes.push_back(app1.spawnPlane());
    // planes.push_back(app1.spawnPlane());

    /* ------------------------------ Init threads ------------------------------ */

    // Create and start threads
    for (auto& airport : airports)
        airport->setThread(ref(stop_prgm));
    thread world_thread(Plane::world, ref(planes), ref(stop_prgm));

    /* -------------------------------- SFML loop ------------------------------- */

    while (app.isOpen()) {
        // Events
        Event event;
        while (app.pollEvent(event)) {
            if ((event.type == Event::KeyPressed && event.key.code == Keyboard::Escape) || event.type == Event::Closed)
                app.close();
        }

        // Stop the programme
        if(stop_prgm) app.close();
        
        app.clear();
        app.draw(background_sprite);

        for (auto& plane : planes) {
            app.draw(plane->toSFML());
            app.draw(plane->getAltitudeLabel());
            app.draw(plane->getNameLabel());
        }

        app.display();
    }

    /* ------------------------ Shut down the simulation ------------------------ */

    stop_prgm = true;
    if(world_thread.joinable()) world_thread.join();
    for (auto& plane : planes) delete plane;

    return 0;
}
