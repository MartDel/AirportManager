#include "CCR.hpp"

/* -------------------------------------------------------------------------- */
/*                                Main function                               */
/* -------------------------------------------------------------------------- */

int main() {
    bool stop_prgm(false);

    /* ------------------------------ Init log file ----------------------------- */

    time_t now = time(NULL);
    srand(0);
    char* dt = ctime(&now);
    ofstream file;
    file.open(LOGS_FILE,std::ofstream::out | std::ofstream::trunc);
    file << dt << "Roll the dice, the program has begun" << endl;
    file.close();

    /* ------------------------------- Init window ------------------------------ */

    // Set up the window
    ContextSettings settings;
    settings.antialiasingLevel = 8.0;
    RenderWindow app(VideoMode(WINDOW_WIDTH*2 + 50, WINDOW_HEIGHT*2 + 50, 32), "Airport simulation", Style::Default, settings);
    app.setFramerateLimit(30);

    // Load the font
    Font font;
    if (!font.loadFromFile(FONTS_FOLDER + OPENSANS_FONT)) {
        cerr << "Cannot load font file : " << FONTS_FOLDER << OPENSANS_FONT << endl;
        return -1;
    }
    Plane::default_font = font;

    // Load the plane img
    Texture plane_img;
    string plane_path = IMG_FOLDER + "plane.png";
    if (!plane_img.loadFromFile(plane_path)) {
        cerr << "Cannot load image file : " << plane_path << endl;
        updateLogs("Cannot load image file : " + plane_path);
        return -1;
    }
    Plane::plane_texture = plane_img;

    /* ------------------------ Init planes and airports ------------------------ */

    // Create CCR
    CCR ccr;
    vector<APP*> airports = ccr.getAirports();

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
    ccr.setThread(ref(stop_prgm));
    for (auto& airport : airports)
        airport->setThread(ref(stop_prgm));

    #ifndef DEBUG
    thread world_thread(Plane::world, ref(planes), ref(stop_prgm));
    #endif

    /* ---------------------------- Load backgrounds ---------------------------- */

    // Load CCR image
    Texture CCR_img;
    Sprite CCR_sprite;
    string img_name = imgAirport("CCR");
    if (!CCR_img.loadFromFile(img_name)) {
        cerr << "Cannot load image file : " << img_name << endl;
        updateLogs("Cannot load image file : " + img_name);
        return -1;
    }
    CCR_sprite.setTexture(CCR_img);
    CCR_sprite.setPosition(Vector2f(0, 0));

    // Load the airport images
    vector<Texture*> background_textures;
    for (auto& airport : airports) {
        if (airport->getReferenceFrame() == ReferenceFrame::NOT_PRINTED) continue;
        Texture* background_img = new Texture();
        Sprite* background_sprite = new Sprite();
        string img_name = imgAirport(airport->getTrigramme());
        if (!background_img->loadFromFile(img_name)) {
            cerr << "Cannot load image file : " << img_name << endl;
            updateLogs("Cannot load image file : " + img_name);
            return -1;
        }
        background_sprite->setTexture(*background_img);
        background_textures.push_back(background_img);
        airport->setBackground(background_sprite);
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

        // Draw CCR
        app.draw(CCR_sprite);
        for (auto& airport : airports) {
            app.draw(airport->getGlobalPoint());
        }
        
        // Draw world->airports
        for (auto& airport : airports) {
            if (airport->getReferenceFrame() == ReferenceFrame::NOT_PRINTED) continue;
            Sprite *to_draw = airport->getBackground();
            Vector2f pos = getFrameStartPoint(airport->getReferenceFrame());
            to_draw->setPosition(pos);
            app.draw(*to_draw);
        }
        
        #ifndef DEBUG

        // Draw planes
        for (auto& plane : planes) {
            if (plane->getLocation().getRefFrame() == ReferenceFrame::NOT_PRINTED) continue;
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

    for (auto &bg : background_textures) delete bg;

    return 0;
}
