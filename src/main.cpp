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
    RenderWindow app(VideoMode(WINDOW_WIDTH*2, WINDOW_HEIGHT*2, 32), "Airport simulation", Style::Default, settings);
    app.setFramerateLimit(30);

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
        for (size_t i = 0; i < j.size(); i++) {
            cout << "Generating the airport : " << j[i]["name"] << endl;
            airports.push_back(new APP(j[i], getFrameFromId(i)));
        }
    }

    #ifndef DEBUG
    // Create planes
    vector<Plane*> planes;
    for (auto& app : airports) {
        Plane* p;
        do {
            p = app->spawnPlane();
            if (p != NULL) planes.push_back(p);
        } while (p != NULL);
    }
    #endif

    /* ------------------------------ Init threads ------------------------------ */

    // Create and start threads
    for (auto& airport : airports)
        airport->setThread(ref(stop_prgm));

    #ifndef DEBUG
    thread world_thread(Plane::world, ref(planes), ref(stop_prgm));
    #endif

    /* ---------------------------- Load backgrounds ---------------------------- */

    // Set up the background image
    map<string, Sprite*> backgrounds;
    vector<Texture*> background_textures;
    for (auto& airport : airports) {
        Texture* background_img = new Texture();
        Sprite* background_sprite = new Sprite();
        string img_name = imgAirport(airport->getTrigramme());
        if (!background_img->loadFromFile(img_name)) {
            cerr << "Cannot load image file : " << img_name << endl;
            return -1;
        }
        background_sprite->setTexture(*background_img);
        background_textures.push_back(background_img);
        backgrounds.insert(pair<string, Sprite*>(airport->getName(), background_sprite));
    }

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
        
        // Draw backgrounds
        size_t i(0);
        for (auto& background : backgrounds) {
            Sprite* to_draw = background.second;
            Vector2f pos((i == 1 ? 0 : 1) * WINDOW_WIDTH, (i > 0 ? 1 : 0) * WINDOW_HEIGHT);
            to_draw->setPosition(pos);
            app.draw(*to_draw);
            i++;
        }
        
        #ifndef DEBUG

        // Draw planes
        for (auto& plane : planes) {
            app.draw(plane->toSFML());
            app.draw(plane->getAltitudeLabel());
            app.draw(plane->getNameLabel());
        }

        #else

        // Draw important points
        for (auto& airport : airports) {
            for (const auto& p : airport->getImportantPoints()) {
                Vector2f pos = p.toVector();
                app.draw(generateDebugCircle(pos));
            }
        }

        #endif

        app.display();
    }

    /* ------------------------ Shut down the simulation ------------------------ */

    stop_prgm = true;

    #ifndef DEBUG
    if(world_thread.joinable()) world_thread.join();
    for (auto& plane : planes) delete plane;
    #endif

    for (auto& bg : backgrounds) delete bg.second;
    for (auto &bg : background_textures) delete bg;
    for (auto& airport : airports) delete airport;

    return 0;
}
