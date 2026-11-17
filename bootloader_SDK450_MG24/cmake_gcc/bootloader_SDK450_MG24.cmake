####################################################################
# Automatically-generated file. Do not edit!                       #
####################################################################

set(SDK_PATH "C:/Users/Ryuu/Desktop/gecko-sdk")
set(COPIED_SDK_PATH "gecko_sdk_4.5.0")
set(PKG_PATH "C:/Users/Ryuu/.silabs/slt/installs")

add_library(slc OBJECT
    "${SDK_PATH}/platform/bootloader/core/btl_bootload.c"
    "${SDK_PATH}/platform/bootloader/core/btl_core.c"
    "${SDK_PATH}/platform/bootloader/core/btl_main.c"
    "${SDK_PATH}/platform/bootloader/core/btl_parse.c"
    "${SDK_PATH}/platform/bootloader/core/btl_reset.c"
    "${SDK_PATH}/platform/bootloader/core/flash/btl_internal_flash.c"
    "${SDK_PATH}/platform/bootloader/debug/btl_debug.c"
    "${SDK_PATH}/platform/bootloader/debug/btl_debug_swo.c"
    "${SDK_PATH}/platform/bootloader/driver/btl_driver_util.c"
    "${SDK_PATH}/platform/bootloader/parser/gbl/btl_gbl_custom_tags.c"
    "${SDK_PATH}/platform/bootloader/parser/gbl/btl_gbl_format.c"
    "${SDK_PATH}/platform/bootloader/parser/gbl/btl_gbl_parser.c"
    "${SDK_PATH}/platform/bootloader/security/btl_crc16.c"
    "${SDK_PATH}/platform/bootloader/security/btl_crc32.c"
    "${SDK_PATH}/platform/bootloader/security/btl_security_aes.c"
    "${SDK_PATH}/platform/bootloader/security/btl_security_ecdsa.c"
    "${SDK_PATH}/platform/bootloader/security/btl_security_sha256.c"
    "${SDK_PATH}/platform/bootloader/security/btl_security_tokens.c"
    "${SDK_PATH}/platform/bootloader/security/ecc/ecc.c"
    "${SDK_PATH}/platform/bootloader/security/sha/btl_sha256.c"
    "${SDK_PATH}/platform/bootloader/security/sha/crypto_sha.c"
    "${SDK_PATH}/platform/bootloader/security/sha/cryptoacc_sha.c"
    "${SDK_PATH}/platform/bootloader/security/sha/se_sha.c"
    "${SDK_PATH}/platform/bootloader/storage/bootloadinfo/btl_storage_bootloadinfo.c"
    "${SDK_PATH}/platform/bootloader/storage/btl_storage.c"
    "${SDK_PATH}/platform/bootloader/storage/btl_storage_library.c"
    "${SDK_PATH}/platform/bootloader/storage/internal_flash/btl_storage_internal_flash.c"
    "${SDK_PATH}/platform/common/src/sl_assert.c"
    "${SDK_PATH}/platform/common/src/sl_syscalls.c"
    "${SDK_PATH}/platform/common/toolchain/src/sl_memory.c"
    "${SDK_PATH}/platform/Device/SiliconLabs/EFR32MG24/Source/startup_efr32mg24.c"
    "${SDK_PATH}/platform/Device/SiliconLabs/EFR32MG24/Source/system_efr32mg24.c"
    "${SDK_PATH}/platform/emlib/src/em_acmp.c"
    "${SDK_PATH}/platform/emlib/src/em_burtc.c"
    "${SDK_PATH}/platform/emlib/src/em_cmu.c"
    "${SDK_PATH}/platform/emlib/src/em_core.c"
    "${SDK_PATH}/platform/emlib/src/em_dbg.c"
    "${SDK_PATH}/platform/emlib/src/em_emu.c"
    "${SDK_PATH}/platform/emlib/src/em_eusart.c"
    "${SDK_PATH}/platform/emlib/src/em_gpcrc.c"
    "${SDK_PATH}/platform/emlib/src/em_gpio.c"
    "${SDK_PATH}/platform/emlib/src/em_i2c.c"
    "${SDK_PATH}/platform/emlib/src/em_iadc.c"
    "${SDK_PATH}/platform/emlib/src/em_ldma.c"
    "${SDK_PATH}/platform/emlib/src/em_letimer.c"
    "${SDK_PATH}/platform/emlib/src/em_msc.c"
    "${SDK_PATH}/platform/emlib/src/em_opamp.c"
    "${SDK_PATH}/platform/emlib/src/em_pcnt.c"
    "${SDK_PATH}/platform/emlib/src/em_prs.c"
    "${SDK_PATH}/platform/emlib/src/em_rmu.c"
    "${SDK_PATH}/platform/emlib/src/em_se.c"
    "${SDK_PATH}/platform/emlib/src/em_system.c"
    "${SDK_PATH}/platform/emlib/src/em_timer.c"
    "${SDK_PATH}/platform/emlib/src/em_usart.c"
    "${SDK_PATH}/platform/emlib/src/em_vdac.c"
    "${SDK_PATH}/platform/emlib/src/em_wdog.c"
    "${SDK_PATH}/platform/security/sl_component/se_manager/src/sl_se_manager.c"
    "${SDK_PATH}/platform/security/sl_component/se_manager/src/sl_se_manager_attestation.c"
    "${SDK_PATH}/platform/security/sl_component/se_manager/src/sl_se_manager_cipher.c"
    "${SDK_PATH}/platform/security/sl_component/se_manager/src/sl_se_manager_entropy.c"
    "${SDK_PATH}/platform/security/sl_component/se_manager/src/sl_se_manager_hash.c"
    "${SDK_PATH}/platform/security/sl_component/se_manager/src/sl_se_manager_key_derivation.c"
    "${SDK_PATH}/platform/security/sl_component/se_manager/src/sl_se_manager_key_handling.c"
    "${SDK_PATH}/platform/security/sl_component/se_manager/src/sl_se_manager_signature.c"
    "${SDK_PATH}/platform/security/sl_component/se_manager/src/sl_se_manager_util.c"
    "${SDK_PATH}/platform/security/sl_component/sl_mbedtls_support/src/se_aes.c"
    "${SDK_PATH}/platform/security/sl_component/sl_mbedtls_support/src/sl_mbedtls.c"
    "${SDK_PATH}/platform/security/sl_component/sl_psa_driver/src/sli_psa_driver_common.c"
    "${SDK_PATH}/platform/security/sl_component/sl_psa_driver/src/sli_psa_driver_init.c"
    "${SDK_PATH}/platform/security/sl_component/sl_psa_driver/src/sli_se_driver_aead.c"
    "${SDK_PATH}/platform/security/sl_component/sl_psa_driver/src/sli_se_driver_builtin_keys.c"
    "${SDK_PATH}/platform/security/sl_component/sl_psa_driver/src/sli_se_driver_cipher.c"
    "${SDK_PATH}/platform/security/sl_component/sl_psa_driver/src/sli_se_driver_key_derivation.c"
    "${SDK_PATH}/platform/security/sl_component/sl_psa_driver/src/sli_se_driver_key_management.c"
    "${SDK_PATH}/platform/security/sl_component/sl_psa_driver/src/sli_se_driver_mac.c"
    "${SDK_PATH}/platform/security/sl_component/sl_psa_driver/src/sli_se_driver_signature.c"
    "${SDK_PATH}/platform/security/sl_component/sl_psa_driver/src/sli_se_opaque_driver_aead.c"
    "${SDK_PATH}/platform/security/sl_component/sl_psa_driver/src/sli_se_opaque_driver_cipher.c"
    "${SDK_PATH}/platform/security/sl_component/sl_psa_driver/src/sli_se_opaque_driver_mac.c"
    "${SDK_PATH}/platform/security/sl_component/sl_psa_driver/src/sli_se_opaque_key_derivation.c"
    "${SDK_PATH}/platform/security/sl_component/sl_psa_driver/src/sli_se_transparent_driver_aead.c"
    "${SDK_PATH}/platform/security/sl_component/sl_psa_driver/src/sli_se_transparent_driver_cipher.c"
    "${SDK_PATH}/platform/security/sl_component/sl_psa_driver/src/sli_se_transparent_driver_hash.c"
    "${SDK_PATH}/platform/security/sl_component/sl_psa_driver/src/sli_se_transparent_driver_mac.c"
    "${SDK_PATH}/platform/security/sl_component/sl_psa_driver/src/sli_se_transparent_key_derivation.c"
    "${SDK_PATH}/platform/security/sl_component/sl_psa_driver/src/sli_se_version_dependencies.c"
    "${SDK_PATH}/util/silicon_labs/silabs_core/memory_manager/sl_malloc.c"
    "${SDK_PATH}/util/third_party/mbedtls/library/aes.c"
    "${SDK_PATH}/util/third_party/mbedtls/library/constant_time.c"
    "${SDK_PATH}/util/third_party/mbedtls/library/platform.c"
    "${SDK_PATH}/util/third_party/mbedtls/library/platform_util.c"
    "${SDK_PATH}/util/third_party/mbedtls/library/psa_crypto_client.c"
    "${SDK_PATH}/util/third_party/mbedtls/library/psa_util.c"
    "${SDK_PATH}/util/third_party/mbedtls/library/threading.c"
    "${SDK_PATH}/util/third_party/trusted-firmware-m/lib/fih/src/fih.c"
    "${SDK_PATH}/util/third_party/trusted-firmware-m/platform/ext/target/siliconlabs/hse/sli_se.c"
)

