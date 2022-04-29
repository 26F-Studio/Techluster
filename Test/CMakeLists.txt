set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(Test CXX)

add_compile_definitions(CMAKE_PROJECT_NAME="${PROJECT_NAME}")

add_executable(${PROJECT_NAME} main.cc)

aux_source_directory(plugins PLUGIN_SRC)
aux_source_directory(structures STRUCT_SRC)
aux_source_directory(utils UTIL_SRC)

target_link_libraries(${PROJECT_NAME} PRIVATE
        Drogon::Drogon
        Shared::Shared)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_sources(${PROJECT_NAME} PRIVATE
        ${PLUGIN_SRC}
        ${STRUCT_SRC}
        ${UTIL_SRC})

ParseAndAddDrogonTests(${PROJECT_NAME})

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/config.json DESTINATION ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
