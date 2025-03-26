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
#include <unistd.h>
#include "tinysettings/settings.h"

extern "C" int main(void)
{
    tinyInstance *instance;
    instance = tinyInstanceInitSingle();
    // Initialize the settings subsystem
    tinyPlatSettingsInit(instance, NULL, 0);
    while (true)
    {
        // next event in 1 second
        sleep(1);
    }
    tinyInstanceFinalize(instance);
    return 0;
}
