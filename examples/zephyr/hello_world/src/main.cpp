// SPDX-FileCopyrightText: Copyright 2025 Clever Design (Switzerland) GmbH
// SPDX-License-Identifier: Apache-2.0

/**
 * @file
 * @brief
 *   TinySettings simplest example: Hello World
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "tinysettings/settings.h"

#define LOG_MODULE_NAME main
LOG_MODULE_REGISTER(main);
const char *TAG = "main";

/*****************************************************************************/

extern "C" int main(void)
{
    // Subscribe to the TinySettings using the state table. This registers the module
    // to receive and process events according to the defined rules.
    LOG_WRN("Subscribe module '%s' to TinySettings", TAG);

    while (true)
    {
        // next event in 1 second
        k_sleep(K_SECONDS(1));
    }
    return 0;
}
