project(mike)
set(PROJECT_FULL_NAME "OBS Login System")
set(PROJECT_NAME mike)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(Libcurl REQUIRED)
include_directories(${LIBCURL_INCLUDE_DIRS})
add_definitions(${LIBCURL_DEFINITIONS})
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/deps/json11")
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
  ${CMAKE_SOURCE_DIR}/deps/json11/json11.cpp
  container-widget.cpp
  dashboard-widget.cpp
  switch.cpp
  login-widget.cpp)

set(${PROJECT_NAME}_HEADERS
  ${CMAKE_SOURCE_DIR}/deps/json11/json11.hpp
  container-widget.hpp
  dashboard-widget.hpp
  switch.hpp
  style.hpp
  login-widget.hpp)

add_library(${PROJECT_NAME} MODULE ${mike_SOURCES} ${mike_HEADERS})
target_link_libraries(${PROJECT_NAME}
  obs-frontend-api
  Qt5::Widgets
  ${LIBCURL_LIBRARIES}
)

if(WIN32)

    if(MSVC)
        # Enable Multicore Builds and disable FH4 (to not depend on VCRUNTIME140_1.DLL)
	    add_definitions(/MP /d2FH4-)
    endif()
	add_definitions(-D_WEBSOCKETPP_CPP11_STL_)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(ARCH_NAME "64bit")
		set(OBS_BUILDDIR_ARCH "build64")
	else()
		set(ARCH_NAME "32bit")
		set(OBS_BUILDDIR_ARCH "build32")
	endif()
	include_directories(
		"${LIBOBS_INCLUDE_DIR}/../${OBS_BUILDDIR_ARCH}/UI"
	)
	if(NOT DEFINED OBS_FRONTEND_LIB)
		target_link_libraries(${PROJECT_NAME}
		obs-frontend-api)
	else()
		target_link_libraries(${PROJECT_NAME}
		"${OBS_FRONTEND_LIB}")
	endif()

	# --- Release package helper ---
	# The "release" folder has a structure similar OBS' one on Windows
	
	# --- End of sub-section ---
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "plugins/mike-server")
set_target_properties(${PROJECT_NAME} PROPERTIES AUTOMOC TRUE)
install_obs_plugin_with_data(${PROJECT_NAME} data)
