# *********************** START: FREERTOS ************************************

file(GLOB_RECURSE FREE_RTOS "Middlewares/Third_Party/FreeRTOS/Source/*.c")
file(GLOB_RECURSE FREE_RTOS1 "Middlewares/Third_Party/FreeRTOS/Source/*/*.c")
file(GLOB_RECURSE FREE_RTOS2 "Middlewares/Third_Party/FreeRTOS/Source/*/*/*.c")

add_library(FREERTOS ${FREE_RTOS} ${FREE_RTOS1} ${FREE_RTOS2})

include_directories(Middlewares/Third_Party/FreeRTOS/Source/include)
include_directories(Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS)
include_directories(Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3)

# *********************** END: FREERTOS ************************************