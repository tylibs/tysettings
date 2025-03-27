// SPDX-FileCopyrightText: Copyright 2025 Clever Design (Switzerland) GmbH
// SPDX-License-Identifier: Apache-2.0
/**
 * @file
 *   This file implements the Tiny platform abstraction for non-volatile storage of settings.
 */

#include <assert.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <tiny/common/debug.hpp>
#include <tiny/logging.h>
#include <tiny/platform/exit_code.h>
// #include <tiny/platform/misc.h>
// #include <tiny/platform/radio.h>

#include <tinysettings/platform/settings.h>
#if TINY_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
#include <tiny/platform/secure_settings.h>
#endif

#include "settings.hpp"
#include "settings_file.hpp"
#include "tiny/common/code_utils.hpp"
// #include "tiny/common/encoding.hpp"

// #include "system.hpp"

static tiny::Posix::SettingsFile sSettingsFile;

#if TINY_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
static const uint16_t *sSensitiveKeys       = nullptr;
static uint16_t        sSensitiveKeysLength = 0;

static bool isSensitiveKey(uint16_t aKey)
{
    bool ret = false;

    VerifyOrExit(sSensitiveKeys != nullptr);

    for (uint16_t i = 0; i < sSensitiveKeysLength; i++)
    {
        VerifyOrExit(aKey != sSensitiveKeys[i], ret = true);
    }

exit:
    return ret;
}
#endif

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
#if !TINY_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
    TINY_UNUSED_VARIABLE(aSensitiveKeys);
    TINY_UNUSED_VARIABLE(aSensitiveKeysLength);
#endif

#if TINY_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
    sSensitiveKeys       = aSensitiveKeys;
    sSensitiveKeysLength = aSensitiveKeysLength;
#endif

    // Don't touch the settings file the system runs in dry-run mode.
    // VerifyOrExit(!IsSystemDryRun());
    SuccessOrExit(settingsFileInit(aInstance));

#if TINY_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
    otPosixSecureSettingsInit(aInstance);
#endif

exit:
    return;
}

void tinyPlatSettingsDeinit(tinyInstance *aInstance)
{
    TINY_UNUSED_VARIABLE(aInstance);

    // VerifyOrExit(!IsSystemDryRun());

#if TINY_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
    otPosixSecureSettingsDeinit(aInstance);
#endif

    sSettingsFile.Deinit();

exit:
    return;
}

tinyError tinyPlatSettingsGet(tinyInstance *aInstance,
                              uint16_t      aKey,
                              int           aIndex,
                              uint8_t      *aValue,
                              uint16_t     *aValueLength)
{
    TINY_UNUSED_VARIABLE(aInstance);

    tinyError error = TINY_ERROR_NOT_FOUND;

    // VerifyOrExit(!IsSystemDryRun());
#if TINY_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
    if (isSensitiveKey(aKey))
    {
        error = otPosixSecureSettingsGet(aInstance, aKey, aIndex, aValue, aValueLength);
    }
    else
#endif
    {
        error = sSettingsFile.Get(aKey, aIndex, aValue, aValueLength);
    }

exit:
    VerifyOrDie(error != TINY_ERROR_PARSE, TINY_EXIT_FAILURE);
    return error;
}

tinyError tinyPlatSettingsSet(tinyInstance *aInstance, uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength)
{
    TINY_UNUSED_VARIABLE(aInstance);

    tinyError error = TINY_ERROR_NONE;

#if TINY_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
    if (isSensitiveKey(aKey))
    {
        error = otPosixSecureSettingsSet(aInstance, aKey, aValue, aValueLength);
    }
    else
#endif
    {
        sSettingsFile.Set(aKey, aValue, aValueLength);
    }

    return error;
}

tinyError tinyPlatSettingsAdd(tinyInstance *aInstance, uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength)
{
    TINY_UNUSED_VARIABLE(aInstance);

    tinyError error = TINY_ERROR_NONE;

#if TINY_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
    if (isSensitiveKey(aKey))
    {
        error = otPosixSecureSettingsAdd(aInstance, aKey, aValue, aValueLength);
    }
    else
#endif
    {
        sSettingsFile.Add(aKey, aValue, aValueLength);
    }

    return error;
}

