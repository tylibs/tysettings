// SPDX-FileCopyrightText: Copyright 2025 Clever Design (Switzerland) GmbH
// SPDX-License-Identifier: Apache-2.0

/**
 * @file
 * @brief
 *   TinySettings simplest example: Hello World
 */

#include <pthread.h>
#include <stdio.h>
#include <tiny/instance.h>
#include <tiny/logging.h>
#include <unistd.h>
#include "tinysettings/platform/settings.h"

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
AppPersistentSettings mAppPersistentSettings = {10, 10};
}

extern "C" int main(void)
{
    tinyInstance *instance;
    tyLogInfoPlat("Starting TinySettings example");
    instance = tinyInstanceInitSingle();
    // Initialize the settings subsystem
    tyPlatSettingsInit(instance, NULL, 0);
    tyPlatSettingsSet(instance, 1, (uint8_t *)&mAppPersistentSettings, sizeof(mAppPersistentSettings));
    while (true)
    {
        // next event in 1 second
        sleep(1);
    }
    tinyInstanceFinalize(instance);
    return 0;
}
