// #include "APP.hpp"

// // Thread timeouts (in ms)
// #define WORLD_INTERVAL 1000
// #define AIRPORTS_INTERVAL 250

// mutex cout_lock; // A thread lock

// /* -------------------------------------------------------------------------- */
// /*                              Thread functions                              */
// /* -------------------------------------------------------------------------- */

// /**
//  * @brief The world thread function.
//  * Manage plane locations each second.
//  * @param planes Planes to manage
//  * @param stop_prgm If the simulation must stoped
//  */
// void world(vector<Plane*>& planes, bool& stop_prgm) {
//     chrono::milliseconds interval(WORLD_INTERVAL);

//     while (!stop_prgm) {
//         for (auto& current_plane : planes) {
//             if (!current_plane->isDestinationReached()) {
//                 // Update plane location
//                 current_plane->updateLocation();
//             }

//             // Debug plane data
//             cout_lock.lock();
//             cout << *current_plane << endl;
//             cout_lock.unlock();
//         }

//         // Add 1s timeout
//         this_thread::sleep_for(interval);
//     }

//     cout_lock.lock();
//     cout << "World stop" << endl;
//     cout_lock.unlock();
// }

// /**
//  * @brief Airport thread function.
//  * Check if a plane can take off or land.
//  * @param twr The TWR to manage
//  * @param stop_prgm If the simulation must stoped
//  */
// void airport_control(APP& app, bool& stop_prgm) {
//     chrono::milliseconds interval(AIRPORTS_INTERVAL);
//     TWR* twr = app.getTWR();

//     while (!stop_prgm) {
//         // Check arrived planes
//         vector<Plane*> arrived_planes = app.getArrivedPlanes();
//         for (auto& arrived_plane : arrived_planes) {
//             arrived_plane->start(app.getCircularTrajectory());
//         }

//         if (!twr->isRunwayUsed() && !twr->isParkingEmpty()) {
//             // Take off a plane
//             cout_lock.lock();
//             twr->takeOffPlane();
//             cout_lock.unlock();
//         }

//         // Add 0.25s timeout
//         this_thread::sleep_for(interval);
//     }
    
//     cout_lock.lock();
//     cout << "Airport stop" << endl;
//     cout_lock.unlock();
// }

// /* -------------------------------------------------------------------------- */
// /*                                Main function                               */
// /* -------------------------------------------------------------------------- */

// int main(void) {

//     bool stop_prgm(false);

//     /* ------------------------------- Init window ------------------------------ */

//     // Set up the window
//     ContextSettings settings;
//     settings.antialiasingLevel = 8;
//     RenderWindow app(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32), "TWR1", Style::Default, settings);
//     app.setFramerateLimit(1);

//     // Set up the background image
//     Texture background_img;
//     Sprite background_sprite;
//     if (!background_img.loadFromFile(IMG_FOLDER + BACKGROUND_IMG))
//     {
//         cerr << "Cannot load image file : " << IMG_FOLDER << BACKGROUND_IMG << endl;
//         return -1;
//     }
//     background_sprite.setTexture(background_img);

//     // Set up the font
//     Font font;
//     if (!font.loadFromFile(FONTS_FOLDER + OPENSANS_FONT)) {
//         cerr << "Cannot load font file : " << FONTS_FOLDER << OPENSANS_FONT << endl;
//         return -1;
//     }
//     Plane::default_font = font;

//     /* ------------------------ Init planes and airports ------------------------ */

//     // Create first TWR
//     vector<Location> twr1_parking;
//     twr1_parking.push_back(Location(1440, 910, 0, 0));
//     twr1_parking.push_back(Location(1380, 890, 0, 0));
//     twr1_parking.push_back(Location(1330, 860, 0, 0));
//     TWR twr1(
//         twr1_parking,
//         Location(1000, 600, 0, 50),
//         Location(1800, 920, 0, 100),
//         Location(1405, 830, 0, 30),
//         Location(2900, 1300, 200, 250)
//     );
//     twr1.setBackground(background_sprite);

//     // Create first APP
//     APP app1(&twr1, Location(100, 100, 1000, CIRCULAR_TRAJ_SPEED), Location(WINDOW_REAL_WIDTH/2, WINDOW_REAL_HEIGHT/2));

//     // Create planes
//     vector<Plane*> planes;
//     Plane* p = twr1.spawnPlane("blabla");
//     if (p == NULL) {
//         cerr << "No more parking spot in Airport1..." << endl;
//         return -1;
//     }
//     planes.push_back(p);
//     planes.push_back(app1.spawnPlane("bleble"));

//     /* ------------------------- Threads and window loop ------------------------ */

//     // Create and start threads
//     thread airport_thread(airport_control, ref(app1), ref(stop_prgm));
//     thread world_thread(world, ref(planes), ref(stop_prgm));

//     while (app.isOpen()) {
//         // Events
//         Event event;
//         while (app.pollEvent(event)) {
//             if ((event.type == Event::KeyPressed && event.key.code == Keyboard::Escape) || event.type == Event::Closed)
//                 app.close();
//         }

//         // Stop the programme
//         if(stop_prgm) app.close();
        
//         app.clear();
//         app.draw(twr1.getBackground());

//         for (auto& plane : planes) {
//             app.draw(plane->toSFML());
//             app.draw(plane->getAltitudeLabel());
//             app.draw(plane->getNameLabel());
//         }

//         app.display();
//     }

//     // Shut down threads
//     stop_prgm = true;
//     if(world_thread.joinable()) world_thread.join();
//     if(airport_thread.joinable()) airport_thread.join();
//     for (auto& plane : planes) delete plane;

//     return 0;
// }
