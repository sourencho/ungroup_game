add_executable(ug-test
    tests-main.cpp
    common/objects/tests-circle.cpp
    common/events/tests-EventController.cpp
)

target_link_libraries(ug-test PRIVATE common catch)
