cmake_minimum_required(VERSION 3.10)
# Change obs-plugintemplate to your plugin's name in a machine-readable format
# (e.g.: obs-myawesomeplugin) and set
project(obs-actions VERSION 0.2.4)

# Replace `Your Name Here` with the name (yours or your organization's) you want
# to see as the author of the plugin (in the plugin's metadata itself and in the installers)
set(PLUGIN_AUTHOR "cpyarger")

# Replace `com.example.obs-plugin-template` with a unique Bundle ID for macOS releases
# (used both in the installer and when submitting the installer for notarization)
set(MACOS_BUNDLEID "com.example.obs-actions")

# Replace `me@contoso.com` with the maintainer email address you want to put in Linux packages
set(LINUX_MAINTAINER_EMAIL "cpyarger@gmail.com")

if(WIN32)
	set(MODULE_DESCRIPTION "OBS actions module")
endif()

##################
# THE GOOD STUFF #
##################




# OBS action
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
set(action_ui
        src/forms/OBSActionsWidget.ui
)

qt5_wrap_ui(action_ui_headers ${action_ui})

include_directories(
	"${LIBOBS_INCLUDE_DIR}/../UI/obs-frontend-api"
        SYSTEM
	${Qt5Core_INCLUDES}
	${Qt5Widgets_INCLUDES}
        "${CMAKE_SOURCE_DIR}/UI")

set(obs-action_SOURCES
	${action_ui_headers}
	src/obs-action.cpp
	src/obs-controller.cpp
	src/utils.cpp
	src/forms/obs-actions-widget.cpp
)

list(APPEND obs-action_HEADERS
	src/obs-action.h
	src/obs-controller.hpp
	src/utils.hpp	
	src/forms/obs-actions-widget.hpp
)

add_library(obs-action MODULE
${action_ui}
        ${obs-action_SOURCES}
        ${obs-action_HEADERS})
if (${isAzure})
if(WIN32 OR LINUX)


	target_link_libraries(obs-action
					"${OBS_FRONTEND_LIB}"
					${obs-action_DEPS}
					Qt5::Core
					Qt5::Widgets
					libobs)



else()
	target_link_libraries(obs-action
					"${OBS_FRONTEND_LIB}"
					${obs-action_DEPS}
					Qt5::Core
					Qt5::Widgets
					libobs)
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

		target_link_libraries(obs-action
			"${OBS_FRONTEND_LIB}")

		# --- Release package helper ---
		# The "release" folder has a structure similar OBS' one on Windows
		set(RELEASE_DIR "${PROJECT_SOURCE_DIR}/release")
		add_custom_command(TARGET obs-action POST_BUILD
			# If config is Release, package release files
			COMMAND if $<CONFIG:Release>==1 (
				"${CMAKE_COMMAND}" -E make_directory
				"${RELEASE_DIR}/data/obs-plugins/obs-action"
				"${RELEASE_DIR}/obs-plugins/${ARCH_NAME}")

			COMMAND if $<CONFIG:Release>==1 ("${CMAKE_COMMAND}" -E copy_directory
				"${PROJECT_SOURCE_DIR}/data"
				"${RELEASE_DIR}/data/obs-plugins/obs-action")

			COMMAND if $<CONFIG:Release>==1 ("${CMAKE_COMMAND}" -E copy
				"$<TARGET_FILE:obs-action>"
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
				"${RELEASE_DIR}/data/obs-plugins/obs-action"
				"${RELEASE_DIR}/obs-plugins/${ARCH_NAME}")

			COMMAND if $<CONFIG:RelWithDebInfo>==1 ("${CMAKE_COMMAND}" -E copy_directory
				"${PROJECT_SOURCE_DIR}/data"
				"${RELEASE_DIR}/data/obs-plugins/obs-action")

			COMMAND if $<CONFIG:RelWithDebInfo>==1 ("${CMAKE_COMMAND}" -E copy
				"$<TARGET_FILE:obs-action>"
				"${RELEASE_DIR}/obs-plugins/${ARCH_NAME}")

			COMMAND if $<CONFIG:RelWithDebInfo>==1 ("${CMAKE_COMMAND}" -E copy
				"$<TARGET_PDB_FILE:obs-action>"
				"${RELEASE_DIR}/obs-plugins/${ARCH_NAME}")

			# Copy to obs-studio dev environment for immediate testing
			COMMAND if $<CONFIG:Debug>==1 (
				"${CMAKE_COMMAND}" -E copy
					"$<TARGET_FILE:obs-action>"
					"${LIBOBS_INCLUDE_DIR}/../${OBS_BUILDDIR_ARCH}/rundir/$<CONFIG>/obs-plugins/${ARCH_NAME}")

			COMMAND if $<CONFIG:Debug>==1 (
				"${CMAKE_COMMAND}" -E copy
					"$<TARGET_PDB_FILE:obs-action>"
					"${LIBOBS_INCLUDE_DIR}/../${OBS_BUILDDIR_ARCH}/rundir/$<CONFIG>/obs-plugins/${ARCH_NAME}")

			COMMAND if $<CONFIG:Debug>==1 (
				"${CMAKE_COMMAND}" -E make_directory
					"${LIBOBS_INCLUDE_DIR}/../${OBS_BUILDDIR_ARCH}/rundir/$<CONFIG>/data/obs-plugins/obs-action")

			COMMAND if $<CONFIG:Debug>==1 (
				"${CMAKE_COMMAND}" -E copy_directory
					"${PROJECT_SOURCE_DIR}/data"
					"${LIBOBS_INCLUDE_DIR}/../${OBS_BUILDDIR_ARCH}/rundir/$<CONFIG>/data/obs-plugins/obs-action")
		)
		# --- End of sub-section ---

		
	endif()# -- End of if AZURE


endif()# --- End of win---


# --- Linux-specific build settings and tasks ---
if(UNIX AND NOT APPLE)
	include(GNUInstallDirs)

	set_target_properties(obs-action PROPERTIES PREFIX "")
	target_link_libraries(obs-action obs-frontend-api Qaction)

	file(GLOB locale_files data/locale/*.ini)

	if(${USE_UBUNTU_FIX})
		install(TARGETS obs-action
			LIBRARY DESTINATION "/usr/lib/obs-plugins")
	endif()
	install(TARGETS obs-action
		LIBRARY DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}/obs-plugins")

	install(FILES ${locale_files}
		DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/obs/obs-plugins/obs-action/locale")
endif()
# --- End of section ---

# -- OS X specific build settings and tasks --
if(APPLE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -fvisibility=default")
	set(CMAKE_SKIP_RPATH TRUE)
	set_target_properties(obs-action PROPERTIES PREFIX "")
	target_link_libraries(obs-action "${OBS_FRONTEND_LIB}")
endif()
# -- End of section --

#is azure endif

target_link_libraries(obs-action
		obs-frontend-api 
		${obs-action_DEPS} 
		Qt5::Core 
		Qt5::Widgets 
		libobs
		)
set_target_properties(obs-action PROPERTIES FOLDER "Control Plugins")
install_obs_plugin_with_data(obs-action data)
