cmake_minimum_required(VERSION 3.16)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_COMPILER avr-gcc)

project(embedded-access-control C)

set(MCU atmega644)

add_executable(${PROJECT_NAME}.elf src/main.c)

target_compile_options(${PROJECT_NAME}.elf PRIVATE -mmcu=${MCU} -Os -DF_CPU=1000000UL)
target_link_options(${PROJECT_NAME}.elf PRIVATE -mmcu=${MCU} -lm)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND avr-objcopy -O ihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
)