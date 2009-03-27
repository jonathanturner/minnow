cmake_minimum_required(VERSION 2.6)

SET(CMAKE_BUILD_TYPE gdb)
SET(CMAKE_CXX_FLAGS_GDB " -Wall -ggdb -O3")
SET(CMAKE_C_FLAGS_GDB " -Wall -ggdb -O3")

INCLUDE_DIRECTORIES(aquarium)
SET(AQUARIUM_SRCS aquarium/message_queue.c aquarium/main.c aquarium/concurrency.c aquarium/scheduler.c)

SET (AQUARIUM_LIBS )
#Platform ident hackery
IF (WIN32)
  IF (MINGW)
  ENDIF (MINGW)

  #SET (AQUARIUM_SRCS ${AQUARIUM_SRCS} aquarium/Windows/Threading.cpp)
ENDIF (WIN32)

IF (UNIX) 
  IF (APPLE)
    IF (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    #SET (AQUARIUM_SRCS ${AQUARIUM_SRCS} aquarium/Mac/Threading.cpp)
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
    #SET (AQUARIUM_SRCS ${AQUARIUM_SRCS} aquarium/Linux/Threading.cpp)
    SET (AQUARIUM_LIBS ${AQUARIUM_LIBS} pthread)
  ENDIF (CMAKE_SYSTEM_NAME STREQUAL "Linux")
ENDIF (UNIX)

ADD_EXECUTABLE (aquarium_bin ${AQUARIUM_SRCS} )
#TARGET_LINK_LIBRARIES (aquarium ${AQUARIUM_LIBS} )
