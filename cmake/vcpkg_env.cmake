# configure the vcpkg toolchain file
macro(set_vcpkg_config)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
    set(VCPKG_MANIFEST_MODE ON CACHE BOOL "Manifest mode")  
    set(VCPKG_MANIFEST_DIR ${CMAKE_SOURCE_DIR} CACHE PATH "Manifest directory")
    if(NOT DEFINED local_VCPKG_TARGET_TRIPLET)
        if(NOT DEFINED VCPKG_TARGET_TRIPLET)
            if(WIN32)
                set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "Vcpkg target triplet")
            else()
                set(VCPKG_TARGET_TRIPLET "x64-linux" CACHE STRING "Vcpkg target triplet")
            endif()
        endif()
        set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "Vcpkg target triplet")
    else()
        set(VCPKG_TARGET_TRIPLET "${local_VCPKG_TARGET_TRIPLET}" CACHE STRING "Vcpkg target triplet")
    endif()
    set(VCPKG_MANIFEST_INSTALL ON CACHE BOOL "Enable manifest install")
    set(VCPKG_APPLOCAL_DEPS ON CACHE BOOL "Enable applocal deps")
    set(VCPKG_INSTALLED_DIR ${CMAKE_BINARY_DIR}/vcpkg_installed)
endmacro(set_vcpkg_config)

# load VCPKG_ROOT from the .vcpkg-root file
if(EXISTS "${CMAKE_SOURCE_DIR}/.vcpkg-target-triplet")
    file(STRINGS "${CMAKE_SOURCE_DIR}/.vcpkg-target-triplet" local_VCPKG_TARGET_TRIPLET)
    string(REPLACE "\n" "" VCPKG_TARGET_TRIPLET "${local_VCPKG_TARGET_TRIPLET}")
    message(STATUS "VCPKG_TARGET_TRIPLET found in the .vcpkg-target-triplet file")
endif()

# load VCPKG_ROOT from the environment
if(DEFINED ENV{VCPKG_ROOT})
    set_vcpkg_config()
    message(STATUS "VCPKG_ROOT found in the environment")
else()
    # load VCPKG_ROOT from the .vcpkg-root file
    if(EXISTS "${CMAKE_SOURCE_DIR}/.vcpkg-root")
        file(STRINGS "${CMAKE_SOURCE_DIR}/.vcpkg-root" VCPKG_ROOT)
        if(NOT EXISTS "${VCPKG_ROOT}")
            message(FATAL_ERROR "VCPKG_ROOT not found in the .vcpkg-root file")
        endif()
        set(ENV{VCPKG_ROOT} "${VCPKG_ROOT}")
        set_vcpkg_config()

        message(STATUS "VCPKG_ROOT found in the .vcpkg-root file")
    else()
        message(WARNING "VCPKG_ROOT not found in the environment or .vcpkg-root file
            Please set VCPKG_ROOT in the environment or create a .vcpkg-root file
            with the path to the vcpkg installation directory.
            For example, C:/Users/Public/vcpkg")
    endif()
endif()
