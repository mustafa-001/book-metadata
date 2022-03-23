add_executable(book-metadata main.cpp pdf.cpp djvu.cpp epub.cpp  book.h)

# SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
MESSAGE("cmake module path ${CMAKE_MODULE_PATH}")

set(LIBPODOFO_SHARED 1)
find_package(LIBPODOFO REQUIRED)
find_path(LIBXMLPP_INCLUDE_DIR libxml++-5.0/libxml++/document.h REQUIRED)
set(CMAKE_FIND_DEBUG_MODE off)
find_library(LIBXMLPP xml++-5.0.lib REQUIRED)
find_package(libzip  CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)

find_package(fmt CONFIG REQUIRED)

message("libxmlpp include dir "  ${LIBXMLPP_INCLUDE_DIR})
# message("djvu" ${DJVU_LIB}) 
add_definitions(-DHAVE_PTHREAD -DHAVE_STDINT_H -DHAS_MBSTATE -DHAVE_NAMESPACES)
target_link_libraries(book-metadata  djvu)

add_definitions(-D_UNICODE)
target_include_directories(book-metadata PUBLIC ${DJVU_INCLUDE_DIR} )

target_link_libraries(book-metadata  libzip::zip)

#HACK
include_directories(${LIBXMLPP_INCLUDE_DIR}/libxml++-5.0/)

target_include_directories(book-metadata PRIVATE ${LIBPODOFO_INCLUDE_DIR})
target_include_directories(book-metadata PRIVATE ${LIBXMLPP_INCLUDE_DIR})
target_link_libraries(book-metadata ${LIBPODOFO_LIBRARY})
target_compile_options(book-metadata PRIVATE "/utf-8")
target_link_libraries(book-metadata fmt::fmt)
target_link_libraries(book-metadata ${Boost_LIBRARIES} ${LIBXMLPP})