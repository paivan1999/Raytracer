add_catch(test_raytracer test.cpp)

if (TEST_SOLUTION)
    target_include_directories(test_raytracer PUBLIC ../tests/raytracer-geom)
    target_include_directories(test_raytracer PUBLIC ../tests/raytracer-reader)
else()
    target_include_directories(test_raytracer PUBLIC ../raytracer-geom)
    target_include_directories(test_raytracer PUBLIC ../raytracer-reader)
endif()

target_link_libraries(test_raytracer ${PNG_LIBRARY} ${JPEG_LIBRARIES})
target_include_directories(
    test_raytracer
    PRIVATE ${PNG_INCLUDE_DIRS} ${JPEG_INCLUDE_DIRS}
)
