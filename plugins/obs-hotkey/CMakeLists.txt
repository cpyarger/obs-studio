cmake_minimum_required(VERSION 3.10)

# Change obs-plugintemplate to your plugin's name in a machine-readable format
# (e.g.: obs-myawesomeplugin) and set 
project(obs-hotkey VERSION 1.0.0)

# Replace `Your Name Here` with the name (yours or your organization's) you want
# to see as the author of the plugin (in the plugin's metadata itself and in the installers)
set(PLUGIN_AUTHOR "Your Name Here")

# Replace `com.example.obs-plugin-template` with a unique Bundle ID for macOS releases
# (used both in the installer and when submitting the installer for notarization)
set(MACOS_BUNDLEID "com.example.obs-plugintemplate")

# Replace `me@contoso.com` with the maintainer email address you want to put in Linux packages
set(LINUX_MAINTAINER_EMAIL "me@contoso.com")

# TAKE NOTE: No need to edit things past this point

set(CMAKE_PREFIX_PATH "${QTDIR}")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# In case you need C++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (${isAzure})
	if (WIN32 OR APPLE)
		include(external/FindLibObs.cmake)
	endif()
	find_package(LibObs REQUIRED)
endif()
if (APPLE)
	include(external/FindLibObs.cmake)
	find_package(LibObs REQUIRED)
endif()

find_package(LibObs REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets)



set(PLUGIN_SOURCES
	src/obs-hotkey.cpp
	src/hotkey-edit.cpp)


set(PLUGIN_HEADERS
	src/obs-hotkey.h
	src/hotkey-edit.hpp)

# --- Platform-independent build settings ---
add_library(obs-hotkey MODULE ${PLUGIN_SOURCES} ${PLUGIN_HEADERS})
if(WIN32 OR LINUX)
if (${isAzure})

	target_link_libraries(obs-hotkey
					"${OBS_FRONTEND_LIB}"
					
					Qt5::Core
					Qt5::Widgets
					libobs)
	else()
	target_link_libraries(obs-hotkey
					obs-frontend-api
					
					Qt5::Core
					Qt5::Widgets
					libobs)
  endif()
else()
	target_link_libraries(obs-hotkey
					"${OBS_FRONTEND_LIB}"
					
					Qt5::Core
					Qt5::Widgets
					libobs)
endif()
target_link_libraries(obs-hotkey
	libobs
	Qt5::Core
	Qt5::Widgets
)

# --- End of section ---

# --- Windows-specific build settings and tasks ---
if (${isAzure})
if(WIN32)
	if(NOT DEFINED OBS_FRONTEND_LIB)
		set(OBS_FRONTEND_LIB "OBS_FRONTEND_LIB-NOTFOUND" CACHE FILEPATH "OBS frontend library")
		message(FATAL_ERROR "Could not find OBS Frontend API\'s library !")
	endif()

    # Enable Multicore Builds and disable FH4 (to not depend on VCRUNTIME140_1.DLL when building with VS2019)
    if (MSVC)
        add_definitions(/MP /d2FH4-)
    endif()

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

	target_link_libraries(obs-hotkey
		"${OBS_FRONTEND_LIB}"
    )

	# --- Release package helper ---
	# The "release" folder has a structure similar OBS' one on Windows
	set(RELEASE_DIR "${PROJECT_SOURCE_DIR}/release")

    add_custom_command(TARGET obs-hotkey POST_BUILD
        # If config is Release or RelWithDebInfo, package release files
        COMMAND if $<CONFIG:Release>==1 OR $<CONFIG:RelWithDebInfo>==1 (
            "${CMAKE_COMMAND}" -E make_directory
                "${RELEASE_DIR}/data/obs-plugins/obs-hotkey"
                "${RELEASE_DIR}/obs-plugins/${ARCH_NAME}"
        )

        COMMAND if $<CONFIG:Release>==1 OR $<CONFIG:RelWithDebInfo>==1 (
            "${CMAKE_COMMAND}" -E copy_directory
                "${PROJECT_SOURCE_DIR}/data"
                "${RELEASE_DIR}/data/obs-plugins/obs-hotkey"
        )

        COMMAND if $<CONFIG:Release>==1 OR $<CONFIG:RelWithDebInfo>==1 (
            "${CMAKE_COMMAND}" -E copy
                "$<TARGET_FILE:obs-hotkey>"
                "${RELEASE_DIR}/obs-plugins/${ARCH_NAME}"
        )

        # If config is RelWithDebInfo, copy the pdb file
        COMMAND if $<CONFIG:RelWithDebInfo>==1 (
            "${CMAKE_COMMAND}" -E copy
                "$<TARGET_PDB_FILE:obs-hotkey>"
                "${RELEASE_DIR}/obs-plugins/${ARCH_NAME}"
        )

        # Copy to obs-studio dev environment for immediate testing
        COMMAND if $<CONFIG:Debug>==1 (
            "${CMAKE_COMMAND}" -E copy
                "$<TARGET_FILE:obs-hotkey>"
                "${LIBOBS_INCLUDE_DIR}/../${OBS_BUILDDIR_ARCH}/rundir/$<CONFIG>/obs-plugins/${ARCH_NAME}"
        )

        COMMAND if $<CONFIG:Debug>==1 (
            "${CMAKE_COMMAND}" -E copy
                "$<TARGET_PDB_FILE:obs-hotkey>"
                "${LIBOBS_INCLUDE_DIR}/../${OBS_BUILDDIR_ARCH}/rundir/$<CONFIG>/obs-plugins/${ARCH_NAME}"
        )

        COMMAND if $<CONFIG:Debug>==1 (
            "${CMAKE_COMMAND}" -E make_directory
                "${LIBOBS_INCLUDE_DIR}/../${OBS_BUILDDIR_ARCH}/rundir/$<CONFIG>/data/obs-plugins/obs-hotkey"
        )

        COMMAND if $<CONFIG:Debug>==1 (
            "${CMAKE_COMMAND}" -E copy_directory
                "${PROJECT_SOURCE_DIR}/data"
                "${LIBOBS_INCLUDE_DIR}/../${OBS_BUILDDIR_ARCH}/rundir/$<CONFIG>/data/obs-plugins/obs-hotkey"
        )
    )
	# --- End of sub-section ---

endif()
# --- End of section ---
endif()
# -- End of if AZURE
# --- Linux-specific build settings and tasks ---
if(UNIX AND NOT APPLE)
    include(GNUInstallDirs)

	set_target_properties(obs-hotkey PROPERTIES PREFIX "")
	target_link_libraries(obs-hotkey obs-frontend-api)

	file(GLOB locale_files data/locale/*.ini)

	install(TARGETS obs-hotkey
		LIBRARY DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}/obs-plugins")

	install(FILES ${locale_files}
		DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/obs/obs-plugins/obs-hotkey/locale")
endif()
# --- End of section ---

# -- OS X specific build settings and tasks --
if(APPLE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -fvisibility=default")

	set_target_properties(obs-hotkey PROPERTIES PREFIX "")
	target_link_libraries(obs-hotkey "${OBS_FRONTEND_LIB}")
endif()
# -- End of section --
