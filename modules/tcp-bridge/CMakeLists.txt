#############################
# CMake - TCP Bridge Module - root
#############################

cmake_minimum_required(VERSION 3.18.0)
cmake_policy(SET CMP0095 NEW)

#############################
# CMake - TCP BRIDGE - root
#############################

if( CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR )
        SET (MOHSES_BUILD_TCP_BRIDGE ON)
else()
  option(MOHSES_BUILD_TCP_BRIDGE "Toggle building of Mohses Module Manager" ON)
endif()

if (MOHSES_BUILD_TCP_BRIDGE)

    project("MOHSES Rest Adapter" CXX)

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
    find_package(TinyXML2 REQUIRED)

    set(TCP_BRIDGE_MODULE_SOURCES
        src/TCPBridgeMain.cpp
        src/Net/Client.cpp
        src/Net/Server.cpp
        src/Net/ServerThread.cpp
        src/Net/UdpDiscoveryServer.cpp
        src/Manikin.cpp
        src/TPMS.cpp
        src/bridge.cpp)


    add_executable(mohses_tcp_bridge ${TCP_BRIDGE_MODULE_SOURCES})
    add_executable(Mohses::tcp_bridge ALIAS mohses_tcp_bridge)
    add_custom_command(TARGET mohses_tcp_bridge POST_BUILD
                           COMMENT "Staging tcp_bridge Config Files"
                           COMMAND ${CMAKE_COMMAND} -E copy_directory 
                                   ${CMAKE_CURRENT_SOURCE_DIR}/config/
                                   $<TARGET_FILE_DIR:mohses_tcp_bridge>/config
                          ) 

    message(STATUS "Loading Mohses::tcp_bridge")

    find_package(Threads)
    target_compile_definitions(mohses_tcp_bridge PRIVATE  BOOST_BIND_GLOBAL_PLACEHOLDERS)
    target_link_libraries(mohses_tcp_bridge
            PUBLIC 
            mohses::net
            tinyxml2::tinyxml2
            Boost::system
            Boost::thread
            Boost::program_options
            Boost::filesystem
            Boost::process
            Boost::uuid
            Boost::assign
            Boost::foreach
            Boost::asio
            Boost::headers
            Threads::Threads
            )
    set_target_properties(mohses_tcp_bridge PROPERTIES 
                 FOLDER ${CMAKE_FOLDER}/modules 
                 INSTALL_RPATH "\$ORIGIN:\$ORIGIN/../lib:\$ORIGIN/lib"
                 CXX_STANDARD 17
    ) 

    target_include_directories(mohses_tcp_bridge PRIVATE ${CONSTANT_INCLUDE_DIR})

    set(MODULE_TARGET_NAME mohses_tcp_bridge ) 
    set(MODULE_ARGUMENTS " --configs=${MOHSES_RESOURCE_DIR}/config")
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


endif(MOHSES_BUILD_TCP_BRIDGE)
