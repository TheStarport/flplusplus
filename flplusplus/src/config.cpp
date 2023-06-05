#include "config.h"
#include "INIReader.h"
#include <cstring>

config::ConfigData conf;

config::ConfigData& config::get_config()
{
    return conf;
}

void config::init_defaults()
{
    conf.lodscale = 0;
    conf.savefoldername = "Freelancer";
    conf.saveindirectory = false;
}
void config::init_from_file(char *filename)
{
    INIReader reader(filename);
    if(reader.ParseError() != 0) {
        return;
    }
    conf.lodscale = reader.GetInteger("flplusplus", "lod_scale", 0);
    conf.savefoldername = strdup(reader.Get("flplusplus","save_folder_name", "Freelancer").c_str());
    conf.saveindirectory = reader.GetBoolean("flplusplus", "save_in_directory", false);
}
