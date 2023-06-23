# Project-level configuration.
cmake_minimum_required(VERSION 3.10)
project(flutter-linux-gtk LANGUAGES CXX)

SET(LIBRARY_NAME flutter_linux_gtk)
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
SET(CMAKE_INSTALL_RPATH "\${ORIGIN}")

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/install" CACHE PATH "..." FORCE)
endif()

include(FetchContent)
FetchContent_Declare(
  prebuilt_embedder
  URL      https://storage.googleapis.com/flutter_infra_release/flutter/45f6e009110df4f34ec2cf99f63cf73b71b7a420/linux-x64/linux-x64-embedder.zip
  URL_HASH SHA256=37737a2a74d53e3e9c66766d16f85039706814a43d6f94cc4b22724256f5f094
  DOWNLOAD_DIR ${CMAKE_BINARY_DIR}
)
FetchContent_MakeAvailable(prebuilt_embedder)
set(FLUTTER_ENGINE_LIBRARY "${prebuilt_embedder_SOURCE_DIR}/libflutter_engine.so")

add_library(${LIBRARY_NAME} SHARED
"shell/platform/linux/fl_accessibility_plugin.cc"
"shell/platform/linux/fl_accessible_node.cc"
"shell/platform/linux/fl_accessible_text_field.cc"
"shell/platform/linux/fl_backing_store_provider.cc"
"shell/platform/linux/fl_basic_message_channel.cc"
"shell/platform/linux/fl_binary_codec.cc"
"shell/platform/linux/fl_binary_messenger.cc"
"shell/platform/linux/fl_dart_project.cc"
"shell/platform/linux/fl_engine.cc"
"shell/platform/linux/fl_event_channel.cc"
"shell/platform/linux/fl_gl_area.cc"
"shell/platform/linux/fl_gnome_settings.cc"
"shell/platform/linux/fl_json_message_codec.cc"
"shell/platform/linux/fl_json_method_codec.cc"
"shell/platform/linux/fl_key_channel_responder.cc"
"shell/platform/linux/fl_key_embedder_responder.cc"
"shell/platform/linux/fl_key_event.cc"
"shell/platform/linux/fl_key_responder.cc"
"shell/platform/linux/fl_keyboard_manager.cc"
"shell/platform/linux/fl_keyboard_view_delegate.cc"
"shell/platform/linux/fl_message_codec.cc"
"shell/platform/linux/fl_method_call.cc"
"shell/platform/linux/fl_method_channel.cc"
"shell/platform/linux/fl_method_codec.cc"
"shell/platform/linux/fl_method_response.cc"
"shell/platform/linux/fl_mouse_cursor_plugin.cc"
"shell/platform/linux/fl_pixel_buffer_texture.cc"
"shell/platform/linux/fl_platform_plugin.cc"
"shell/platform/linux/fl_platform_view.cc"
"shell/platform/linux/fl_platform_view_factory.cc"
"shell/platform/linux/fl_platform_views_plugin.cc"
"shell/platform/linux/fl_plugin_registrar.cc"
"shell/platform/linux/fl_plugin_registry.cc"
"shell/platform/linux/fl_renderer.cc"
"shell/platform/linux/fl_renderer_gl.cc"
"shell/platform/linux/fl_renderer_headless.cc"
"shell/platform/linux/fl_scrolling_manager.cc"
"shell/platform/linux/fl_scrolling_view_delegate.cc"
"shell/platform/linux/fl_settings.cc"
"shell/platform/linux/fl_settings_plugin.cc"
"shell/platform/linux/fl_settings_portal.cc"
"shell/platform/linux/fl_standard_message_codec.cc"
"shell/platform/linux/fl_standard_method_codec.cc"
"shell/platform/linux/fl_string_codec.cc"
"shell/platform/linux/fl_task_runner.cc"
"shell/platform/linux/fl_task_runner.h"
"shell/platform/linux/fl_text_input_plugin.cc"
"shell/platform/linux/fl_text_input_view_delegate.cc"
"shell/platform/linux/fl_texture.cc"
"shell/platform/linux/fl_texture_gl.cc"
"shell/platform/linux/fl_texture_registrar.cc"
"shell/platform/linux/fl_value.cc"
"shell/platform/linux/fl_view.cc"
"shell/platform/linux/fl_view_accessible.cc"
"shell/platform/linux/key_mapping.g.cc"
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED IMPORTED_TARGET gtk+-3.0)
pkg_check_modules(EPOXY REQUIRED IMPORTED_TARGET epoxy)
pkg_check_modules(X11 REQUIRED IMPORTED_TARGET x11)
pkg_check_modules(EGL REQUIRED IMPORTED_TARGET egl)

target_include_directories(${LIBRARY_NAME} PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../"
    "${CMAKE_CURRENT_LIST_DIR}/third_party/rapidjson/include"
)

target_link_libraries(${LIBRARY_NAME} PRIVATE
    ${FLUTTER_ENGINE_LIBRARY}
    PkgConfig::GTK
    PkgConfig::EPOXY
    PkgConfig::X11
    PkgConfig::EGL
)

target_compile_definitions(${LIBRARY_NAME} PRIVATE
    FLUTTER_LINUX_COMPILATION
    FLUTTER_ENGINE_NO_PROTOTYPES
)

target_compile_features(${LIBRARY_NAME} PUBLIC cxx_std_17)

install(
    TARGETS ${LIBRARY_NAME}
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    COMPONENT Runtime
)

install(
    FILES "${FLUTTER_ENGINE_LIBRARY}"
    DESTINATION "${CMAKE_INSTALL_PREFIX}"
    COMPONENT Runtime
)

install(
    DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/shell/platform/linux/public/flutter_linux"
    DESTINATION "${CMAKE_INSTALL_PREFIX}"
    COMPONENT Runtime
)