// SPDX-FileCopyrightText: Copyright 2025 Clever Design (Switzerland) GmbH
// SPDX-License-Identifier: Apache-2.0

/**
 * @file
 * @brief
 *   TinySettings implementation
 */
#include <stdbool.h>
#include <stdint.h>

#include <tinyplatform/platform.h>
#include <tinyplatform/toolchain.h>

#include "tinysettings-config.h"
#include "tinysettings/tinysettings.hpp"

tbError tsLoad(TsSetting *aSettingsDefault)
{
    TB_UNUSED_VARIABLE(aSettingsDefault);
    return TB_ERROR_NONE;
}
