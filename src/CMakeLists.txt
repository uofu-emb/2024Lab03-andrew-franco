# This is the main binary. List your C files here.
add_executable(threads
    threads.c
    loop.c
)

target_link_libraries(threads PRIVATE
    pico_stdlib
    pico_multicore
    pico_async_context_freertos
    FreeRTOS-Kernel-Heap4
    FreeRTOS-Kernel
)


pico_enable_stdio_uart(threads 0)
pico_enable_stdio_usb(threads 1)

if(PICO_CYW43_SUPPORTED)
    target_link_libraries(threads PRIVATE
        pico_cyw43_arch_none
    )
endif()

pico_add_extra_outputs(threads)

add_custom_target(flash picotool load -f $<TARGET_FILE:threads> DEPENDS threads)