target_include_directories(slc PUBLIC
   "../config"
   "../autogen"
    "${SDK_PATH}/platform/Device/SiliconLabs/EFR32MG24/Include"
    "${SDK_PATH}/platform/bootloader"
    "${SDK_PATH}/platform/bootloader/debug"
    "${SDK_PATH}/platform/bootloader/parser"
    "${SDK_PATH}/platform/bootloader/api"
    "${SDK_PATH}/platform/bootloader/core/flash"
    "${SDK_PATH}/platform/bootloader/security"
    "${SDK_PATH}/platform/CMSIS/Core/Include"
    "${SDK_PATH}/platform/emlib/inc"
    "${SDK_PATH}/platform/common/inc"
    "${SDK_PATH}/platform/security/sl_component/sl_mbedtls_support/config"
    "${SDK_PATH}/platform/security/sl_component/sl_mbedtls_support/config/preset"
    "${SDK_PATH}/platform/security/sl_component/sl_mbedtls_support/inc"
    "${SDK_PATH}/util/third_party/mbedtls/include"
    "${SDK_PATH}/util/third_party/mbedtls/library"
    "${SDK_PATH}/platform/security/sl_component/sl_psa_driver/inc"
    "${SDK_PATH}/platform/security/sl_component/se_manager/inc"
    "${SDK_PATH}/platform/security/sl_component/se_manager/src"
    "${SDK_PATH}/util/silicon_labs/silabs_core/memory_manager"
    "${SDK_PATH}/util/third_party/trusted-firmware-m/lib/fih/inc"
    "${SDK_PATH}/util/third_party/trusted-firmware-m/platform/include"
    "${SDK_PATH}/platform/common/toolchain/inc"
)

