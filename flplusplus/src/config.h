#pragma once

#include <iostream>

namespace config {
    void init_defaults();
    void init_from_file(const char *filename);
    class ConfigData
    {
    public:
        unsigned int lodscale;
        std::string savefoldername;
        bool saveindirectory;
        bool removestartlocationwarning;
    };
    ConfigData& get_config();
}
