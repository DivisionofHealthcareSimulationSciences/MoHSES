#############################
# CMake - Rest Bridge Module - root
#############################

cmake_minimum_required(VERSION 3.18)
cmake_policy(SET CMP0095 NEW)

if( CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR )
        SET (MOHSES_BUILD_REST_ADAPTER ON)
else()
  option(MOHSES_BUILD_REST_ADAPTER "Toggle building of Mohses Module Manager" ON)
endif()

if (MOHSES_BUILD_REST_ADAPTER AND NOT WIN32)

    project("MOHSES Rest Adapter")

    set(PROJECT_VERSION_MAJOR 1)
    set(PROJECT_VERSION_MINOR 0)
    set(PROJECT_VERSION_PATCH 0)

    site_name(VERSION_HOST) # read hostname to VERSION_HOST

    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
    include(Packing)
    #############################
    # CMake Mod Manager root/src
    #############################

    file(GLOB_RECURSE SQLITE_SRCS LIST_DIRECTORIES OFF CONFIGURE_DEPENDS *.cpp)
    file(GLOB_RECURSE SQLITE_HDRS LIST_DIRECTORIES OFF CONFIGURE_DEPENDS *.h)
    set(MODULE_MANAGER_SOURCES src/RESTAdapterMain.cpp ${SQLITE_SRCS} ${SQLITE_HDRS})

    add_executable(mohses_rest_adapter ${MODULE_MANAGER_SOURCES})
    add_executable(Mohses::rest_adapater ALIAS mohses_rest_adapter)
        add_custom_command(TARGET mohses_rest_adapter POST_BUILD
                           COMMENT "Staging rest_adapter Config Files"
                           COMMAND ${CMAKE_COMMAND} -E copy_directory 
                                   ${CMAKE_CURRENT_SOURCE_DIR}/config/
                                   $<TARGET_FILE_DIR:mohses_rest_adapter>/config
                          ) 

    message(STATUS "Loading Mohses::rest_adapater")

    target_link_libraries(mohses_rest_adapter
            PUBLIC 
            mohses::net
            SQLite::SQLite3
            tinyxml2::tinyxml2
            eProsima::fastcdr
            eProsima::fastrtps
            rapidjson::headers
            pistache::lib
            Boost::headers
            Boost::program_options
            Boost::uuid
            Boost::process
            )

    set_target_properties(mohses_rest_adapter PROPERTIES 
                 FOLDER ${CMAKE_FOLDER}/modules 
                 CXX_STANDARD 20
                 INSTALL_RPATH "\$ORIGIN:\$ORIGIN/../lib:\$ORIGIN/lib"
    ) 

    target_include_directories(mohses_rest_adapter PRIVATE ${CONSTANT_INCLUDE_DIR})
    set(MODULE_TARGET_NAME mohses_rest_adapter)
    set(MODULE_ARGUMENTS " --configs=${MOHSES_RESOURCE_DIR}/config")
    install(TARGETS ${MODULE_TARGET_NAME} RUNTIME DESTINATION bin)
    if(NOT WIN32)
      configure_file(${CMAKE_SOURCE_DIR}/etc/supervisor/module.conf.in
                     ${CMAKE_CURRENT_BINARY_DIR}/${MODULE_TARGET_NAME}.conf @ONLY)
      install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${MODULE_TARGET_NAME}.conf 
              DESTINATION  ${MOHSES_SUPERVISOR_DIR})
    endif()
    install(DIRECTORY config
                DESTINATION  ${MOHSES_RESOURCE_DIR})

endif(MOHSES_BUILD_REST_ADAPTER  AND NOT WIN32)
