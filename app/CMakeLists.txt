# Sets the minimum version of CMake required to build your native library.
# This ensures that a certain set of CMake features is available to
# your build.

cmake_minimum_required(VERSION 3.4.1)

# Specifies a library name, specifies whether the library is STATIC or
# SHARED, and provides relative paths to the source code. You can
# define multiple libraries by adding multiple add_library() commands,
# and CMake builds them for you. When you build your app, Gradle
# automatically packages shared libraries with your APK.

add_library( # Specifies the name of the library.
        native-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        src/main/cpp/Base/native-lib.cpp
        src/main/cpp/Base/configuracion.cpp
        src/main/cpp/Base/Archivos.cpp
        src/main/cpp/Base/comunicaciones.cpp
        src/main/cpp/Base/Mensajeria.cpp
        src/main/cpp/Base/Utilidades.cpp
        src/main/cpp/Base/Inicializacion.cpp
        src/main/cpp/Base/Reportes.cpp
        src/main/cpp/Base/menu.cpp
        )

include_directories(src/main/cpp/include/)
include_directories(src/main/cpp/include/openssl)
add_library(
        netcom_Bancolombia
        STATIC
        src/main/cpp/Bancolombia/deposito.cpp
        src/main/cpp/Bancolombia/pagoProducto.cpp
        src/main/cpp/Bancolombia/utilidadesBancolombia.cpp
        src/main/cpp/Bancolombia/cierre_bancolombia.cpp
        src/main/cpp/Bancolombia/giros_bancolombia.cpp
        src/main/cpp/Bancolombia/retiro_bancolombia.cpp
        src/main/cpp/Bancolombia/Transferencia_bancolombia.cpp
        src/main/cpp/Bancolombia/recaudo_bancolombia.cpp
        src/main/cpp/Bancolombia/saldo_bancolombia.cpp
        src/main/cpp/Bancolombia/anulacion_bancolombia.cpp
        src/main/cpp/Bancolombia/duplicado_bancolombia.cpp
)

add_library(
        netcom_Caja
        STATIC
        src/main/cpp/Caja/Cajas.cpp
        src/main/cpp/Caja/CajaIP.cpp
        src/main/cpp/Caja/Mensajeria_caja.cpp
)

add_library(
        netcom_Venta
        STATIC
        src/main/cpp/Venta/cierre_venta.cpp
        src/main/cpp/Venta/duplicado_venta.cpp
        src/main/cpp/Venta/saldo_venta.cpp
        src/main/cpp/Venta/anulacion_venta.cpp
        src/main/cpp/Venta/manejo_impuestos.cpp
        src/main/cpp/Venta/consultas_informacion_costos.cpp
        src/main/cpp/Venta/core_venta.cpp
        src/main/cpp/Venta/dcc_proceso.cpp
        src/main/cpp/Venta/emv_venta.cpp
        src/main/cpp/Venta/qr_code_venta.cpp
        src/main/cpp/Venta/utilidades_qr_code.cpp
        src/main/cpp/Venta/dinero_electronico.cpp
        src/main/cpp/Venta/puntosColombia.cpp
)

add_library(
        netcom_Lealtad
        STATIC
        src/main/cpp/LifeMiles/lealtad.cpp
)
add_library(
        libcrypto STATIC IMPORTED
)
set_target_properties(libcrypto PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/libs/openssl-1.0.2/${ANDROID_ABI}/lib/libcrypto.a)
add_library(
        libssl STATIC IMPORTED
)

set_target_properties(
        libssl PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/libs/openssl-1.0.2/${ANDROID_ABI}/lib/libssl.a)
# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.
find_library(
        log-lib
        log
)

target_link_libraries(
        native-lib
        libssl
        libcrypto
        netcom_Bancolombia
        netcom_Venta
        netcom_Lealtad
        netcom_Caja
        ${log-lib}
)