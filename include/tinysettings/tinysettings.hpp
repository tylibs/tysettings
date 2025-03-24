// SPDX-FileCopyrightText: Copyright 2025 Clever Design (Switzerland) GmbH
// SPDX-License-Identifier: Apache-2.0

/**
 * @file
 * @brief
 *   Main include for C based applications
 */

#ifndef TINYSETTINGS_TINYSETTINGS_H
#define TINYSETTINGS_TINYSETTINGS_H

#include <cstdint>
#include <stdbool.h>
#include <stddef.h>

#include "tinyplatform/error.h"
#include "tinyplatform/toolchain.h"

/* tbError tbSubscribe(TbSubscriber *aSubscriber); */

struct TsSetting
{
    uint64_t magic;
    size_t   len;
};

tbError tsLoad(TsSetting *aSettings);

#endif // TINYSETTINGS_TINYSETTINGS_H
