cmake_minimum_required(VERSION 3.10)
# Change obs-plugintemplate to your plugin's name in a machine-readable format
# (e.g.: obs-myawesomeplugin) and set
project(obs-hotkeys VERSION 0.2.4)

# Replace `Your Name Here` with the name (yours or your organization's) you want
# to see as the author of the plugin (in the plugin's metadata itself and in the installers)
set(PLUGIN_AUTHOR "cpyarger")

# Replace `com.example.obs-plugin-template` with a unique Bundle ID for macOS releases
# (used both in the installer and when submitting the installer for notarization)
set(MACOS_BUNDLEID "com.example.obs-hotkeys")

# Replace `me@contoso.com` with the maintainer email address you want to put in Linux packages
set(LINUX_MAINTAINER_EMAIL "cpyarger@gmail.com")

if(WIN32)
	set(MODULE_DESCRIPTION "OBS Hotkeys module")
endif()

##################
# THE GOOD STUFF #
##################

# RThotkey
add_subdirectory(src/QHotkey)

set(obs-hotkeys_DEPS QHotkey)

include_directories(src/QHotkey)




# OBS hotkey
set(CMAKE_PREFIX_PATH "${QTDIR}")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DASIO_STANDALONE)
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

find_package(Qt5 REQUIRED COMPONENTS Core Widgets)
set(hotkey_ui
        src/forms/OBSHotkeysWidget.ui
)

qt5_wrap_ui(hotkey_ui_headers ${hotkey_ui})

include_directories(
	"${LIBOBS_INCLUDE_DIR}/../UI/obs-frontend-api"
        SYSTEM
	${Qt5Core_INCLUDES}
	${Qt5Widgets_INCLUDES}
        "${CMAKE_SOURCE_DIR}/UI")

set(obs-hotkey_SOURCES
	${hotkey_ui_headers}
	src/obs-hotkey.cpp
	src/forms/obs-hotkeys-widget.cpp
)

list(APPEND obs-hotkey_HEADERS
	src/obs-hotkey.h
	src/forms/obs-hotkeys-widget.hpp)

add_library(obs-hotkey MODULE
${hotkey_ui}
        ${obs-hotkey_SOURCES}
        ${obs-hotkey_HEADERS})
if (${isAzure})
if(WIN32 OR LINUX)


	target_link_libraries(obs-hotkey
					"${OBS_FRONTEND_LIB}"
					${obs-hotkey_DEPS}
					Qt5::Core
					Qt5::Widgets
					libobs
					qhotkey)



else()
	target_link_libraries(obs-hotkey
					"${OBS_FRONTEND_LIB}"
					${obs-hotkey_DEPS}
					Qt5::Core
					Qt5::Widgets
					libobs
					qhotkey)
endif()
endif()
# --- Windows-specific build settings and tasks ---


