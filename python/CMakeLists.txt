message("pht_ui: ${CMAKE_CURRENT_BINARY_DIR} ")
message("pht_ui: ${CMAKE_CURRENT_LIST_DIR} ")
set(PHT_UI_SOURCES
        ui.py
        )
list(TRANSFORM PHT_UI_SOURCES PREPEND "${CMAKE_CURRENT_LIST_DIR}/")
add_custom_target(pht_ui ALL SOURCES ${PHT_UI_SOURCES})
add_dependencies(pht_ui pht_lib)

add_custom_command(TARGET pht_ui POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/pht/Debug/pht_lib.dll          ${CMAKE_CURRENT_BINARY_DIR}/Debug/pht_lib.dll)
add_custom_command(TARGET pht_ui POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/python             ${CMAKE_CURRENT_BINARY_DIR}/Debug)