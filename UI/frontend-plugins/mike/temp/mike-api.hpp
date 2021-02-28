#pragma once

#include "curl/curl.h"
#include <json11.hpp>

std::string Login(std::string un, std::string pw);

bool LoadLoginResult(std::string loginResult, json11::Json &output);