if(WIN32)
	if (${isAzure})
		if(NOT DEFINED OBS_FRONTEND_LIB)
			set(OBS_FRONTEND_LIB "OBS_FRONTEND_LIB-NOTFOUND" CACHE FILEPATH "OBS frontend library")
			message(FATAL_ERROR "Could not find OBS Frontend API's library !")
		endif()

	    if(MSVC)
	        # Enable Multicore Builds and disable FH4 (to not depend on VCRUNTIME140_1.DLL)
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
			qhotkey)

		# --- Release package helper ---
		# The "release" folder has a structure similar OBS' one on Windows
		set(RELEASE_DIR "${PROJECT_SOURCE_DIR}/release")
		add_custom_command(TARGET obs-hotkey POST_BUILD
			# If config is Release, package release files
			COMMAND if $<CONFIG:Release>==1 (
				"${CMAKE_COMMAND}" -E make_directory
				"${RELEASE_DIR}/data/obs-plugins/obs-hotkey"
				"${RELEASE_DIR}/obs-plugins/${ARCH_NAME}")

			COMMAND if $<CONFIG:Release>==1 ("${CMAKE_COMMAND}" -E copy_directory
				"${PROJECT_SOURCE_DIR}/data"
				"${RELEASE_DIR}/data/obs-plugins/obs-hotkey")

			COMMAND if $<CONFIG:Release>==1 ("${CMAKE_COMMAND}" -E copy
				"$<TARGET_FILE:obs-hotkey>"
				"${RELEASE_DIR}/obs-plugins/${ARCH_NAME}")

			# In Release mode, copy Qt image format plugins
			COMMAND if $<CONFIG:Release>==1 (
				"${CMAKE_COMMAND}" -E copy
				"${QTDIR}/plugins/imageformats/qjpeg.dll"
				"${RELEASE_DIR}/bin/${ARCH_NAME}/imageformats/qjpeg.dll")
			COMMAND if $<CONFIG:RelWithDebInfo>==1 (
				"${CMAKE_COMMAND}" -E copy
				"${QTDIR}/plugins/imageformats/qjpeg.dll"
				"${RELEASE_DIR}/bin/${ARCH_NAME}/imageformats/qjpeg.dll")

			# If config is RelWithDebInfo, package release files
			COMMAND if $<CONFIG:RelWithDebInfo>==1 (
				"${CMAKE_COMMAND}" -E make_directory
				"${RELEASE_DIR}/data/obs-plugins/obs-hotkey"
				"${RELEASE_DIR}/obs-plugins/${ARCH_NAME}")

			COMMAND if $<CONFIG:RelWithDebInfo>==1 ("${CMAKE_COMMAND}" -E copy_directory
				"${PROJECT_SOURCE_DIR}/data"
				"${RELEASE_DIR}/data/obs-plugins/obs-hotkey")

			COMMAND if $<CONFIG:RelWithDebInfo>==1 ("${CMAKE_COMMAND}" -E copy
				"$<TARGET_FILE:obs-hotkey>"
				"${RELEASE_DIR}/obs-plugins/${ARCH_NAME}")

			COMMAND if $<CONFIG:RelWithDebInfo>==1 ("${CMAKE_COMMAND}" -E copy
				"$<TARGET_PDB_FILE:obs-hotkey>"
				"${RELEASE_DIR}/obs-plugins/${ARCH_NAME}")

			# Copy to obs-studio dev environment for immediate testing
			COMMAND if $<CONFIG:Debug>==1 (
				"${CMAKE_COMMAND}" -E copy
					"$<TARGET_FILE:obs-hotkey>"
					"${LIBOBS_INCLUDE_DIR}/../${OBS_BUILDDIR_ARCH}/rundir/$<CONFIG>/obs-plugins/${ARCH_NAME}")

			COMMAND if $<CONFIG:Debug>==1 (
				"${CMAKE_COMMAND}" -E copy
					"$<TARGET_PDB_FILE:obs-hotkey>"
					"${LIBOBS_INCLUDE_DIR}/../${OBS_BUILDDIR_ARCH}/rundir/$<CONFIG>/obs-plugins/${ARCH_NAME}")

			COMMAND if $<CONFIG:Debug>==1 (
				"${CMAKE_COMMAND}" -E make_directory
					"${LIBOBS_INCLUDE_DIR}/../${OBS_BUILDDIR_ARCH}/rundir/$<CONFIG>/data/obs-plugins/obs-hotkey")

			COMMAND if $<CONFIG:Debug>==1 (
				"${CMAKE_COMMAND}" -E copy_directory
					"${PROJECT_SOURCE_DIR}/data"
					"${LIBOBS_INCLUDE_DIR}/../${OBS_BUILDDIR_ARCH}/rundir/$<CONFIG>/data/obs-plugins/obs-hotkey")
		)
		# --- End of sub-section ---

		
	endif()# -- End of if AZURE


endif()# --- End of win---


# --- Linux-specific build settings and tasks ---
if(UNIX AND NOT APPLE)
	include(GNUInstallDirs)

	set_target_properties(obs-hotkey PROPERTIES PREFIX "")
	target_link_libraries(obs-hotkey obs-frontend-api qhotkey)

	file(GLOB locale_files data/locale/*.ini)

	if(${USE_UBUNTU_FIX})
		install(TARGETS obs-hotkey
			LIBRARY DESTINATION "/usr/lib/obs-plugins")
	endif()
	install(TARGETS obs-hotkey
		LIBRARY DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}/obs-plugins")

	install(FILES ${locale_files}
		DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/obs/obs-plugins/obs-hotkey/locale")
endif()
# --- End of section ---

# -- OS X specific build settings and tasks --
if(APPLE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -fvisibility=default")
	set(CMAKE_SKIP_RPATH TRUE)
	set_target_properties(obs-hotkey PROPERTIES PREFIX "")
	target_link_libraries(obs-hotkey "${OBS_FRONTEND_LIB}")
endif()
# -- End of section --

#is azure endif

target_link_libraries(obs-hotkey
		obs-frontend-api 
		${obs-hotkey_DEPS} 
		Qt5::Core 
		Qt5::Widgets 
		libobs
		qhotkey)
install_obs_plugin_with_data(obs-hotkey data)
set_target_properties(obs-hotkey PROPERTIES FOLDER "Control Plugins")
set_target_properties(qhotkey PROPERTIES FOLDER "Control Plugins/Externals")

