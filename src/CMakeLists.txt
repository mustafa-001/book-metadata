add_executable(book-metadata main.cpp pdf.cpp djvu.cpp epub.cpp  book.h)
# SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
MESSAGE("cmake module path ${CMAKE_MODULE_PATH}")

#vcpkg's libpodofo doesn't export proper libxmlpp-config.cmake module, when linking static
#(default on linux) dependencies doesnt added to build. This creates a TARGET and defines necessary 
#interface paramaters.
#TODO Move this to it's own FindPackage.cmake
set(LIBPODOFO_SHARED 0)
find_package(LIBPODOFO REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(JPEG REQUIRED)
find_package(freetype CONFIG REQUIRED)
add_library(podofo::podofo UNKNOWN IMPORTED)
set_target_properties(podofo::podofo PROPERTIES
IMPORTED_LOCATION ${LIBPODOFO_LIBRARY}
INTERFACE_INCLUDE_DIRECTORIES "${LIBPODOFO_INCLUDE_DIR}"
INTERFACE_LINK_LIBRARIES "OpenSSL::Crypto;${JPEG_LIBRARIES};freetype"
)

find_package(libzip CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)
find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_definitions(-DHAVE_PTHREAD -DHAVE_STDINT_H -DHAS_MBSTATE -DHAVE_NAMESPACES)
target_include_directories(book-metadata PUBLIC ${DJVU_INCLUDE_DIR} )


#vcpkg's libxmlpp doesn't export proper libxmlpp-config.cmake module, when linking static
#(default on linux) dependencies doesnt added to build. This creates a TARGET and defines necessary 
#interface paramaters.
#TODO Move this to it's own FindPackage.cmake
find_path(LIBXMLPP_INCLUDE_DIR libxml++-5.0/libxml++/document.h REQUIRED)
find_path(LIBXMLPP_CONFIG_H  libxml++config.h  PATH_SUFFIXES lib/libxml++-5.0/include REQUIRED NOCACHE)
find_library(LIBXMLPP xml++-5.0.lib libxml++-5.0.a REQUIRED)
#HACK
include_directories(${LIBXMLPP_INCLUDE_DIR}/libxml++-5.0/)
find_package(LibXml2 CONFIG REQUIRED)
add_library(LIBXMLPP::LIBXMLPP UNKNOWN IMPORTED)
set_target_properties(LIBXMLPP::LIBXMLPP PROPERTIES 
IMPORTED_LOCATION "${LIBXMLPP}"
INTERFACE_INCLUDE_DIRECTORIES "${LIBXMLPP_INCLUDE_DIR};${LIBXMLPP_CONFIG_H}"
INTERFACE_LINK_LIBRARIES  LibXml2::LibXml2) 

target_link_libraries(book-metadata PRIVATE LIBXMLPP::LIBXMLPP 
                                            spdlog::spdlog
                                            spdlog::spdlog_header_only
                                            libzip::zip
                                            djvu
                                            podofo::podofo 
                                            ${Boost_LIBRARIES} 
                                            fmt::fmt)