add_executable(${ProjectName}
    main.c
    tasks.c
    tasks.h
    queues.c
    queues.h
)

target_include_directories(${ProjectName} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(${ProjectName} 
    pico_stdlib 
    hardware_i2c
    hardware_uart
    FreeRTOS-Kernel-Heap4 
    )

pico_add_extra_outputs(${ProjectName})