tinyError tinyPlatSettingsDelete(tinyInstance *aInstance, uint16_t aKey, int aIndex)
{
    TINY_UNUSED_VARIABLE(aInstance);

    tinyError error;

#if TINY_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
    if (isSensitiveKey(aKey))
    {
        error = otPosixSecureSettingsDelete(aInstance, aKey, aIndex);
    }
    else
#endif
    {
        error = sSettingsFile.Delete(aKey, aIndex);
    }

    return error;
}

void tinyPlatSettingsWipe(tinyInstance *aInstance)
{
    TINY_UNUSED_VARIABLE(aInstance);
#if TINY_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
    otPosixSecureSettingsWipe(aInstance);
#endif

    sSettingsFile.Wipe();
}

namespace ot {
namespace Posix {
#if TINY_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
void PlatformSettingsGetSensitiveKeys(tinyInstance *aInstance, const uint16_t **aKeys, uint16_t *aKeysLength)
{
    TINY_UNUSED_VARIABLE(aInstance);

    assert(aKeys != nullptr);
    assert(aKeysLength != nullptr);

    *aKeys       = sSensitiveKeys;
    *aKeysLength = sSensitiveKeysLength;
}
#endif

} // namespace Posix
} // namespace ot

#ifndef SELF_TEST
#define SELF_TEST 0
#endif

#if SELF_TEST

void otLogCritPlat(const char *aFormat, ...)
{
    TINY_UNUSED_VARIABLE(aFormat);
}

const char *otExitCodeToString(uint8_t aExitCode)
{
    TINY_UNUSED_VARIABLE(aExitCode);
    return "";
}

void tinyPlatRadioGetIeeeEui64(tinyInstance *aInstance, uint8_t *aIeeeEui64)
{
    TINY_UNUSED_VARIABLE(aInstance);

    memset(aIeeeEui64, 0, sizeof(uint64_t));
}

// Stub implementation for testing
bool IsSystemDryRun(void)
{
    return false;
}

