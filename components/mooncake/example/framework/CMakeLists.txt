# Framework examples

# Simplekv test
add_executable(simplekv_test ./simplekv_test.cpp)
target_link_libraries(simplekv_test ${PROJECT_NAME})

# App manager basic
add_executable(app_manager_basic ./app_manager_basic.cpp)
target_link_libraries(app_manager_basic ${PROJECT_NAME})

# App register test
add_executable(app_register_test ./app_register_test.cpp)
target_link_libraries(app_register_test ${PROJECT_NAME})

# Input device register basic
add_executable(input_device_register_test ./input_device_register_test.cpp)
target_link_libraries(input_device_register_test ${PROJECT_NAME})

# Mooncake test
add_executable(mooncake_framework_test ./mooncake_framework_test.cpp)
target_link_libraries(mooncake_framework_test ${PROJECT_NAME})

# App user data test
add_executable(app_user_data_test ./app_user_data_test.cpp)
target_link_libraries(app_user_data_test ${PROJECT_NAME})

