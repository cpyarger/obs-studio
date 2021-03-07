project(mike)
set(PROJECT_FULL_NAME "OBS Login System")
set(PROJECT_NAME mike)

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

set(${PROJECT_NAME}_SOURCES
  container-widget.cpp
  dashboard-widget.cpp
  switch.cpp
  login-widget.cpp)

set(${PROJECT_NAME}_HEADERS
  container-widget.hpp
  dashboard-widget.hpp
  switch.hpp
  style.hpp
  login-widget.hpp)

add_library(${PROJECT_NAME} MODULE ${mike_SOURCES} ${mike_HEADERS})
target_link_libraries(${PROJECT_NAME}
  obs-frontend-api
  Qt5::Widgets
  libobs)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "plugins/mike-server")
set_target_properties(${PROJECT_NAME} PROPERTIES AUTOMOC TRUE)
install_obs_plugin_with_data(${PROJECT_NAME} data)