target_compile_definitions(slc PUBLIC
    "EFR32MG24B210F1536IM48=1"
    "SE_MANAGER_CONFIG_FILE=\"btl_aes_ctr_stream_block_cfg.h\""
    "BOOTLOADER_ENABLE=1"
    "BOOTLOADER_SECOND_STAGE=1"
    "SL_RAMFUNC_DISABLE=1"
    "__START=main"
    "__STARTUP_CLEAR_BSS=1"
    "BOOTLOADER_SUPPORT_INTERNAL_STORAGE=1"
    "BOOTLOADER_SUPPORT_STORAGE=1"
    "SL_BOARD_NAME=\"BRD4186C\""
    "SL_BOARD_REV=\"A01\""
    "HARDWARE_BOARD_DEFAULT_RF_BAND_2400=1"
    "HARDWARE_BOARD_SUPPORTS_1_RF_BAND=1"
    "HARDWARE_BOARD_SUPPORTS_RF_BAND_2400=1"
    "HFXO_FREQ=39000000"
    "SL_COMPONENT_CATALOG_PRESENT=1"
    "MBEDTLS_CONFIG_FILE=<sl_mbedtls_config.h>"
    "MBEDTLS_PSA_CRYPTO_CLIENT=1"
    "MBEDTLS_PSA_CRYPTO_CONFIG_FILE=<psa_crypto_config.h>"
    "SL_TRUSTZONE_SECURE=1"
)

