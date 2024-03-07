idf_component_register(SRCS "app_driver.cpp"
                            "app_main.cpp"
                            "${ESP_MATTER_PATH}/examples/all_device_types_app/main/esp_matter_console_helpers.cpp"
                            "${ESP_MATTER_PATH}/examples/all_device_types_app/main/nvs_helpers.cpp"
                            PRIV_INCLUDE_DIRS  "."
                                               "${ESP_MATTER_PATH}/examples/common/utils"
                                               "${ESP_MATTER_PATH}/examples/all_device_types_app/main")

set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 17)
target_compile_options(${COMPONENT_LIB} PRIVATE "-DCHIP_HAVE_CONFIG_H")
