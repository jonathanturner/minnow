cmake_minimum_required(VERSION 2.6)

SET(CMAKE_BUILD_TYPE gdb)
SET(CMAKE_C_FLAGS_GDB " -Wall -ggdb")

INCLUDE_DIRECTORIES(aquarium libcoroutine)
SET(AQUARIUM_SRCS aquarium/actor.c aquarium/message.c aquarium/message_queue.c aquarium/main.c 
    aquarium/scheduler.c aquarium/work_queue.c)

SET (AQUARIUM_LIBS )
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

ADD_EXECUTABLE (aquarium_bin ${AQUARIUM_SRCS} )
TARGET_LINK_LIBRARIES (aquarium_bin ${AQUARIUM_LIBS} )
