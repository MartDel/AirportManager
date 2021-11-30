#include "APP.hpp"

/* -------------------------------------------------------------------------- */
/*                                Main function                               */
/* -------------------------------------------------------------------------- */

int main(void) {
    bool stop_prgm(false);

    /* ------------------------------ Init log file ----------------------------- */

    srand(time(NULL));
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
    RenderWindow app(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32), "Airport simulation", Style::Default, settings);
    app.setFramerateLimit(30);

    // Set up the background image
    Texture background_img;
    Sprite background_sprite;
    if (!background_img.loadFromFile(imgAirport("LIL"))) {
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

    // Create airports
    vector<APP*> airports;
    ifstream file_in(AIRPORTS_FILE);
    if (!file_in.is_open()) {
        cerr << "Cannot open file : " << AIRPORTS_FILE << endl;
        return -1;
    } else {
        json j;
        file_in >> j;
        for (auto& airport : j) {
            cout << "Generating the airport : " << airport["name"] << endl;
            airports.push_back(new APP(airport));
        }
    }

    // Create planes
    vector<Plane*> planes;
    for (auto& app : airports) {
        cout << app->getName() << endl;
        TWR* current_twr = app->getTWR();
        Plane* p;
        do {
            p = current_twr->spawnPlane();
            if (p != NULL) planes.push_back(p);
        } while (p != NULL);
    }

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
