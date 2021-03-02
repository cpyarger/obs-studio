#include "dashboard-widget.hpp"

#include <obs-data.h>
#include <obs-module.h>
#include <obs.h>
#include <string>
#include "obs-frontend-api.h"
#include "switch.hpp"
#include "util/config-file.h"
#include "../../obs-app.hpp"

void change_advanced(Json &parsed)
{
    char encoderJsonPath[512];
    int ret = GetProfilePath( encoderJsonPath, sizeof(encoderJsonPath), "streamEncoder.json");
    if (ret > 0) {
        obs_data_t *data = obs_data_create_from_json_file_safe(encoderJsonPath, "bak");

        auto op = parsed["output"];

        int bitrate = std::stoi(op["bitrate"].string_value());

        obs_data_set_int(data, "bitrate", bitrate);
        obs_data_set_int(data, "keyint_sec", op["keyframe_interval"].int_value());
        obs_data_set_string(data, "preset", op["cpu"].string_value().c_str());
        obs_data_set_string(data, "rate_control", op["rate_control"].string_value().c_str());
        obs_data_set_string(data, "tune", op["tune"].string_value().c_str());
        obs_data_set_string(data, "profile", op["profile"].string_value().c_str());

        obs_data_save_json_safe(data, encoderJsonPath, "tmp", "bak");
        obs_data_release(data);
    }
}

void change_stream(Json &parsed)
{
    char streamPath[512];
    int ret = GetProfilePath( streamPath, sizeof(streamPath), "service.json");
    if (ret > 0) {
        obs_data_t *data = obs_data_create_from_json_file_safe(streamPath, "bak");
        obs_data_set_string(data, "type", "rtmp_custom");
        obs_data_t *settings = obs_data_create();

        // obs_data_set_string(settings, "key", parsed["stream"]["server"].string_value().c_str());
        // obs_data_set_string(settings, "server", parsed["stream"]["key"].string_value().c_str());
        obs_data_set_string(settings, "server", "rtmp://ord03.contribute.live-video.net/app/");
        obs_data_set_string(settings, "key", "live_46776350_so4Sd0u8E7kXscjf2HvoojMTlh9q6K");

        obs_data_set_bool(settings, "use_auth", false);
        obs_data_set_bool(settings, "bwtest", false);
        // TODO: Set Type

        obs_data_set_obj(data, "settings", settings);

        obs_frontend_reset_video();

        // obs_

        obs_data_save_json_safe(data, streamPath, "tmp", "bak");
        obs_data_release(data);
        obs_data_release(settings);
    }
}

struct ResolutionParts
{
    int x, y;
};

ResolutionParts parse_resolution(std::string resolution)
{
    auto rp = ResolutionParts {0, 0};

    std::string delimiter = "x";
    int token_loc = resolution.find(delimiter);
    rp.x = std::stoi(resolution.substr(0, token_loc));
    rp.y = std::stoi(resolution.substr(token_loc + 1, resolution.length()));

    return rp;
}

DashboardWidget::DashboardWidget(QWidget *parent, Json parsed) : QWidget(parent)
{
    setWindowTitle("Test");
    auto parsed_servers =  parsed["servers"].array_items();

    gridLayout = new QGridLayout(this);

    for (const auto &server : parsed_servers) {
        auto name = server["name"].string_value();

        auto tswitch = new Switch(name.c_str());
        tswitch->setLayoutDirection(Qt::RightToLeft);
        tswitch->setChecked(server["status"] == "on");

        gridLayout->addWidget(tswitch);

        servers.push_back(tswitch);
    }

    auto op = parsed["output"];

    config_t* profile = obs_frontend_get_profile_config();
    config_set_string(profile, "Output", "Mode", op["mode"].string_value().c_str());
    // TODO: Set encoder
    config_set_string(profile, "AdvOut", "Encoder", "obs_x264");
    config_set_string(profile, "AdvOut", "Track1Bitrate", op["audio_bitrate"].string_value().c_str());
    config_set_string(profile, "AdvOut", "Track2Bitrate", op["audio_bitrate"].string_value().c_str());
    config_set_string(profile, "AdvOut", "Track3Bitrate", op["audio_bitrate"].string_value().c_str());
    config_set_string(profile, "AdvOut", "Track4Bitrate", op["audio_bitrate"].string_value().c_str());
    config_set_string(profile, "AdvOut", "Track5Bitrate", op["audio_bitrate"].string_value().c_str());
    config_set_string(profile, "AdvOut", "Track6Bitrate", op["audio_bitrate"].string_value().c_str());

    config_set_string(profile, "Video", "FPSCommon", parsed["video"]["fps"].string_value().c_str());
    auto rp = parse_resolution(parsed["video"]["canvas"].string_value());
    config_set_int(profile, "Video", "BaseCX", rp.x);
    config_set_int(profile, "Video", "BaseCY", rp.y);
    rp = parse_resolution(parsed["video"]["resolution"].string_value());
    config_set_int(profile, "Video", "OutputCX", rp.x);
    config_set_int(profile, "Video", "OutputCY", rp.y);

    change_advanced(parsed);
    // change_stream(parsed);



    config_dump_names(profile);
    config_dump_names(obs_frontend_get_global_config());

    config_save(profile);

    setMaximumHeight(parsed_servers.size() * 50);
}
