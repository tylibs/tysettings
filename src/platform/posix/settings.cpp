// SPDX-FileCopyrightText: Copyright 2025 Clever Design (Switzerland) GmbH
// SPDX-License-Identifier: Apache-2.0

#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "tiny/common/code_utils.hpp"
#include "tinysettings/settings.h"

#include <tiny/platform/exit_code.h>

#include "settings_file.hpp"

static tiny::Posix::SettingsFile sSettingsFile;

static tinyError settingsFileInit(tinyInstance *aInstance)
{
    static constexpr size_t kMaxFileBaseNameSize = 32;
    char                    fileBaseName[kMaxFileBaseNameSize];
    const char             *offset = getenv("PORT_OFFSET");
    uint64_t                nodeId;

    // tinyPlatRadioGetIeeeEui64(aInstance, reinterpret_cast<uint8_t *>(&nodeId));
    // nodeId = tiny::BigEndian::HostSwap64(nodeId);
    nodeId = 0x1234567890abcdef;

    snprintf(fileBaseName, sizeof(fileBaseName), "%s_%" PRIx64, offset == nullptr ? "0" : offset, nodeId);
    VerifyOrDie(strlen(fileBaseName) < kMaxFileBaseNameSize, TINY_EXIT_FAILURE);

    return sSettingsFile.Init(fileBaseName);
}

void tinyPlatSettingsInit(tinyInstance *aInstance, const uint16_t *aSensitiveKeys, uint16_t aSensitiveKeysLength)
{
#if !OPENTHREAD_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
    TINY_UNUSED_VARIABLE(aSensitiveKeys);
    TINY_UNUSED_VARIABLE(aSensitiveKeysLength);
#endif

#if OPENTHREAD_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
    sSensitiveKeys       = aSensitiveKeys;
    sSensitiveKeysLength = aSensitiveKeysLength;
#endif

    SuccessOrExit(settingsFileInit(aInstance));

#if OPENTHREAD_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
    tinyPosixSecureSettingsInit(aInstance);
#endif

exit:
    return;
}
