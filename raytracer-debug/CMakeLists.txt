add_catch(test_raytracer_debug test.cpp)

if (TEST_SOLUTION)
    target_include_directories(test_raytracer_debug PUBLIC ../tests/raytracer-geom)
    target_include_directories(test_raytracer_debug PUBLIC ../tests/raytracer-reader)
    target_include_directories(test_raytracer_debug PUBLIC ../tests/raytracer)
else()
    target_include_directories(test_raytracer_debug PUBLIC ../raytracer-geom)
    target_include_directories(test_raytracer_debug PUBLIC ../raytracer-reader)
endif()
target_include_directories(test_raytracer_debug PUBLIC ../raytracer)

target_link_libraries(test_raytracer_debug ${PNG_LIBRARY} ${JPEG_LIBRARIES})
target_include_directories(
    test_raytracer_debug
    PRIVATE ${PNG_INCLUDE_DIRS} ${JPEG_INCLUDE_DIRS}
)
