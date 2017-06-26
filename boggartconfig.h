#ifndef BOGGARTCONF_H
#define BOGGARTCONF_H

#include <string>

// holds the config values read from the boggart config file

struct BoggartConfig{
    std::string host;
    std::string port;
    std::string host_name;
    std::string heart_beat_timeout; // how long we are willing to wait (in seconds) before we declare a worker as dead
};

#endif // BOGGARTCONF_H
