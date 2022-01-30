set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(User CXX)

# locate mailio
if (WIN32)
    set(MAILIO_INCLUDE_DIR ${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/include)
    set(MAILIO_LIB ${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/lib/mailio.lib)
elseif (UNIX)
    set(MAILIO_INCLUDE_DIR ${CMAKE_BINARY_DIR}/vcpkg_installed/x64-linux/include)
    set(MAILIO_LIB ${CMAKE_BINARY_DIR}/vcpkg_installed/x64-linux/lib/libmailio.a)
endif ()

# Include boost lib if compiler is gcc
#if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
#    find_package(OpenSSL REQUIRED)
#    target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::SSL OpenSSL::Crypto)
#
#    set(Boost_USE_STATIC_LIBS ON)
#    set(Boost_USE_MULTITHREADED ON)
#    find_package(Boost 1.75 REQUIRED regex date_time)
#    if (Boost_FOUND)
#        target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS})
#        target_link_libraries(${PROJECT_NAME} PUBLIC ${Boost_LIBRARIES})
#    endif ()
#endif ()

add_executable(${PROJECT_NAME} main.cc)

aux_source_directory(controllers CTL_SRC)
aux_source_directory(filters FILTER_SRC)
aux_source_directory(models MODEL_SRC)
aux_source_directory(plugins PLUGIN_SRC)
aux_source_directory(structures STRUCT_SRC)
aux_source_directory(services SVC_SRC)
aux_source_directory(utils UTIL_SRC)

target_link_libraries(${PROJECT_NAME} PRIVATE
        ${MAILIO_LIB}
        Drogon::Drogon
        ${CPPREDIS_LIB}
        ${TACOPIE_LIB}
        Shared::Shared)

drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
        ${CMAKE_CURRENT_BINARY_DIR})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${MAILIO_INCLUDE_DIR})
target_sources(${PROJECT_NAME} PRIVATE
        ${CTL_SRC}
        ${FILTER_SRC}
        ${MODEL_SRC}
        ${PLUGIN_SRC}
        ${STRUCT_SRC}
        ${SVC_SRC}
        ${UTIL_SRC})

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/config.json DESTINATION ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/verifyEmail.html DESTINATION ${CMAKE_BINARY_DIR}/${PROJECT_NAME})