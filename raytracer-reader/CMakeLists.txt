add_catch(test_raytracer_reader test.cpp)

if (TEST_SOLUTION)
    target_include_directories(test_raytracer_reader PUBLIC ../tests/raytracer-geom)
else()
    target_include_directories(test_raytracer_reader PUBLIC ../raytracer-geom)
endif()
