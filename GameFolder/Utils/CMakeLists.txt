
set(SOURCES 
${CMAKE_CURRENT_SOURCE_DIR}/IOData.h
${CMAKE_CURRENT_SOURCE_DIR}/mathStuff.h
${CMAKE_CURRENT_SOURCE_DIR}/messages.h
)


target_sources(Game PRIVATE
${SOURCES}
)
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${SOURCES})
target_include_directories(Game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})