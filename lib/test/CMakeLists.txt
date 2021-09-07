cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 11)

project(example_project)

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost REQUIRED)

find_package(Threads)
find_package(LibWaveform REQUIRED)



if(NOT USE_DYNAMIC)
   set(USE_DYNAMIC 0)
   message(STATUS "Dynamic loading disabled, enable with -DUSE_DYNAMIC=1")
    set(FLEX_LIB_INTERFACE "../flex_waveformlib_static_intf.cpp")
endif(NOT USE_DYNAMIC)
if(USE_DYNAMIC)
    add_compile_definitions(USE_DYNAMIC)
    message(STATUS "Dynamic loading enabled.")
    set(FLEX_LIB_INTERFACE "../flex_waveformlib_dl_intf.cpp")
endif(USE_DYNAMIC)

add_executable(mytest test.cpp ../FlexRadio6000.cpp ${FLEX_LIB_INTERFACE})#../flex_waveformlib_ptrdef.h)#../spsc_semaphore_buffer.h)

target_link_libraries(mytest PUBLIC Boost::boost 
                                    Threads::Threads 
                                    LibWaveform::waveform
                                    ${CMAKE_DL_LIBS})