include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
FetchContent_MakeAvailable(googletest)

add_executable(test_bilinear test_bilinear.cpp)
add_executable(test_polygon_intersection test_polygon_intersection.cpp)
add_executable(test_levelset2d_bilinear_measure
    test_bilinear.cpp
    test_polygon_intersection.cpp
)

foreach(target test_bilinear test_polygon_intersection test_levelset2d_bilinear_measure)
    target_link_libraries(${target}
        PRIVATE
            levelset2d_bilinear_measure
            GTest::gtest_main
    )
    add_test(NAME ${target} COMMAND ${target})
endforeach()