int main()
{
    tinyInstance *instance = nullptr;
    uint8_t       data[60];

    for (uint8_t i = 0; i < sizeof(data); ++i)
    {
        data[i] = i;
    }

    tinyPlatSettingsInit(instance, nullptr, 0);

    // verify empty situation
    tinyPlatSettingsWipe(instance);
    {
        uint8_t  value[sizeof(data)];
        uint16_t length = sizeof(value);

        assert(tinyPlatSettingsGet(instance, 0, 0, value, &length) == TINY_ERROR_NOT_FOUND);
        assert(tinyPlatSettingsDelete(instance, 0, 0) == TINY_ERROR_NOT_FOUND);
        assert(tinyPlatSettingsDelete(instance, 0, -1) == TINY_ERROR_NOT_FOUND);
    }

    // verify write one record
    assert(tinyPlatSettingsSet(instance, 0, data, sizeof(data) / 2) == TINY_ERROR_NONE);
    {
        uint8_t  value[sizeof(data)];
        uint16_t length = sizeof(value);

        assert(tinyPlatSettingsGet(instance, 0, 0, nullptr, nullptr) == TINY_ERROR_NONE);
        assert(tinyPlatSettingsGet(instance, 0, 0, nullptr, &length) == TINY_ERROR_NONE);
        assert(length == sizeof(data) / 2);

        length = sizeof(value);
        assert(tinyPlatSettingsGet(instance, 0, 0, value, &length) == TINY_ERROR_NONE);
        assert(length == sizeof(data) / 2);
        assert(0 == memcmp(value, data, length));

        // insufficient buffer
        length -= 1;
        value[length] = 0;
        assert(tinyPlatSettingsGet(instance, 0, 0, value, &length) == TINY_ERROR_NONE);
        // verify length becomes the actual length of the record
        assert(length == sizeof(data) / 2);
        // verify this byte is not changed
        assert(value[length] == 0);

        // wrong index
        assert(tinyPlatSettingsGet(instance, 0, 1, nullptr, nullptr) == TINY_ERROR_NOT_FOUND);
        // wrong key
        assert(tinyPlatSettingsGet(instance, 1, 0, nullptr, nullptr) == TINY_ERROR_NOT_FOUND);
    }
    tinyPlatSettingsWipe(instance);

    // verify write two records
    assert(tinyPlatSettingsSet(instance, 0, data, sizeof(data)) == TINY_ERROR_NONE);
    assert(tinyPlatSettingsAdd(instance, 0, data, sizeof(data) / 2) == TINY_ERROR_NONE);
    {
        uint8_t  value[sizeof(data)];
        uint16_t length = sizeof(value);

        assert(tinyPlatSettingsGet(instance, 0, 1, value, &length) == TINY_ERROR_NONE);
        assert(length == sizeof(data) / 2);
        assert(0 == memcmp(value, data, length));

        length = sizeof(value);
        assert(tinyPlatSettingsGet(instance, 0, 0, value, &length) == TINY_ERROR_NONE);
        assert(length == sizeof(data));
        assert(0 == memcmp(value, data, length));
    }
    tinyPlatSettingsWipe(instance);

    // verify write two records of different keys
    assert(tinyPlatSettingsSet(instance, 0, data, sizeof(data)) == TINY_ERROR_NONE);
    assert(tinyPlatSettingsAdd(instance, 1, data, sizeof(data) / 2) == TINY_ERROR_NONE);
    {
        uint8_t  value[sizeof(data)];
        uint16_t length = sizeof(value);

        assert(tinyPlatSettingsGet(instance, 1, 0, value, &length) == TINY_ERROR_NONE);
        assert(length == sizeof(data) / 2);
        assert(0 == memcmp(value, data, length));

        length = sizeof(value);
        assert(tinyPlatSettingsGet(instance, 0, 0, value, &length) == TINY_ERROR_NONE);
        assert(length == sizeof(data));
        assert(0 == memcmp(value, data, length));
    }
    tinyPlatSettingsWipe(instance);

    // verify delete record
    assert(tinyPlatSettingsAdd(instance, 0, data, sizeof(data)) == TINY_ERROR_NONE);
    assert(tinyPlatSettingsAdd(instance, 0, data, sizeof(data) / 2) == TINY_ERROR_NONE);
    assert(tinyPlatSettingsAdd(instance, 0, data, sizeof(data) / 3) == TINY_ERROR_NONE);
    {
        uint8_t  value[sizeof(data)];
        uint16_t length = sizeof(value);

        // wrong key
        assert(tinyPlatSettingsDelete(instance, 1, 0) == TINY_ERROR_NOT_FOUND);
        assert(tinyPlatSettingsDelete(instance, 1, -1) == TINY_ERROR_NOT_FOUND);

        // wrong index
        assert(tinyPlatSettingsDelete(instance, 0, 3) == TINY_ERROR_NOT_FOUND);

        // delete one record
        assert(tinyPlatSettingsDelete(instance, 0, 1) == TINY_ERROR_NONE);
        assert(tinyPlatSettingsGet(instance, 0, 1, value, &length) == TINY_ERROR_NONE);
        assert(length == sizeof(data) / 3);
        assert(0 == memcmp(value, data, length));

        // delete all records
        assert(tinyPlatSettingsDelete(instance, 0, -1) == TINY_ERROR_NONE);
        assert(tinyPlatSettingsGet(instance, 0, 0, nullptr, nullptr) == TINY_ERROR_NOT_FOUND);
    }
    tinyPlatSettingsWipe(instance);

    // verify delete all records of a type
    assert(tinyPlatSettingsAdd(instance, 0, data, sizeof(data)) == TINY_ERROR_NONE);
    assert(tinyPlatSettingsAdd(instance, 1, data, sizeof(data) / 2) == TINY_ERROR_NONE);
    assert(tinyPlatSettingsAdd(instance, 0, data, sizeof(data) / 3) == TINY_ERROR_NONE);
    {
        uint8_t  value[sizeof(data)];
        uint16_t length = sizeof(value);

        assert(tinyPlatSettingsDelete(instance, 0, -1) == TINY_ERROR_NONE);
        assert(tinyPlatSettingsGet(instance, 0, 0, value, &length) == TINY_ERROR_NOT_FOUND);
        assert(tinyPlatSettingsGet(instance, 1, 0, value, &length) == TINY_ERROR_NONE);
        assert(length == sizeof(data) / 2);
        assert(0 == memcmp(value, data, length));

        assert(tinyPlatSettingsDelete(instance, 0, 0) == TINY_ERROR_NOT_FOUND);
        assert(tinyPlatSettingsGet(instance, 0, 0, nullptr, nullptr) == TINY_ERROR_NOT_FOUND);
    }
    tinyPlatSettingsWipe(instance);
    tinyPlatSettingsDeinit(instance);

    return 0;
}
#endif
