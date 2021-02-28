#include "pch.h"
#include "mike-api.hpp"
#include <json11.hpp>
#include <util/base.h>

#include <iostream>

using namespace json11;

namespace {
int curl_string_callback(void *data, int size, int cnt, void *user)
{
    auto str = (std::string *)user;
    str->insert(str->end(), (const char *)data, (const char *)data + (size * cnt));
    return cnt;
}

static struct MikeAPI {
    MikeAPI()
    {
        if (curl_global_init(CURL_GLOBAL_DEFAULT) == CURLE_OK) {
            usable = true;
        } else {
            usable = false;
        }
    }

    bool usable;

    std::string Login(std::string un, std::string pw)
    {
        if (!usable)
            return {};
        CURL *h = curl_easy_init();
        if (!h)
            return {};
        auto e_un = curl_easy_escape(h, un.c_str(), un.size());
        auto e_pw = curl_easy_escape(h, pw.c_str(), pw.size());
        std::string url =
                std::string("https://mdca.co.com/api/obs_login.php?user=") + e_un + "&pass=" + e_pw;
        curl_free(e_un);
        curl_free(e_pw);

        std::string result;
        curl_easy_setopt(h, CURLOPT_TIMEOUT, 10L);
        curl_easy_setopt(h, CURLOPT_URL, url.c_str());
        curl_easy_setopt(h, CURLOPT_WRITEFUNCTION, &curl_string_callback);
        curl_easy_setopt(h, CURLOPT_WRITEDATA, &result);
        curl_easy_setopt(h, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NO_REVOKE);
        auto ret = curl_easy_perform(h);
        curl_easy_cleanup(h);

        if (ret == CURLE_OK)
            return result;
        else
            return {};
    }
} s_api;
};

std::string Login(std::string un, std::string pw)
{
    return s_api.Login(un, pw);
}

bool LoadLoginResult(std::string loginResult, Json &output)
{
    std::string err;
    output = Json::parse(loginResult, err);

    return true;

    // QJsonDocument jsondoc = QJsonDocument::fromJson(QByteArray(loginResult.c_str(), loginResult.size()));
    // if (!jsondoc.isObject())
    //     return false;
    // auto j = jsondoc.object();

    // {
    //     auto it = j.find("status");
    //     if (it == j.end() || !it->isString())
    //         return false;
    //     if (it->toString() != "TRUE")
    //         return false;
    // }

    // QJsonArray servers;
    // {
    //     auto it = j.find("servers");
    //     if (it == j.end() || !it->isArray())
    //         return false;
    //     servers = it->toArray();
    // }

    // QJsonArray targets;
    // for(const auto& node : servers) {
    //     if (!node.isObject())
    //         continue;
    //     auto server = node.toObject();

    //     QJsonObject cur;
    //     auto it = server.find("service");
    //     if (it == server.end() || !it->isString())
    //         continue;
    //     cur.insert("name", it->toString());

    //     it = server.find("rtmp");
    //     if (it == server.end() || !it->isString())
    //         continue;
    //     auto serviceConf = it->toString();
    //     auto sjdoc = QJsonDocument::fromJson(serviceConf.toUtf8());
    //     if (!sjdoc.isObject())
    //         continue;
    //     auto sj = sjdoc.object();

    //     it = sj.find("server");
    //     if (it == sj.end() || !it->isString())
    //         continue;
    //     cur.insert("rtmp-path", it->toString());

    //     it = sj.find("key");
    //     if (it != sj.end() && it->isString())
    //         cur.insert("rtmp-key", it->toString());

    //     it = sj.find("video_encoder");
    //     if (it != sj.end() && it->isString() && it->toString() != "get_from_obs")
    //         cur.insert("v-enc", it->toString());

    //     it = sj.find("video_resolution");
    //     if (it != sj.end() && it->isString() && it->toString() != "get_from_obs")
    //         cur.insert("v-resolution", it->toString());

    //     try {
    //         it = sj.find("video_bitrate");
    //         if (it != sj.end() && it->isString() && it->toString() != "get_from_obs")
    //             cur.insert("v-bitrate", std::stod(it->toString().toStdWString()));
    //     } catch(...) {}

    //     try {
    //         it = sj.find("video_keyframe");
    //         if (it != sj.end() && it->isString() && it->toString() != "get_from_obs")
    //             cur.insert("v-keyframe-sec", std::stod(it->toString().toStdWString()));
    //     } catch(...) {}

    //     it = sj.find("audio_encoder");
    //     if (it != sj.end() && it->isString() && it->toString() != "get_from_obs")
    //         cur.insert("a-enc", it->toString());

    //     try {
    //         it = sj.find("audio_bitrate");
    //         if (it != sj.end() && it->isString() && it->toString() != "get_from_obs")
    //             cur.insert("a-bitrate", std::stod(it->toString().toStdWString()));
    //     } catch(...) {}

    //     targets.append(cur);
    // }

    // output.insert("targets", targets);
    // return true;
}
