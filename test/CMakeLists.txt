add_executable(mytest
    mytest.c
    ${CMAKE_SOURCE_DIR}/src/loop.c
    ${CMAKE_SOURCE_DIR}/src/threads.c
)

target_compile_definitions(mytest PRIVATE UNIT_TEST)

target_link_libraries(mytest PRIVATE
    pico_stdlib
    FreeRTOS-Kernel
    pico_async_context_freertos
    FreeRTOS-Kernel-Heap4
    Unity
)


add_library(Unity STATIC
    ${CMAKE_SOURCE_DIR}/lib/unity/src/unity.c
)

target_include_directories(Unity PUBLIC
    ${CMAKE_SOURCE_DIR}/lib/unity/src
)



# Update the target_include_directories for mytest
target_include_directories(mytest PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/lib/unity/src
)

pico_add_extra_outputs(mytest)

# The CYW43 is the Wifi/Bluetooth module. If the board is set to pico_w, this
# variable will be set and the wireless libraries added.
if(PICO_CYW43_SUPPORTED)
    target_link_libraries(mytest PRIVATE
        pico_cyw43_arch_none
    )
endif()

# Modify the below lines to enable/disable output over UART/USB.
# These are mutually exclusive, only set one.
pico_enable_stdio_uart(mytest 0)
pico_enable_stdio_usb(mytest 1)

# This will speed up the simulation, since data won't be copied from the flash controller.
# It does make the binary non-persistent on a real microcontroller.
# pico_set_binary_type(test no_flash)

find_program(RENODE renode
# This is extra stuff so it can find the mac installation paths.
 NAMES macos_run.command Renode.exe
 PATHS /Applications/Renode.app/Contents/MacOS)

set(RENODE_FLAGS
  --disable-xwt
  --port -2
  --pid-file renode.pid
  --console
  )

add_test(NAME simulate_mytest COMMAND
    ${RENODE}
     ${RENODE_FLAGS}
    -e "$ELF=@$<TARGET_FILE:mytest>; $WORKING=@${CMAKE_SOURCE_DIR}; include @${CMAKE_SOURCE_DIR}/test/simulate.resc; start"
    )

add_custom_target(flash_test picotool load -f $<TARGET_FILE:mytest> DEPENDS mytest)
