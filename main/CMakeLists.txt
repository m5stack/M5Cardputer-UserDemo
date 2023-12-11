

file(GLOB_RECURSE APPS_SRCS
    ./apps/*.c
    ./apps/*.cpp
)

file(GLOB_RECURSE HAL_SRCS
    ./hal/*.c
    ./hal/*.cpp
)

set(PIKA_PYTHON_INCS
    ./apps/app_repl/pikapython/pikascript-api/
    ./apps/app_repl/pikapython/pikascript-core/
)



idf_component_register(SRCS "cardputer.cpp" ${APPS_SRCS} ${HAL_SRCS}
                    INCLUDE_DIRS "." "./hal" ${PIKA_PYTHON_INCS})


idf_component_get_property(tusb_lib leeebo__tinyusb_src COMPONENT_LIB)
cmake_policy(SET CMP0079 NEW)
target_link_libraries(${tusb_lib} PRIVATE ${COMPONENT_LIB})