target_link_libraries(slc PUBLIC
    "-Wl,--start-group"
    "gcc"
    "c"
    "m"
    "nosys"
    "-Wl,--end-group"
)
target_compile_options(slc PUBLIC
    $<$<COMPILE_LANGUAGE:C>:-mcpu=cortex-m33>
    $<$<COMPILE_LANGUAGE:C>:-mthumb>
    $<$<COMPILE_LANGUAGE:C>:-mfpu=fpv5-sp-d16>
    $<$<COMPILE_LANGUAGE:C>:-mfloat-abi=hard>
    $<$<COMPILE_LANGUAGE:C>:-Wall>
    $<$<COMPILE_LANGUAGE:C>:-Wextra>
    $<$<COMPILE_LANGUAGE:C>:-Os>
    $<$<COMPILE_LANGUAGE:C>:-fdata-sections>
    $<$<COMPILE_LANGUAGE:C>:-ffunction-sections>
    $<$<COMPILE_LANGUAGE:C>:-fomit-frame-pointer>
    "$<$<COMPILE_LANGUAGE:C>:SHELL:-imacros sl_gcc_preinclude.h>"
    $<$<COMPILE_LANGUAGE:C>:-mcmse>
    $<$<COMPILE_LANGUAGE:C>:-Wno-ignored-qualifiers>
    $<$<COMPILE_LANGUAGE:C>:-Wno-sign-compare>
    $<$<COMPILE_LANGUAGE:C>:--specs=nano.specs>
    $<$<COMPILE_LANGUAGE:C>:-g>
    $<$<COMPILE_LANGUAGE:CXX>:-mcpu=cortex-m33>
    $<$<COMPILE_LANGUAGE:CXX>:-mthumb>
    $<$<COMPILE_LANGUAGE:CXX>:-mfpu=fpv5-sp-d16>
    $<$<COMPILE_LANGUAGE:CXX>:-mfloat-abi=hard>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
    $<$<COMPILE_LANGUAGE:CXX>:-Wall>
    $<$<COMPILE_LANGUAGE:CXX>:-Wextra>
    $<$<COMPILE_LANGUAGE:CXX>:-Os>
    $<$<COMPILE_LANGUAGE:CXX>:-fdata-sections>
    $<$<COMPILE_LANGUAGE:CXX>:-ffunction-sections>
    $<$<COMPILE_LANGUAGE:CXX>:-fomit-frame-pointer>
    "$<$<COMPILE_LANGUAGE:CXX>:SHELL:-imacros sl_gcc_preinclude.h>"
    $<$<COMPILE_LANGUAGE:CXX>:-mcmse>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-ignored-qualifiers>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-sign-compare>
    $<$<COMPILE_LANGUAGE:CXX>:--specs=nano.specs>
    $<$<COMPILE_LANGUAGE:CXX>:-g>
    $<$<COMPILE_LANGUAGE:ASM>:-mcpu=cortex-m33>
    $<$<COMPILE_LANGUAGE:ASM>:-mthumb>
    $<$<COMPILE_LANGUAGE:ASM>:-mfpu=fpv5-sp-d16>
    $<$<COMPILE_LANGUAGE:ASM>:-mfloat-abi=hard>
    "$<$<COMPILE_LANGUAGE:ASM>:SHELL:-imacros sl_gcc_preinclude.h>"
    "$<$<COMPILE_LANGUAGE:ASM>:SHELL:-x assembler-with-cpp>"
)

set(post_build_command ${POST_BUILD_EXE} postbuild "./bootloader_SDK450_MG24.slpb" --parameter build_dir:"$<TARGET_FILE_DIR:bootloader_SDK450_MG24>")
set_property(TARGET slc PROPERTY C_STANDARD 99)
set_property(TARGET slc PROPERTY CXX_STANDARD 11)
set_property(TARGET slc PROPERTY CXX_EXTENSIONS OFF)

target_link_options(slc INTERFACE
    -mcpu=cortex-m33
    -mthumb
    -mfpu=fpv5-sp-d16
    -mfloat-abi=hard
    -T${CMAKE_CURRENT_LIST_DIR}/../autogen/linkerfile.ld
    --specs=nano.specs
    "SHELL:-Xlinker -Map=$<TARGET_FILE_DIR:bootloader_SDK450_MG24>/bootloader_SDK450_MG24.map"
    -Wl,--gc-sections
    -Wl,--no-warn-rwx-segments
)

# BEGIN_SIMPLICITY_STUDIO_METADATA=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END_SIMPLICITY_STUDIO_METADATA