# set(XMP_BUILD_STATIC SHARED)
# set(XMP_ENABLE_SECURE_SETTINGS "OFF")
# set(CMAKE_CL_64 ON)
# add_definitions(-DUNIX_ENV=1)
# add_subdirectory(XMP-Toolkit-SDK/build)
# message(status "xmp root ${XMP_ROOT}")

# set(PODOFO_BUILD_LIB_ONLY 1)
# set(PODOFO_BUILD_STATIC 1)
# set("FREETYPE_INCLUDE_DIR=${MINGW_PREFIX}/include/freetype2")
# add_subdirectory(podofo-0.9.6)


# project(djvu)
add_definitions( -DHAVE_STDINT_H -DHAS_MBSTATE -DHAVE_NAMESPACES)
file(GLOB DJVU_SRC_FILES djvulibre-3.5.28/libdjvu/*)
add_library(djvu STATIC  ${DJVU_SRC_FILES})
target_include_directories(djvu PUBLIC djvulibre-3.5.28/libdjvu )
# include(ExternalProject)
# ExternalProject_Add(
#     djvulibre
#     PATCH_COMMAND NOCONFIGURE=1 ./autogen.sh
#     # BUILD_IN_SOURCE 1
#     CONFIGURE_COMMAND COMMAND ${CFG_CMD} COMMAND ${PATCH_CMD}
#     BUILD_COMMAND "{make -j4}"
#     SUBDIRS_FIRST=libdjvu
#     # skip install
#     INSTALL_COMMAND ""
# )

# cmake_minimum_required(VERSION 3.5.1)


# # enable_language(C CXX)

# # assert_var_defined(CC)
# # assert_var_defined(CFLAGS)
# # assert_var_defined(CXX)
# # assert_var_defined(CXXFLAGS)
# # assert_var_defined(LDFLAGS)
# # assert_var_defined(LIBS)
# # assert_var_defined(CHOST)



# # set(CFG_ENV_VAR "CC=\"${CC}\" CXX=\"${CXX}\" CFLAGS=\"${CFLAGS}\" CXXFLAGS=\"${CMAKE_CXX_FLAGS}\"")
# set(CFG_OPTS "-q --disable-desktopfiles --disable-static --enable-shared --disable-xmltools --disable-largefile --without-jpeg --without-tiff -host=\"${CHOST}\"")
# set(CFG_CMD sh -c "${SOURCE_DIR}/configure ${CFG_OPTS}")


# set(PATCH_CMD sh -c "${ISED} \"s|-lstdc++||g\" libtool")


# #ko_write_gitclone_script(
#     GIT_CLONE_SCRIPT_FILENAME
#     https://gitlab.com/koreader/djvulibre.git
#     32ff0948c5bc27c0b7507dba90e726fc941fcc42
#     ${SOURCE_DIR}
# )

# include(ExternalProject)
# ExternalProject_Add(
#     ${PROJECT_NAME}
#     PATCH_COMMAND NOCONFIGURE=1 ./autogen.sh
#     BUILD_IN_SOURCE 1
#     CONFIGURE_COMMAND COMMAND ${CFG_CMD} COMMAND ${PATCH_CMD}
#     BUILD_COMMAND "{make -j4}"
#     SUBDIRS_FIRST=libdjvu
#     # skip install
#     INSTALL_COMMAND ""
# )
