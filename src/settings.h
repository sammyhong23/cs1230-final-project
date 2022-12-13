#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

struct Settings {
    std::string sceneFilePath;
    int shapeParameter1 = 1;
    int shapeParameter2 = 1;
    float nearPlane = 0.1;
    float farPlane = 50;
    bool bezier = false;
    bool parallax = false;
    bool worley = false;
    bool flow = false;
    float texGenParam1 = 1;
    float texGenParam2 = 1;
    float flowspeed = 0;
};


// The global Settings object, will be initialized by MainWindow
extern Settings settings;

#endif // SETTINGS_H
