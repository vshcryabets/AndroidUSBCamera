cmake_minimum_required(VERSION 3.22)

if(BUILD_IMAGES_MODULE)
        add_library(Images STATIC
                src/ImageUseCases.cpp
                )

        target_include_directories(Images PUBLIC
                $<INSTALL_INTERFACE:include>
                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                )

        if (BUILD_CATCH2_TESTS)
                add_executable(images_tests
                        test/test_ImageUseCases.cpp
                        )
                target_include_directories(images_tests PRIVATE ./include/)
                target_link_libraries(images_tests PRIVATE Catch2::Catch2WithMain Images)
        endif()
endif()                