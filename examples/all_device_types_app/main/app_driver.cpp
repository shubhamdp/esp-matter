/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_err.h"
#include "esp_log.h"
#include "esp_matter.h"
#include <stdlib.h>
#include <string.h>

#include <app/clusters/fan-control-server/fan-control-delegate.h>
#include <app_priv.h>
#include <device.h>
#include <protocols/Protocols.h>
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S3
#include "fan_hal_bldc.h"
#include "fan_hal_stepper_motor.h"
#endif

using namespace chip::app::Clusters;
using namespace chip::app::Clusters::FanControl;
using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::cluster;
using namespace esp_matter::endpoint;

static const char *TAG = "app_driver";
extern uint16_t app_endpoint_id;

static bool s_fan_attr_update_in_progress = false;

static void get_attribute(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    attribute_t *attribute = attribute::get(endpoint_id, cluster_id, attribute_id);

    if (attribute == nullptr) {
        ESP_LOGE(TAG, "Failed to get attribute");
        return;
    }
    esp_err_t err = attribute::get_val(attribute, val);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get attribute value: %d", err);
        return;
    }
}

#include <app/reporting/reporting.h> // for MatterReportingAttributeChangeCallback

static esp_err_t set_attribute(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id,
                               esp_matter_attr_val_t val)
{
    esp_err_t err = attribute::set_val(endpoint_id, cluster_id, attribute_id, &val, false /* call_callbacks */);
    if (err == ESP_OK) {
        MatterReportingAttributeChangeCallback(endpoint_id, cluster_id, attribute_id);
        return ESP_OK;
    }
    if (err == ESP_ERR_NOT_FINISHED) {
        return ESP_OK;
    }
    ESP_LOGE(TAG, "Failed to set attribute 0x%" PRIx32 ": %d", attribute_id, err);
    return err;
}

#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S3
chip::Protocols::InteractionModel::Status FanDelegateImpl::HandleStep(
    chip::app::Clusters::FanControl::StepDirectionEnum aDirection, bool aWrap, bool aLowestOff)
{
    uint8_t lowest = 0;
    static int8_t max_speed = HIGH_MODE_PERCENT_MAX;
    int8_t tmp = 0;

    esp_matter_attr_val_t val = esp_matter_invalid(NULL);
    get_attribute(app_endpoint_id, FanControl::Id, FanControl::Attributes::PercentSetting::Id, &val);
    tmp = val.val.u8;

    ESP_LOGI(TAG, "Step received value: %d %d %d", (uint8_t)aDirection, aWrap, aLowestOff);
    /* Update percent-setting when speed is changed by the step command */
    if (aLowestOff)
        lowest = 0;
    else
        lowest = 1;
    if ((uint8_t)aDirection == 0) {
        tmp += 1;
        if (aWrap == 0) {
            tmp = tmp > max_speed ? max_speed : tmp;
        } else {
            tmp = tmp > max_speed ? lowest : tmp;
        }
    } else if ((uint8_t)aDirection == 1) {
        tmp -= 1;
        if (aWrap == 0) {
            tmp = tmp < lowest ? lowest : tmp;
        } else {
            tmp = tmp < lowest ? max_speed : tmp;
        }
    }
    val.val.u8 = tmp;
    attribute::update(app_endpoint_id, FanControl::Id, FanControl::Attributes::PercentSetting::Id, &val);

    return chip::Protocols::InteractionModel::Status::Success;
}

static esp_err_t app_driver_fan_set_mode(fan_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    ESP_LOGI(TAG, "Mode received value = %d ", val->val.i);
    hal_bldc_set_wind(0);
    if (val->val.i == 0) {
        hal_stepper_motor_set_rock(0);
    }
    hal_bldc_set_fanmode(val->val.i);

    return ESP_OK;
}

static esp_err_t app_driver_fan_set_percent(fan_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    hal_bldc_set_wind(0);
    ESP_LOGI(TAG, "Percent received value = %d ", val->val.i);
    hal_bldc_set_percent_speed(val->val.i);

    return ESP_OK;
}

