cmake_minimum_required(VERSION 3.16)
project(stm32_timer_capture_instrument C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(ENABLE_STRICT_WARNINGS "Treat compiler warnings as errors" ON)

add_library(pwm_measurement src/pwm_measurement.c)
target_include_directories(pwm_measurement PUBLIC include)

add_executable(test_pwm_measurement tests/test_pwm_measurement.c)
target_link_libraries(test_pwm_measurement PRIVATE pwm_measurement)

if(ENABLE_STRICT_WARNINGS)
    if(MSVC)
        target_compile_options(pwm_measurement PRIVATE /W4 /WX)
        target_compile_options(test_pwm_measurement PRIVATE /W4 /WX)
    else()
        target_compile_options(pwm_measurement PRIVATE -Wall -Wextra -Wpedantic -Werror)
        target_compile_options(test_pwm_measurement PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
endif()

enable_testing()
add_test(NAME pwm_measurement_tests COMMAND test_pwm_measurement)
