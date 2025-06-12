<<<<<<< HEAD
#include <iostream>
int main() { std::cout << "VR-DAW startet erfolgreich!" << std::endl; return 0; }
=======
#include "core/VRMusicStudioApp.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    VRMusicStudio::VRMusicStudioApp app;
    if (!app.initialize()) {
        std::cerr << "Fehler bei der Initialisierung der VRMusicStudioApp!" << std::endl;
        return 1;
    }
    app.run();
    app.shutdown();
    return 0;
} 
>>>>>>> 0dff1c4 (init 2)