static esp_err_t app_driver_fan_set_rock(fan_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    ESP_LOGI(TAG, "Shake received value = %d ", val->val.i);
    return hal_stepper_motor_set_rock(val->val.i);
}

static esp_err_t app_driver_fan_set_wind(fan_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    ESP_LOGI(TAG, "Wind received value = %d", val->val.i);
    hal_bldc_set_wind(val->val.i);
    /* When exit nature wind mode, set fan speed value to the pre-set percent-setting */
    if (val->val.i != 2) {
        esp_matter_attr_val_t val = esp_matter_invalid(NULL);
        get_attribute(app_endpoint_id, FanControl::Id, FanControl::Attributes::PercentSetting::Id, &val);
        attribute::update(app_endpoint_id, FanControl::Id, FanControl::Attributes::PercentSetting::Id, &val);
    }

    return ESP_OK;
}

static esp_err_t app_driver_fan_set_airflow_direction(fan_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    ESP_LOGI(TAG, "Airflow_Direction received value = %d", val->val.b);
    return hal_bldc_set_airflow_direction(val->val.b);
}

#else // !(CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S3)

static esp_err_t app_driver_fan_set_mode(fan_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    ESP_LOGE(TAG, "don't support");
    // intentionally return ESP_OK to avoid failing on attribute update
    return ESP_OK;
}

static esp_err_t app_driver_fan_set_percent(fan_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    ESP_LOGE(TAG, "don't support");
    // intentionally return ESP_OK to avoid failing on attribute update
    return ESP_OK;
}

static esp_err_t app_driver_fan_set_rock(fan_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    ESP_LOGE(TAG, "don't support");
    // intentionally return ESP_OK to avoid failing on attribute update
    return ESP_OK;
}

static esp_err_t app_driver_fan_set_wind(fan_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    ESP_LOGE(TAG, "don't support");
    // intentionally return ESP_OK to avoid failing on attribute update
    return ESP_OK;
}

static esp_err_t app_driver_fan_set_airflow_direction(fan_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    ESP_LOGE(TAG, "don't support");
    // intentionally return ESP_OK to avoid failing on attribute update
    return ESP_OK;
}
#endif // CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S3

static bool check_if_mode_percent_match(uint8_t fan_mode, uint8_t percent)
{
    switch (fan_mode) {
    case chip::to_underlying(FanModeEnum::kHigh): {
        if (percent < HIGH_MODE_PERCENT_MIN) {
            return false;
        }
        break;
    }
    case chip::to_underlying(FanModeEnum::kMedium): {
        if ((percent < MED_MODE_PERCENT_MIN) || (percent > MED_MODE_PERCENT_MAX)) {
            return false;
        }
        break;
    }
    case chip::to_underlying(FanModeEnum::kLow): {
        if ((percent < LOW_MODE_PERCENT_MIN) || (percent > LOW_MODE_PERCENT_MAX)) {
            return false;
        }
        break;
    }
    default:
        return false;
    }

    return true;
}

static bool is_fan_on(uint16_t endpoint_id)
{
    esp_matter_attr_val_t val = esp_matter_invalid(NULL);
    get_attribute(endpoint_id, OnOff::Id, OnOff::Attributes::OnOff::Id, &val);
    return val.val.b == true;
}

static bool supports_multi_speed(uint16_t endpoint_id)
{
    static int8_t cached = -1;
    if (cached < 0) {
        esp_matter_attr_val_t val = esp_matter_invalid(NULL);
        get_attribute(endpoint_id, FanControl::Id, Globals::Attributes::FeatureMap::Id, &val);
        cached = (val.val.u32 & chip::to_underlying(FanControl::Feature::kMultiSpeed)) ? 1 : 0;
    }
    return cached == 1;
}

