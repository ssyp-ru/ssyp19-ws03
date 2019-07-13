cmake_minimum_required (VERSION 3.12)

project ("EspectoProject")

include (CheckSymbolExists)
include (CheckCSourceCompiles)
include (GNUInstallDirs)


list (APPEND CMAKE_REQUIRED_DEFINITIONS
  -D_GNU_SOURCE
  -D_CRT_RAND_S
  -DVC_EXTRALEAN
  -DWIN32_LEAN_AND_MEAN)

check_symbol_exists (open_memstream stdio.h HAVE_OPEN_MEMSTREAM)
check_symbol_exists (fopencookie stdio.h HAVE_FOPENCOOKIE)
check_symbol_exists (funopen stdio.h HAVE_FUNOPEN)
check_symbol_exists (tmpfile stdio.h HAVE_TMPFILE)
check_symbol_exists (rand_s stdlib.h HAVE_WINAPI_RAND_S)
check_symbol_exists (CreateFile windows.h HAVE_WINAPI_CREATEFILE)
check_symbol_exists (CloseHandle windows.h HAVE_WINAPI_CLOSEHANDLE)
check_symbol_exists (GetFileSize windows.h HAVE_WINAPI_GETFILESIZE)
check_symbol_exists (CreateFileMapping windows.h HAVE_WINAPI_CREATEFILEMAPPING)
check_symbol_exists (MapViewOfFile windows.h HAVE_WINAPI_MAPVIEWOFFILE)
check_symbol_exists (UnmapViewOfFile windows.h HAVE_WINAPI_UNMAPVIEWOFFILE)
check_symbol_exists (GetTempPath windows.h HAVE_WINAPI_GETTEMPPATH)
check_symbol_exists (_open_osfhandle io.h HAVE_WINAPI_OPEN_OSFHANDLE)
check_symbol_exists (_get_osfhandle io.h HAVE_WINAPI_GET_OSFHANDLE)
check_symbol_exists (_fdopen stdio.h HAVE_WINAPI_FDOPEN)
check_symbol_exists (_fileno stdio.h HAVE_WINAPI_FILENO)
check_symbol_exists (_close io.h HAVE_WINAPI_CLOSE)

set (SOURCES)

if (HAVE_OPEN_MEMSTREAM)
  list (APPEND SOURCES fmem/fmem-open_memstream.c)
elseif (HAVE_FOPENCOOKIE)
  list (APPEND SOURCES
    fmem/alloc.c
    fmem/alloc.h
    fmem/fmem-fopencookie.c)
elseif (HAVE_FUNOPEN)
  list (APPEND SOURCES
    fmem/alloc.c
    fmem/alloc.h
    fmem/fmem-funopen.c)
elseif (HAVE_WINAPI_CREATEFILE
    AND HAVE_WINAPI_CLOSEHANDLE
    AND HAVE_WINAPI_GETFILESIZE
    AND HAVE_WINAPI_CREATEFILEMAPPING
    AND HAVE_WINAPI_MAPVIEWOFFILE
    AND HAVE_WINAPI_UNMAPVIEWOFFILE
    AND HAVE_WINAPI_GETTEMPPATH
    AND HAVE_WINAPI_FDOPEN
    AND HAVE_WINAPI_FILENO
    AND HAVE_WINAPI_CLOSE
    AND HAVE_WINAPI_OPEN_OSFHANDLE
    AND HAVE_WINAPI_GET_OSFHANDLE
    AND HAVE_WINAPI_RAND_S)
  list (APPEND SOURCES fmem/fmem-winapi-tmpfile.c)
elseif (HAVE_TMPFILE)
  list (APPEND SOURCES fmem/fmem-tmpfile.c)
else ()
  message (FATAL_ERROR "No memory stream implementation found")
endif ()


add_executable(EspectoProject
    ${SOURCES}
    bmp.cpp
    human-pack.cpp
    human-unpack.cpp
    main.cpp)

target_compile_features(EspectoProject PUBLIC cxx_std_17)
set_target_properties(EspectoProject PROPERTIES CXX_EXTENSIONS OFF)

include_directories (fmem)
