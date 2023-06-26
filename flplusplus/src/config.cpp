#include "config.h"

#include "Common.h"

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
    conf.removestartlocationwarning = true;
    conf.logtoconsole = false;
}
void config::init_from_file(const char *filename)
{
    INI_Reader reader{};

    if (!reader.open(filename, false))
        return;

    while (reader.read_header())
    {
        if (!reader.is_header("flplusplus"))
            continue;

        while (reader.read_value())
        {
            if (reader.is_value("lod_scale"))
                conf.lodscale = reader.get_value_int(0);

            if (reader.is_value("save_folder_name"))
                conf.savefoldername = std::string(reader.get_value_string(0));

            if (reader.is_value("save_in_directory"))
                conf.saveindirectory = reader.get_value_bool(0);

            if (reader.is_value("remove_start_location_warning"))
                conf.removestartlocationwarning = reader.get_value_bool(0);
                
            if (reader.is_value("log_to_console"))
                conf.logtoconsole = reader.get_value_bool(0);
        }
    }

    reader.close();
}

void config::read_font_files(const char *filename) {
    INI_Reader reader{};

    if (!reader.open(filename, false))
        return;

    while (reader.read_header())
    {
        if (!reader.is_header("FontFiles"))
            continue;

        while (reader.read_value())
        {
            if (reader.is_value("path"))
                conf.fontfiles.emplace_back(reader.get_value_string(0));
        }
    }

    reader.close();
}