esp_err_t app_driver_attribute_update(app_driver_handle_t driver_handle, uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    esp_err_t err = ESP_OK;

    if (endpoint_id == app_endpoint_id) {
        fan_driver_handle_t handle = (fan_driver_handle_t)driver_handle;

        if (cluster_id == FanControl::Id) {
            /* Guard against re-entrant calls: when we set_attribute on a related
               fan attribute below, the callback fires again for that attribute.
               Skip the cross-attribute sync on re-entry to prevent recursion. */
            if (s_fan_attr_update_in_progress) {
                return ESP_OK;
            }
            s_fan_attr_update_in_progress = true;

            if (attribute_id == FanControl::Attributes::FanMode::Id) {
                /* Off and Auto are fully handled by the core SDK's AttributeChangedCallback,
                   but that path may skip MarkDirty when values are unchanged. Force reports
                   here so subscriptions always see the updates. */
                if (val->val.u8 == chip::to_underlying(FanModeEnum::kOff)) {
                    esp_matter_attr_val_t zero_nullable = esp_matter_nullable_uint8(0);
                    esp_matter_attr_val_t zero = esp_matter_uint8(0);
                    set_attribute(endpoint_id, FanControl::Id, Attributes::PercentSetting::Id, zero_nullable);
                    set_attribute(endpoint_id, FanControl::Id, FanControl::Attributes::PercentCurrent::Id, zero);
                    if (supports_multi_speed(endpoint_id)) {
                        set_attribute(endpoint_id, FanControl::Id, Attributes::SpeedSetting::Id, zero_nullable);
                        set_attribute(endpoint_id, FanControl::Id, FanControl::Attributes::SpeedCurrent::Id, zero);
                    }
                } else if (val->val.u8 == chip::to_underlying(FanModeEnum::kAuto)) {
                    /* Auto: PercentSetting and SpeedSetting go null */
                } else {
                    esp_matter_attr_val_t percent_setting = esp_matter_invalid(NULL);
                    get_attribute(endpoint_id, FanControl::Id, Attributes::PercentSetting::Id, &percent_setting);
                    uint8_t percent = percent_setting.val.u8;
                    if (!check_if_mode_percent_match(val->val.u8, percent)) {
                        switch (val->val.u8) {
                        case chip::to_underlying(FanModeEnum::kHigh):
                            percent = HIGH_MODE_PERCENT_MAX;
                            break;
                        case chip::to_underlying(FanModeEnum::kMedium):
                            percent = MED_MODE_PERCENT_MAX;
                            break;
                        case chip::to_underlying(FanModeEnum::kLow):
                            percent = LOW_MODE_PERCENT_MAX;
                            break;
                        default:
                            break;
                        }
                        esp_matter_attr_val_t nullable_percent = esp_matter_nullable_uint8(percent);
                        set_attribute(endpoint_id, FanControl::Id, Attributes::PercentSetting::Id, nullable_percent);
                    }
                    if (is_fan_on(endpoint_id)) {
                        esp_matter_attr_val_t current_val = esp_matter_uint8(percent);
                        set_attribute(endpoint_id, FanControl::Id, FanControl::Attributes::PercentCurrent::Id,
                                      current_val);
                        if (supports_multi_speed(endpoint_id)) {
                            set_attribute(endpoint_id, FanControl::Id, FanControl::Attributes::SpeedCurrent::Id,
                                          current_val);
                        }
                    }
                }
                err = app_driver_fan_set_mode(handle, val);

            } else if (attribute_id == FanControl::Attributes::PercentSetting::Id) {
                uint8_t percent = val->val.u8;
                if (percent == 0) {
                    esp_matter_attr_val_t off_mode = esp_matter_enum8(chip::to_underlying(FanModeEnum::kOff));
                    set_attribute(endpoint_id, FanControl::Id, Attributes::FanMode::Id, off_mode);
                    esp_matter_attr_val_t zero = esp_matter_uint8(0);
                    set_attribute(endpoint_id, FanControl::Id, FanControl::Attributes::PercentCurrent::Id, zero);
                    if (supports_multi_speed(endpoint_id)) {
                        esp_matter_attr_val_t zero_nullable = esp_matter_nullable_uint8(0);
                        set_attribute(endpoint_id, FanControl::Id, Attributes::SpeedSetting::Id, zero_nullable);
                        set_attribute(endpoint_id, FanControl::Id, FanControl::Attributes::SpeedCurrent::Id, zero);
                    }
                } else {
                    esp_matter_attr_val_t fan_mode_val = esp_matter_invalid(NULL);
                    get_attribute(endpoint_id, FanControl::Id, Attributes::FanMode::Id, &fan_mode_val);
                    if (!check_if_mode_percent_match(fan_mode_val.val.u8, percent)) {
                        if (percent >= HIGH_MODE_PERCENT_MIN) {
                            esp_matter_attr_val_t new_mode = esp_matter_enum8(chip::to_underlying(FanModeEnum::kHigh));
                            set_attribute(endpoint_id, FanControl::Id, Attributes::FanMode::Id, new_mode);
                        } else if (percent >= MED_MODE_PERCENT_MIN) {
                            esp_matter_attr_val_t new_mode =
                                esp_matter_enum8(chip::to_underlying(FanModeEnum::kMedium));
                            set_attribute(endpoint_id, FanControl::Id, Attributes::FanMode::Id, new_mode);
                        } else if (percent >= LOW_MODE_PERCENT_MIN) {
                            esp_matter_attr_val_t new_mode = esp_matter_enum8(chip::to_underlying(FanModeEnum::kLow));
                            set_attribute(endpoint_id, FanControl::Id, Attributes::FanMode::Id, new_mode);
                        }
                    }
                    if (is_fan_on(endpoint_id)) {
                        esp_matter_attr_val_t current_val = esp_matter_uint8(percent);
                        set_attribute(endpoint_id, FanControl::Id, FanControl::Attributes::PercentCurrent::Id,
                                      current_val);
                        if (supports_multi_speed(endpoint_id)) {
                            set_attribute(endpoint_id, FanControl::Id, FanControl::Attributes::SpeedCurrent::Id,
                                          current_val);
                        }
                    }
                }
                err = app_driver_fan_set_percent(handle, val);
            } else if (attribute_id == FanControl::Attributes::RockSetting::Id) {
                err = app_driver_fan_set_rock(handle, val);
            } else if (attribute_id == FanControl::Attributes::WindSetting::Id) {
                err = app_driver_fan_set_wind(handle, val);
            } else if (attribute_id == FanControl::Attributes::AirflowDirection::Id) {
                err = app_driver_fan_set_airflow_direction(handle, val);
            }

            s_fan_attr_update_in_progress = false;
        } else if (cluster_id == OnOff::Id) {
            if (attribute_id == OnOff::Attributes::OnOff::Id) {
                if (val->val.b) {
                    esp_matter_attr_val_t ps = esp_matter_invalid(NULL);
                    get_attribute(endpoint_id, FanControl::Id, Attributes::PercentSetting::Id, &ps);
                    esp_matter_attr_val_t current_val = esp_matter_uint8(ps.val.u8);
                    set_attribute(endpoint_id, FanControl::Id, FanControl::Attributes::PercentCurrent::Id, current_val);
                    if (supports_multi_speed(endpoint_id)) {
                        set_attribute(endpoint_id, FanControl::Id, FanControl::Attributes::SpeedCurrent::Id,
                                      current_val);
                    }
                } else {
                    esp_matter_attr_val_t zero = esp_matter_uint8(0);
                    set_attribute(endpoint_id, FanControl::Id, FanControl::Attributes::PercentCurrent::Id, zero);
                    if (supports_multi_speed(endpoint_id)) {
                        set_attribute(endpoint_id, FanControl::Id, FanControl::Attributes::SpeedCurrent::Id, zero);
                    }
                }
            }
        }
    }
    return err;
}

#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S3
esp_err_t fan_driver_init()
{
    esp_err_t err = ESP_OK;
    /* initializing brushless motor */
    bldc_variable_init();
    err = hal_bldc_init(CW);

    /* initializing step motor */
    err = hal_stepper_motor_init((gpio_num_t)CONFIG_STEP_MOTOR_A0_GPIO, (gpio_num_t)CONFIG_STEP_MOTOR_A1_GPIO,
                                 (gpio_num_t)CONFIG_STEP_MOTOR_B0_GPIO, (gpio_num_t)CONFIG_STEP_MOTOR_B1_GPIO);

    return err;
}
#endif

esp_err_t app_driver_init()
{
    esp_err_t err = ESP_OK;
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S3
    /* initializing fan driver */
    fan_driver_init();
#endif

    return err;
}
