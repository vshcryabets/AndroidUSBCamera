cmake_minimum_required(VERSION 3.22)

if(BUILD_AUDIO_SOURCES_MODULE)
        ## portaudio
        ## brew install portaudio
        ## sudo apt install libportaudio2 portaudio19-dev
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(PortAudio REQUIRED portaudio-2.0)

        # message("PortAudio_CFLAGS: ${PortAudio_CFLAGS}")
        # message("PortAudio_LIBRARIES: ${PortAudio_LIBRARIES}")
        # message("PortAudio_LIBRARY_DIRS: ${PortAudio_LIBRARY_DIRS}")

        list(APPEND SOURCES 
                src/AudioSource.cpp
                )

        add_library(AudioSources STATIC
                ${SOURCES}
                )
        target_link_directories(AudioSources PRIVATE ${PortAudio_LIBRARY_DIRS})
        target_compile_options(AudioSources PRIVATE ${PortAudio_CFLAGS})
        target_link_libraries(AudioSources PRIVATE ${PortAudio_LIBRARIES})
        target_include_directories(AudioSources PUBLIC
                $<INSTALL_INTERFACE:include>
                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                )
            
endif()                