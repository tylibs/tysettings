// SPDX-FileCopyrightText: Copyright 2025 Clever Design (Switzerland) GmbH
// SPDX-License-Identifier: Apache-2.0

/**
 * @file
 * @brief
 *   TinySettings simplest example: Hello World
 */

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <tinyplatform/toolchain.h>
#include <tinysettings/tinysettings.hpp>

const char *TAG = "main";

/*****************************************************************************/

// just as a hint, but better not to pack, because this object only lives once
TB_TOOL_PACKED_BEGIN
struct AppPersistentSettings : TsSetting
{
    int a;
    int b;
} TB_TOOL_PACKED_END;

// keep the settings in global scope but not accessible (in C this would be static)
// the Settings will be injected in each module to support testing setups
namespace {
AppPersistentSettings mAppPersistentSettings;
}

extern "C" void app_main()
{
    ESP_LOGI(TAG, "Load settings");
    mAppPersistentSettings.magic = 0x1234;
    mAppPersistentSettings.len   = sizeof(mAppPersistentSettings);
    mAppPersistentSettings.a     = 1;

    tsLoad(&mAppPersistentSettings);

    while (true)
    {
        // next event in 1 second
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
