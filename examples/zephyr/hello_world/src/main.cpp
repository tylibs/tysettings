// SPDX-FileCopyrightText: Copyright 2025 Clever Design (Switzerland) GmbH
// SPDX-License-Identifier: Apache-2.0

/**
 * @file
 * @brief
 *   TinySettings simplest example: Hello World
 */

#include <tiny/logging.h>
#include <tiny/platform/toolchain.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "tinysettings/platform/settings.h"

#define LOG_MODULE_NAME main
LOG_MODULE_REGISTER(main);
const char *TAG = "main";

/*****************************************************************************/
// just as a hint, but better not to pack, because this object only lives once
TINY_TOOL_PACKED_BEGIN
struct AppPersistentSettings
{
    int a;
    int b;
} TINY_TOOL_PACKED_END;

// keep the settings in global scope but not accessible (in C this would be static)
// the Settings will be injected in each module to support testing setups
namespace {
AppPersistentSettings mAppPersistentSettings = {1, 2};
}

extern "C" int main(void)
{
    tinyInstance *instance;
    tinyLogInfoPlat("Starting TinySettings example");

    instance = tinyInstanceInitSingle();
    // Initialize the settings subsystem
    tinyPlatSettingsInit(instance, NULL, 0);
    tinyPlatSettingsSet(instance, 1, (uint8_t *)&mAppPersistentSettings, sizeof(mAppPersistentSettings));
    while (true)
    {
        // next event in 1 second
        k_sleep(K_SECONDS(1));
    }
    tinyInstanceFinalize(instance);
    return 0;
}
