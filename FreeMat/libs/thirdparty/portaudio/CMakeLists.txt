INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/src/common)
IF (APPLE)
   ADD_DEFINITIONS(-DPA_USE_COREAUDIO)
   ADD_LIBRARY(portaudio
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_allocation.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_converters.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_cpuload.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_debugprint.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_dither.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_front.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_process.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_ringbuffer.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_stream.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_trace.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/hostapi/coreaudio/pa_mac_core.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/hostapi/coreaudio/pa_mac_core_blocking.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/hostapi/coreaudio/pa_mac_core_utilities.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/os/mac_osx/pa_mac_hostapis.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/os/unix/pa_unix_util.c )
ENDIF (APPLE)

IF (WIN32)
   ADD_DEFINITIONS(-DPA_USE_WDMKSDEVICE_INFO)


   INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/src/os/win)
   ADD_LIBRARY(portaudio
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_allocation.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_converters.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_cpuload.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_debugprint.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_dither.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_front.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_process.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_stream.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_trace.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/hostapi/wmme/pa_win_wmme.c
   #${CMAKE_CURRENT_SOURCE_DIR}/src/hostapi/wasapi/pa_win_wasapi.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/src/os/win/pa_win_hostapis.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/os/win/pa_win_util.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/os/win/pa_win_waveformat.c
   #${CMAKE_CURRENT_SOURCE_DIR}/src/os/win/pa_win_wdmks_utils.c
   #${CMAKE_CURRENT_SOURCE_DIR}/src/os/win/pa_x86_plain_converters.c
   )
ENDIF (WIN32)

IF (NOT WIN32 AND NOT APPLE)
   FIND_FILE(HAVE_SYS_SOUNDCARD_H sys/soundcard.h)
   FIND_FILE(HAVE_LINUX_SOUNDCARD_H linux/soundcard.h)
   FIND_FILE(HAVE_MACHINE_SOUNDCARD_H machine/soundcard.h)
   INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/src/os/unix)
   MARK_AS_ADVANCED(HAVE_SYS_SOUNDCARD_H HAVE_LINUX_SOUNDCARD_H HAVE_MACHINE_SOUNDCARD_H)
   IF (HAVE_SYS_SOUNDCARD_H)
     ADD_DEFINITIONS(-DHAVE_SYS_SOUNDCARD_H)
   ENDIF (HAVE_SYS_SOUNDCARD_H)
   IF (HAVE_LINUX_SOUNDCARD_H)
     ADD_DEFINITIONS(-DHAVE_LINUX_SOUNDCARD_H)
   ENDIF (HAVE_LINUX_SOUNDCARD_H)
   IF (HAVE_MACHINE_SOUNDCARD_H)
     ADD_DEFINITIONS(-DHAVE_MACHINE_SOUNDCARD_H)
   ENDIF (HAVE_MACHINE_SOUNDCARD_H)
      ADD_DEFINITIONS(-DPA_USE_OSS)
   ADD_LIBRARY(portaudio
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_allocation.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_converters.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_cpuload.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_debugprint.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_dither.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_front.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_process.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_ringbuffer.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_stream.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pa_trace.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/hostapi/oss/pa_unix_oss.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/os/unix/pa_unix_hostapis.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/os/unix/pa_unix_util.c )
ENDIF (NOT WIN32 AND NOT APPLE)
