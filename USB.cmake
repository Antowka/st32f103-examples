# *********************** START: FREERTOS ************************************

file(GLOB_RECURSE USB_CLASS "Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/*.c")
file(GLOB_RECURSE USB_SRC "Middlewares/ST/STM32_USB_Device_Library/Core/Src/*.c")

add_library(USB ${USB_CLASS} ${USB_SRC})

include_directories(Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc)
include_directories(Middlewares/ST/STM32_USB_Device_Library/Core/Inc)

# *********************** END: FREERTOS ************************************