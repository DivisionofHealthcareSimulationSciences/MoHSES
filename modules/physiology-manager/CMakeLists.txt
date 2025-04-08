#############################
# CMake Mod Manager root/
#############################

cmake_minimum_required(VERSION 3.18)
cmake_policy(SET CMP0095 NEW)

if( CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR )
        SET (MOHSES_BUILD_physiology_manager ON)
else()
  option(MOHSES_BUILD_physiology_manager "Toggle building of Mohses Module Manager" ON)
endif()

if (MOHSES_BUILD_physiology_manager)
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
        find_package(TinyXML2 REQUIRED)

        set(physiology_manager_SOURCES src/PhysiologyManager.cpp src/AMM/PhysiologyEngineManager.cpp src/AMM/BiogearsThread.cpp)

        add_executable(mohses_physiology_manager ${physiology_manager_SOURCES})
        add_executable(Mohses::physiology_manager ALIAS mohses_physiology_manager)
        add_custom_command(TARGET mohses_physiology_manager POST_BUILD
                           COMMENT "Staging physiology_manager Config Files"
                           COMMAND ${CMAKE_COMMAND} -E copy_directory 
                                   ${CMAKE_CURRENT_SOURCE_DIR}/config/
                                   $<TARGET_FILE_DIR:mohses_physiology_manager>/config
                          ) 

        message(STATUS "Loading Mohses::physiology_manager")

        target_link_libraries(mohses_physiology_manager
                PUBLIC
                Threads::Threads
                Biogears::libbiogears
                eProsima::fastrtps
                eProsima::fastcdr
                mohses::net
                SQLite::SQLite3
                tinyxml2::tinyxml2
                Boost::headers
                Boost::program_options
                Boost::assign
                Boost::uuid
                )

        target_include_directories(mohses_physiology_manager PRIVATE ${CONSTANT_INCLUDE_DIR})
        set_target_properties(mohses_physiology_manager PROPERTIES 
                 FOLDER ${CMAKE_FOLDER}/modules 
                 CXX_STANDARD 20
                 INSTALL_RPATH "\$ORIGIN:\$ORIGIN/../lib:\$ORIGIN/lib"
        ) 

        set(MODULE_TARGET_NAME mohses_physiology_manager )
        set(MODULE_ARGUMENTS "-a --configs=${MOHSES_RESOURCE_DIR}/config" --resources=${MOHSES_RESOURCE_DIR} )
        install(TARGETS ${MODULE_TARGET_NAME} RUNTIME DESTINATION bin)
        if(NOT WIN32)
          configure_file(${CMAKE_SOURCE_DIR}/etc/supervisor/module.conf.in
                         ${CMAKE_CURRENT_BINARY_DIR}/${MODULE_TARGET_NAME}.conf @ONLY)
          install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${MODULE_TARGET_NAME}.conf 
                   DESTINATION ${MOHSES_SUPERVISOR_DIR})
        else()
           target_compile_definitions(${MODULE_TARGET_NAME} PRIVATE BOOST_UUID_FORCE_AUTO_LINK )
        endif()
        install(DIRECTORY config
                DESTINATION  ${MOHSES_RESOURCE_DIR})

endif(MOHSES_BUILD_physiology_manager)
