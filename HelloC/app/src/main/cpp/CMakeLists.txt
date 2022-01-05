# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)

# Declares and names the project.

project("helloc")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

#CMakeLists 경로 매크로
set(DIR_CURR ./)
set(DIR_GAME ./game/)
set(DIR_GLS ./gls/)
set(DIR_MAIN ./main/)
set(DIR_STD ./std/)

add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             native-lib.cpp #App.cpp 대신

             #src/main/cpp/lib/iStd.cpp 절대경로 안됨
             ${DIR_GAME}DrawCounters.cpp
             ${DIR_GAME}Intro.cpp
             ${DIR_GAME}LoadMap.cpp
             ${DIR_GAME}Menu.cpp
             ${DIR_GAME}Proc.cpp
             ${DIR_GAME}ProcUI_Commander.cpp
             ${DIR_GAME}ProcUI_MapKey.cpp
             ${DIR_GAME}ProcUI_MiniMap.cpp
             ${DIR_GAME}ProcUI_RegimentInfo.cpp
             ${DIR_GAME}RegimentAction.cpp
             ${DIR_GAME}RegimentMgr.cpp
             ${DIR_GAME}TextGroup.cpp

             ${DIR_GAME}Test.cpp #Testing

             ${DIR_GLS}iColor.cpp
             ${DIR_GLS}iFBO.cpp
             ${DIR_GLS}iGraphics_Android.cpp
             ${DIR_GLS}iGraphics_Windows.cpp
             ${DIR_GLS}iImage.cpp
             ${DIR_GLS}iMatrix.cpp
             ${DIR_GLS}iOpenGL.cpp
             ${DIR_GLS}iPage.cpp
             ${DIR_GLS}iParticleSystem.cpp
             ${DIR_GLS}iPopup.cpp
             ${DIR_GLS}iStrtex.cpp

             ${DIR_MAIN}Game.cpp
             ${DIR_MAIN}iControl.cpp
             ${DIR_MAIN}iPopWindow.cpp
             ${DIR_MAIN}iVolume.cpp
             ${DIR_MAIN}Loading.cpp
             ${DIR_MAIN}Sound.cpp

             ${DIR_STD}iArray.cpp
             ${DIR_STD}iFPS.cpp
             ${DIR_STD}iNDK.cpp
             ${DIR_STD}iOpenAL.cpp
             ${DIR_STD}iPoint.cpp
             ${DIR_STD}iRect.cpp
             ${DIR_STD}iSize.cpp
             ${DIR_STD}iStd.cpp
             ${DIR_STD}iString.cpp
        )

# Specifies a path to native header files.
#include_directories(src/main/cpp/lib/) 절대경로 안됨
#include_directories(util/) 구분자;를 사용해서 경로 추가
include_directories(${DIR_CURR};
                    ${DIR_GAME};
                    ${DIR_GLS};
                    ${DIR_MAIN}
                    ${DIR_STD};)
#add_subdirectory()
#CMakeLists 내부 매크로
add_definitions(-DAAA="GAME" -DBBB=333 -O2) #gradle이 최종

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       native-lib

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}   #log
                       android
                       EGL          # activity + opengl bridge
                       GLESv1_CM
                       GLESv2)      # version 2

# *.lib
#pragma comment(lib, "opengl.lib"); // c

