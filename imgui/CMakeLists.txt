set(SOURCES
${CMAKE_CURRENT_SOURCE_DIR}/imgui_demo.cpp
${CMAKE_CURRENT_SOURCE_DIR}/imgui_tables.cpp
${CMAKE_CURRENT_SOURCE_DIR}/imgui_widgets.cpp
${CMAKE_CURRENT_SOURCE_DIR}/imgui_draw.cpp
${CMAKE_CURRENT_SOURCE_DIR}/rlImGui.cpp
${CMAKE_CURRENT_SOURCE_DIR}/imgui.cpp
${CMAKE_CURRENT_SOURCE_DIR}/imconfig.h
${CMAKE_CURRENT_SOURCE_DIR}/imgui_impl_raylib.h
${CMAKE_CURRENT_SOURCE_DIR}/imgui_internal.h
${CMAKE_CURRENT_SOURCE_DIR}/imgui.h
${CMAKE_CURRENT_SOURCE_DIR}/imstb_rectpack.h
${CMAKE_CURRENT_SOURCE_DIR}/imstb_textedit.h
${CMAKE_CURRENT_SOURCE_DIR}/imstb_truetype.h
${CMAKE_CURRENT_SOURCE_DIR}/rlImGui.h
${CMAKE_CURRENT_SOURCE_DIR}/rlImGuiColors.h
)



target_sources(Game PRIVATE
${SOURCES}
)
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${SOURCES})

target_include_directories(Game PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})