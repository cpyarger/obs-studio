project(mike)
set(PROJECT_FULL_NAME "OBS Login System")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#set(mike_SOURCES
#mike - api.cpp
#mike - login.cpp
#edit - widget.cpp
#obs - multi - rtmp.cpp
#push - widget.h)

#set(mike_HEADERS
#mike - api.hpp
#mike - login.hpp
#edit - widget.h
#obs - multi - rtmp.h
#pch.h
#push - widget.h)

set(mike_SOURCES
  container-widget.cpp
  dashboard-widget.cpp
  switch.cpp
  login-widget.cpp)

set(mike_HEADERS
  container-widget.hpp
  dashboard-widget.hpp
  switch.hpp
  style.hpp
  login-widget.hpp)

add_library(mike MODULE ${mike_SOURCES} ${mike_HEADERS})
target_link_libraries(mike
  obs-frontend-api
  Qt5::Widgets
  libobs)

set_target_properties(mike PROPERTIES FOLDER "plugins/mike-server")
install_obs_plugin_with_data(mike data)
