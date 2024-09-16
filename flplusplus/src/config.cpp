#include "config.h"

#include "Common.h"

config::ConfigData conf;

config::ConfigData& config::get_config()
{
    return conf;
}

void config::init_defaults()
{
    conf.lodscale = 1;
    conf.pbubblescale = 1;
    conf.characterdetailscale = 1;
    conf.savefoldername = "Freelancer";
    conf.saveindirectory = false;
    conf.screenshotsfoldername = "FreelancerShots";
    conf.screenshotsindirectory = false;
    conf.removestartlocationwarning = true;
    conf.logtoconsole = false;
    conf.shippreviewscrollingspeed = 2;
    conf.shippreviewscrollinginverse = false;
    conf.shippreviewscrollingmindistance = 0.0f;
    conf.shippreviewscrollingmaxdistance = 100.0f;
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
                conf.lodscale = reader.get_value_float(0);

            if (reader.is_value("pbubble_scale"))
                conf.pbubblescale = reader.get_value_float(0);

            if (reader.is_value("character_detail_scale"))
                conf.characterdetailscale = reader.get_value_float(0);

            if (reader.is_value("save_folder_name"))
                conf.savefoldername = std::string(reader.get_value_string(0));

            if (reader.is_value("save_in_directory"))
                conf.saveindirectory = reader.get_value_bool(0);

            if (reader.is_value("screenshots_folder_name"))
                conf.screenshotsfoldername = std::string(reader.get_value_string(0));

            if (reader.is_value("screenshots_in_directory"))
                conf.screenshotsindirectory = reader.get_value_bool(0);

            if (reader.is_value("remove_start_location_warning"))
                conf.removestartlocationwarning = reader.get_value_bool(0);

            if (reader.is_value("log_to_console"))
                conf.logtoconsole = reader.get_value_bool(0);

            if (reader.is_value("ship_preview_scrolling_speed"))
                conf.shippreviewscrollingspeed = reader.get_value_float(0);

            if (reader.is_value("ship_preview_scrolling_inverse"))
                conf.shippreviewscrollinginverse = reader.get_value_bool(0);

            if (reader.is_value("ship_preview_scrolling_min_distance"))
                conf.shippreviewscrollingmindistance = reader.get_value_float(0);

            if (reader.is_value("ship_preview_scrolling_max_distance"))
                conf.shippreviewscrollingmaxdistance = reader.get_value_float(0);
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
