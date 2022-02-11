set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(Shared CXX)

add_compile_definitions(CMAKE_PROJECT_NAME=std::string\("${PROJECT_NAME}"\))

find_package(cryptopp CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)

add_library(${PROJECT_NAME} STATIC)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

aux_source_directory(controllers CTL_SRC)
aux_source_directory(filters FILTER_SRC)
aux_source_directory(helpers HELPER_SRC)
aux_source_directory(plugins PLUGIN_SRC)
aux_source_directory(structures STRUCT_SRC)
aux_source_directory(services SVC_SRC)
aux_source_directory(types TYPE_SRC)
aux_source_directory(utils UTIL_SRC)

target_sources(${PROJECT_NAME} PRIVATE
        ${CTL_SRC}
        ${FILTER_SRC}
        ${PLUGIN_SRC}
        ${STRUCT_SRC}
        ${SVC_SRC}
        ${TYPE_SRC}
        ${UTIL_SRC}
        ${HELPER_SRC})

target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        cryptopp-static
        ${MAILIO_LIB}
        PUBLIC
        ${CPPREDIS_LIB}
        Drogon::Drogon
        magic_enum::magic_enum
        ${TACOPIE_LIB}
)

target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        PRIVATE
        ${MAILIO_INCLUDE_DIR}
)
