cmake_minimum_required(VERSION 2.6)

SET (CMAKE_BUILD_TYPE gdb)
SET (CMAKE_C_FLAGS_GDB " -Wall -ggdb")
SET (CMAKE_CXX_FLAGS_GDB " -Wall -ggdb")

INCLUDE_DIRECTORIES (aquarium aqualisp)
SET (AQUARIUM_SRCS aquarium/actor.c aquarium/message.c aquarium/message_queue.c 
    aquarium/scheduler.c aquarium/work_queue.c)

SET (AQUARIUM_LIBS )

SET (AQUALISP_SRCS aqualisp/parser.cpp aqualisp/utils.cpp aqualisp/analyzer.cpp)

#Platform ident hackery
IF (WIN32)
  IF (MINGW)
  ENDIF (MINGW)

  SET (AQUARIUM_SRCS ${AQUARIUM_SRCS} aquarium/windows/concurrency.c)
ENDIF (WIN32)

IF (UNIX) 
  IF (APPLE)
    IF (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        SET (AQUARIUM_SRCS ${AQUARIUM_SRCS} aquarium/macosx/concurrency.c)

    ENDIF (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  ENDIF (APPLE)

  IF (CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    #SET (AQUARIUM_SRCS ${AQUARIUM_SRCS} aquarium/BSD/Threading.cpp)
    SET (AQUARIUM_LIBS ${AQUARIUM_LIBS} pthread)
  ENDIF (CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")

  IF (CMAKE_SYSTEM_NAME STREQUAL "SunOS")
    #SET (AQUARIUM_SRCS ${AQUARIUM_SRCS} aquarium/Solaris/Threading.cpp)
    SET (AQUARIUM_LIBS ${AQUARIUM_LIBS} pthread)
  ENDIF (CMAKE_SYSTEM_NAME STREQUAL "SunOS")

  IF (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    SET (AQUARIUM_SRCS ${AQUARIUM_SRCS} aquarium/linux/concurrency.c)
    SET (AQUARIUM_LIBS ${AQUARIUM_LIBS} pthread)

  ENDIF (CMAKE_SYSTEM_NAME STREQUAL "Linux")
ENDIF (UNIX)

ADD_LIBRARY (aquarium ${AQUARIUM_SRCS} )
TARGET_LINK_LIBRARIES (aquarium ${AQUARIUM_LIBS} )

ADD_EXECUTABLE(bin/test_steal_wait aquarium/test_steal_wait.c)
TARGET_LINK_LIBRARIES(bin/test_steal_wait aquarium)

ADD_EXECUTABLE(bin/test_threadring aquarium/test_threadring.c)
TARGET_LINK_LIBRARIES(bin/test_threadring aquarium)

ADD_EXECUTABLE(bin/test_big_bang aquarium/test_big_bang.c)
TARGET_LINK_LIBRARIES(bin/test_big_bang aquarium)

ADD_EXECUTABLE(bin/test_ali_parser ${AQUALISP_SRCS} aqualisp/test_ali_parser.cpp)

ADD_EXECUTABLE(bin/test_ali_analyzer ${AQUALISP_SRCS} aqualisp/test_ali_analyzer.cpp)
