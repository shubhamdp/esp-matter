list(APPEND srcs_list "attribute_get_val.cpp")
list(APPEND srcs_list "attribute_get_val_type.cpp")

idf_component_register(SRCS ${srcs_list}
                       INCLUDE_DIRS "."
                       REQUIRES unity esp_matter)
