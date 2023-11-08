cmake_minimum_required(VERSION 3.18.0)

include(GNUInstallDirs)


if(NOT MOHSES_RUNTIME_DIR)
     set(MOHSES_RUNTIME_DIR "${CMAKE_INSTALL_PREFIX}/share/mohses")
  if(NOT WIN32)
     if(CMAKE_INSTALL_PREFIX MATCHES "^/usr(/.*)?$")
        set(MOHSES_RUNTIME_DIR "/opt/mohses/"   )
      endif()
  endif()
endif()
if(NOT MOHSES_LOGGING_DIR)
  set(MOHSES_LOGGING_DIR "${CMAKE_INSTALL_PREFIX}/logs")
  if(NOT WIN32)
     if(CMAKE_INSTALL_PREFIX MATCHES "^/usr(/.*)?$")
        set(MOHSES_LOGGING_DIR "/var/log/mohses")
      endif()
  endif()
endif()
if(NOT MOHSES_CONFIG_DIR)
  set(MOHSES_CONFIG_DIR  "${CMAKE_INSTALL_PREFIX}/share/mohses/supervisor/")
  if(NOT WIN32)
     if(CMAKE_INSTALL_PREFIX MATCHES "^/usr(/.*)?$")
        set(MOHSES_CONFIG_DIR "/etc/supervisor/conf.d/")
      endif()
  endif()
endif()
if(NOT MOHSES_USERNAME)
  set(MOHSES_USERNAME    "mohses")  
  if(NOT WIN32)
     if(CMAKE_INSTALL_PREFIX MATCHES "^/usr(/.*)?$")
        set(MOHSES_USERNAME    "mohses")   
      endif()
  endif()
endif()

set(_CMAKE_FOLDER ${CMAKE_FOLDER})
set(CMAKE_FOLDER Mohses)
add_subdirectory(module-manager)
add_subdirectory(rest-adapter)
add_subdirectory(simulation-manager)
add_subdirectory(tcp-bridge)
add_subdirectory(physiology-manager)
set(CMAKE_FOLDER ${_CMAKE_FOLDER})

find_package(Python QUIET)

#Add Triple Dection as needed
if ( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64"  AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
 set(triple "x86_64-linux-gnu")
elseif( CMAKE_SYSTEM_PROCESSOR STREQUAL "armv8" OR MAKE_SYSTEM_PROCESSOR STREQUAL "aarch64" )
 set(triple"aarch64-linux-gnu")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
 set(triple"unknown-linux-gnu")
else()
 set(triple"unknown-unknown-gnu")
endif()

string(REPLACE ";" "\;" pp "${CMAKE_PREFIX_PATH}")
if (TARGET create_distribution_bundle)
  ###############################################################################
  # Add modules to the dep list for our fixup target
  # You likely need to add on project to stage for every executable you would like
  # to stage (AKA move dep DLLs to the run folder for debugging.
  ################################################################################
 
  if(TARGET  mohses_module_manager ) 
    add_dependencies(create_distribution_bundle mohses_module_manager)
    if ( Python_FOUND)
      install(CODE 
      "
      message(STATUS \"Post Install Bundle Fixing\")
      execute_process(COMMAND
          ${Python_EXECUTABLE}
          ${CMAKE_SOURCE_DIR}/scripts/fixup_bundle.py
           $<$<TARGET_EXISTS:mohses_module_manager>:$<TARGET_FILE_DIR:mohses_module_manager>/$<TARGET_FILE_NAME:mohses_module_manager>>
          --hints \"${triple}\"
          --dirs \"${pp}\"
          --outdir \"${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}\"
          )
      "
      )
    endif()
  endif( )
  if( TARGET  mohses_physiology_manager )
    add_dependencies(create_distribution_bundle mohses_physiology_manager)
    if ( Python_FOUND)
      install(CODE 
      "
      message(STATUS \"Post Install Bundle Fixing\")
      execute_process(COMMAND
          ${Python_EXECUTABLE}
          ${CMAKE_SOURCE_DIR}/scripts/fixup_bundle.py
           $<$<TARGET_EXISTS:mohses_physiology_manager>:$<TARGET_FILE_DIR:mohses_physiology_manager>/$<TARGET_FILE_NAME:mohses_physiology_manager>>
          --hints \"${triple}\"
          --dirs \"${pp}\"
          --outdir \"${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}\"
          )
      "
      )
    endif()
  endif()
  if(TARGET mohses_rest_adapter )
    add_dependencies(create_distribution_bundle mohses_rest_adapter)
    if ( Python_FOUND)
      install(CODE 
      "
      message(STATUS \"Post Install Bundle Fixing\")
      execute_process(COMMAND
          ${Python_EXECUTABLE}
          ${CMAKE_SOURCE_DIR}/scripts/fixup_bundle.py
           $<$<TARGET_EXISTS:mohses_rest_adapter>:$<TARGET_FILE_DIR:mohses_rest_adapter>/$<TARGET_FILE_NAME:mohses_rest_adapter>>
          --hints \"${triple}\"
          --dirs \"${pp}\"
          --outdir \"${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}\"
          )
      "
      )
    endif()
  endif()
  if(TARGET mohses_simulation_manager )
    add_dependencies(create_distribution_bundle mohses_simulation_manager)
    if ( Python_FOUND)
      install(CODE 
      "
      message(STATUS \"Post Install Bundle Fixing\")
      execute_process(COMMAND
          ${Python_EXECUTABLE}
          ${CMAKE_SOURCE_DIR}/scripts/fixup_bundle.py
           $<$<TARGET_EXISTS:mohses_simulation_manager>:$<TARGET_FILE_DIR:mohses_simulation_manager>/$<TARGET_FILE_NAME:mohses_simulation_manager>>
          --hints \"${triple}\"
          --dirs \"${pp}\"
          --outdir \"${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}\"
          )
      "
      )
    endif()
  endif()
  if(TARGET  mohses_tcp_bridge)
    add_dependencies(create_distribution_bundle mohses_tcp_bridge)
    if ( Python_FOUND)
      install(CODE 
      "
      message(STATUS \"Post Install Bundle Fixing\")
      execute_process(COMMAND
          ${Python_EXECUTABLE}
          ${CMAKE_SOURCE_DIR}/scripts/fixup_bundle.py
           $<$<TARGET_EXISTS:mohses_tcp_bridge>:$<TARGET_FILE_DIR:mohses_tcp_bridge>/$<TARGET_FILE_NAME:mohses_tcp_bridge>>
          --hints \"${triple}\"
          --dirs \"${pp}\"
          --outdir \"${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}\"
          )
      "
      )
    endif()
  endif()
endif()
