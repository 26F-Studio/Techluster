project(Shared CXX)

find_package(cryptopp CONFIG REQUIRED)

add_library(${PROJECT_NAME} STATIC)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

aux_source_directory(plugins PLUGIN_SRC)
aux_source_directory(utils UTIL_SRC)
target_sources(${PROJECT_NAME} PRIVATE
        ${PLUGIN_SRC}
        ${UTIL_SRC})

target_link_libraries(${PROJECT_NAME} PRIVATE
        cryptopp-static
        Drogon::Drogon)

target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        PRIVATE source)
