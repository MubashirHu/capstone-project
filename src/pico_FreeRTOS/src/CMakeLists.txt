add_executable(${ProjectName}
    main.c
    tasks.c
    tasks.h
    queues.c
    queues.h
    util.c
    util.h
    interrupt.c
    interrupt.h
)

target_include_directories(${ProjectName} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

pico_enable_stdio_usb(${ProjectName} 1)
pico_enable_stdio_uart(${ProjectName} 0)

target_link_libraries(${ProjectName} 
    pico_stdlib 
    hardware_i2c
    hardware_uart
    hardware_gpio
    FreeRTOS-Kernel-Heap4
    )

pico_add_extra_outputs(${ProjectName})