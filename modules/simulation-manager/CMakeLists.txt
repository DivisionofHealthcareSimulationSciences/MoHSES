#############################
# CMake Mod Manager root/
#############################

cmake_minimum_required(VERSION 3.18)
cmake_policy(SET CMP0095 NEW)

if( CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR )
        SET (MOHSES_BUILD_SIMULATION_MANAGER ON)
else()
  option(MOHSES_BUILD_SIMULATION_MANAGER "Toggle building of Mohses Module Manager" ON)
endif()

if (MOHSES_BUILD_SIMULATION_MANAGER)
        project("MOSHES Module Manager")

        set(PROJECT_VERSION_MAJOR 1)
        set(PROJECT_VERSION_MINOR 0)
        set(PROJECT_VERSION_PATCH 0)

        site_name(VERSION_HOST) # read hostname to VERSION_HOST
        
        set_property(GLOBAL PROPERTY USE_FOLDERS ON)
        set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

        #############################
        # CMake Mod Manager root/src
        #############################

        
        set(SIMULATION_MANAGER_SOURCES src/SimManager.cpp src/SimulationManager.cpp)


        add_executable(mohses_simulation_manager ${SIMULATION_MANAGER_SOURCES})
        add_executable(Mohses::simulation_manager ALIAS mohses_simulation_manager)
        add_custom_command(TARGET mohses_simulation_manager POST_BUILD
                           COMMENT "Staging simulation_manager Config Files"
                           COMMAND ${CMAKE_COMMAND} -E copy_directory 
                                   ${CMAKE_CURRENT_SOURCE_DIR}/config/
                                   $<TARGET_FILE_DIR:mohses_simulation_manager>/config
                          ) 

        message(STATUS "Loading Mohses::simulation_manager")

        target_link_libraries(mohses_simulation_manager
                PUBLIC 
                mohses::net
                SQLite::SQLite3
                tinyxml2::tinyxml2
                Boost::headers
                Boost::program_options
                Boost::uuid
                )
        set_target_properties(mohses_simulation_manager PROPERTIES 
                 FOLDER ${CMAKE_FOLDER}/modules
                 INSTALL_RPATH "\$ORIGIN:\$ORIGIN/../lib:\$ORIGIN/lib"
                 CXX_STANDARD 20
        ) 

        target_include_directories(mohses_simulation_manager  PRIVATE ${CONSTANT_INCLUDE_DIR})

        set(MODULE_TARGET_NAME mohses_simulation_manager )
        set(MODULE_ARGUMENTS " --configs=${MOHSES_RESOURCE_DIR}/config" )
        install(TARGETS ${MODULE_TARGET_NAME} RUNTIME DESTINATION bin)
        if(NOT WIN32)
          configure_file(${CMAKE_SOURCE_DIR}/etc/supervisor/module.conf.in
                         ${CMAKE_CURRENT_BINARY_DIR}/${MODULE_TARGET_NAME}.conf @ONLY)
          install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${MODULE_TARGET_NAME}.conf 
                  DESTINATION  ${MOHSES_SUPERVISOR_DIR})
          install(DIRECTORY config
                          DESTINATION  ${MOHSES_RESOURCE_DIR})
        else()
           target_compile_definitions(${MODULE_TARGET_NAME} PRIVATE BOOST_UUID_FORCE_AUTO_LINK )
        endif()

endif(MOHSES_BUILD_SIMULATION_MANAGER)
