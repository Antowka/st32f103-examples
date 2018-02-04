cmake_minimum_required(VERSION 3.5)
project(blink C CXX ASM)

add_definitions(-DSTM32F103xB)

include("RTOS.cmake")
include("USB.cmake")

file(GLOB_RECURSE USER_SOURCES "Src/*.c")
file(GLOB_RECURSE HAL_SOURCES "Drivers/STM32F1xx_HAL_Driver/Src/*.c")
file(GLOB_RECURSE USERSOURCES "UserSrc/Src/*.c")
file(GLOB_RECURSE LIBS "UserSrc/Lib/*/Src/*.c")

add_library(HAL ${HAL_SOURCES})
add_library(CMSIS Src/system_stm32f1xx.c startup/startup_stm32f103xb.s)
add_library(LIBS ${LIBS})
add_library(USERSRC ${USERSOURCES})

include_directories(/opt/cross/arm-none-eabi/include)
include_directories(Inc)
include_directories(UserSrc/Lib/ssd1306/Inc)
include_directories(UserSrc/Lib/syscall/Inc)
include_directories(UserSrc/Lib/dh11/Inc)
include_directories(Drivers/STM32F1xx_HAL_Driver/Inc)
include_directories(Drivers/CMSIS/Include)
include_directories(Drivers/CMSIS/Device/ST/STM32F1xx/Include)
include_directories(UserSrc/Inc)


add_executable(${PROJECT_NAME}.elf ${USER_SOURCES} ${USB} ${FREERTOS} ${HAL_SOURCES} ${USERSOURCES} ${LIBS} ${LINKER_SCRIPT})

target_link_libraries(${PROJECT_NAME}.elf HAL CMSIS USERSRC LIBS FREERTOS USB)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.map")
set(HEX_FILE ${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE} \nBuilding ${BIN_FILE}")