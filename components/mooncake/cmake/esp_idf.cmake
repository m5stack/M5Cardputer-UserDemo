# CMakeLists.txt of MOONCAKE component

# Src files
file(GLOB_RECURSE MOONCAKE_SRCS
    ${MOONCAKE_ROOT_DIR}/src/*.c
    ${MOONCAKE_ROOT_DIR}/src/*.cpp
)
# Include
set(MOONCAKE_INCS
    ${MOONCAKE_ROOT_DIR}/src/
    ${MOONCAKE_ROOT_DIR}/src/spdlog/include/
)

# Builtin apps
file(GLOB_RECURSE MOONCAKE_BUILTIN_APPS_SRCS
    ${MOONCAKE_ROOT_DIR}/apps/*.c
    ${MOONCAKE_ROOT_DIR}/apps/*.cpp
)
set(MOONCAKE_BUILTIN_APPS_INCS
    ${MOONCAKE_ROOT_DIR}/apps/
)

# Public component requirement
set(MOONCAKE_REQUIRES
)

# Private component requirement
set(MOONCAKE_PRIV_REQUIRES
)

# Register component
idf_component_register(
    SRCS ${MOONCAKE_SRCS} ${MOONCAKE_BUILTIN_APPS_SRCS}
    INCLUDE_DIRS ${MOONCAKE_INCS} ${MOONCAKE_BUILTIN_APPS_INCS}
    REQUIRES ${MOONCAKE_REQUIRES}
    PRIV_REQUIRES ${MOONCAKE_PRIV_REQUIRES}
)
