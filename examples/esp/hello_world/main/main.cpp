// SPDX-FileCopyrightText: Copyright 2025 Clever Design (Switzerland) GmbH
// SPDX-License-Identifier: Apache-2.0

/**
 * @file
 * @brief
 *   TySettings simplest example: Hello World
 */

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <ty/logging.h>
#include <ty/platform/toolchain.h>
#include <tysettings/platform/settings.h>

#include <nvs_flash.h>

static const char *kLogModule = "HelloWorld";

/*****************************************************************************/
// just as a hint, but better not to pack, because this object only lives once
TY_TOOL_PACKED_BEGIN
struct AppPersistentSettings
{
    int a;
    int b;
} TY_TOOL_PACKED_END;

// keep the settings in global scope but not accessible (in C this would be static)
// the Settings will be injected in each module to support testing setups
namespace {
AppPersistentSettings mAppPersistentSettings = {1, 2};
}

extern "C" void app_main()
{
    tinyInstance *instance;
    tyLogInfo(kLogModule, "Starting TySettings example");
    // Initialize NVS

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
        tyLogWarn(kLogModule, "NVS flash erased");
    }

    instance = tinyInstanceInitSingle();
    // Initialize the settings subsystem
    tyPlatSettingsInit(instance, NULL, 0);
    tyPlatSettingsSet(instance, 1, (uint8_t *)&mAppPersistentSettings, sizeof(mAppPersistentSettings));
    while (true)
    {
        // next event in 1 second
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
    tinyInstanceFinalize(instance);
}
