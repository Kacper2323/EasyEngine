set(SOURCES
${CMAKE_CURRENT_SOURCE_DIR}/PlayScene.cpp
${CMAKE_CURRENT_SOURCE_DIR}/platformerLevelEdit.cpp
${CMAKE_CURRENT_SOURCE_DIR}/PlayScene.h
${CMAKE_CURRENT_SOURCE_DIR}/platformerLevelEdit.h
)


target_sources(Game PRIVATE 
${SOURCES}
)
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${SOURCES})

target_include_directories(Game PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
