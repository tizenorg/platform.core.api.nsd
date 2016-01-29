SET(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)
INCLUDE_DIRECTORIES(${INC_DIR})

INCLUDE(FindPkgConfig)
pkg_check_modules(TARGET_NSD_TESTS REQUIRED capi-base-common glib-2.0)
FOREACH(flag ${TARGET_NSD_TESTS_CFLAGS})
    SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
    MESSAGE(${flag})
ENDFOREACH()

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -Wall -fPIE")
SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed -pie")

aux_source_directory(. sources)
FOREACH(src ${sources})
    GET_FILENAME_COMPONENT(src_name ${src} NAME_WE)
    MESSAGE("${src_name}")
    ADD_EXECUTABLE(${src_name} ${src})
    TARGET_LINK_LIBRARIES(${src_name} ${TARGET_LIB_NSD_DNS_SD} ${TARGET_NSD_TESTS_LDFLAGS})
ENDFOREACH()

INSTALL(TARGETS dns-sd-test RUNTIME DESTINATION ${BIN_DIR})
