#############################
# CMake Mod Manager root/
#############################

cmake_minimum_required(VERSION 3.18)
cmake_policy(SET CMP0095 NEW)

if( CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR )
        SET (MOHSES_BUILD_MODULE_MANAGER ON)
else()
  option(MOHSES_BUILD_MODULE_MANAGER "Toggle building of Mohses Module Manager" ON)
endif()

if (MOHSES_BUILD_MODULE_MANAGER)
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

        set(MODULE_MANAGER_SOURCES src/ModuleManagerMain.cpp src/ModuleManager.cpp)

        add_executable(mohses_module_manager ${MODULE_MANAGER_SOURCES})
        add_executable(Mohses::module_manager ALIAS mohses_module_manager)
        add_custom_command(TARGET mohses_module_manager POST_BUILD
                           COMMENT "Staging module_manager Config Files"
                           COMMAND ${CMAKE_COMMAND} -E copy_directory 
                                   ${CMAKE_CURRENT_SOURCE_DIR}/config/
                                   $<TARGET_FILE_DIR:mohses_module_manager>/config
                          ) 
        message(STATUS "Loading Mohses::module_manager")

        target_link_libraries(mohses_module_manager
                PUBLIC 
                mohses::net
                SQLite::SQLite3
                tinyxml2::tinyxml2
                Boost::headers                        
                $<TARGET_NAME_IF_EXISTS:Boost::dynamic_linking>
                $<TARGET_NAME_IF_EXISTS:Boost::disable_autolinking>
                Boost::program_options
                Boost::uuid
                )
 
        set_target_properties(mohses_module_manager PROPERTIES 
                 FOLDER ${CMAKE_FOLDER}/modules
                 CXX_STANDARD 20
                 INSTALL_RPATH "\$ORIGIN:\$ORIGIN/../lib:\$ORIGIN/lib"
         ) 

 target_include_directories(mohses_module_manager PRIVATE ${CONSTANT_INCLUDE_DIR})

        set(MODULE_TARGET_NAME mohses_module_manager)
        set(MODULE_ARGUMENTS "-s -a --configs=${MOHSES_RESOURCE_DIR}/config")
        install(TARGETS ${MODULE_TARGET_NAME} RUNTIME DESTINATION bin)
        if(NOT WIN32)
          configure_file(${CMAKE_SOURCE_DIR}/etc/supervisor/module.conf.in
                         ${CMAKE_CURRENT_BINARY_DIR}/${MODULE_TARGET_NAME}.conf @ONLY)
          install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${MODULE_TARGET_NAME}.conf 
                  DESTINATION  ${MOHSES_SUPERVISOR_DIR})
        else()
           target_compile_definitions(${MODULE_TARGET_NAME} PRIVATE BOOST_UUID_FORCE_AUTO_LINK )
        endif()
        install(DIRECTORY config
                DESTINATION  ${MOHSES_RESOURCE_DIR})
        install(DIRECTORY static/scenarios
                DESTINATION  ${MOHSES_RESOURCE_DIR})

endif(MOHSES_BUILD_MODULE_MANAGER)
