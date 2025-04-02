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

#include <ty/common/debug.hpp>
#include <ty/exit_code.h>
#include <ty/logging.h>
// #include <ty/platform/misc.h>
// #include <ty/platform/radio.h>

#include <tysettings/platform/settings.h>
#if TY_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
#include <ty/platform/secure_settings.h>
#endif

#include "settings.hpp"
#include "settings_file.hpp"
#include "ty/common/code_utils.hpp"
// #include "ty/common/encoding.hpp"

// #include "system.hpp"

static ty::Posix::SettingsFile sSettingsFile;

#if TY_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
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

    // tyPlatRadioGetIeeeEui64(aInstance, reinterpret_cast<uint8_t *>(&nodeId));
    // nodeId = ty::BigEndian::HostSwap64(nodeId);
    nodeId = 0x1234567890abcdef;
    snprintf(fileBaseName, sizeof(fileBaseName), "%s_%" PRIx64, offset == nullptr ? "0" : offset, nodeId);
    VerifyOrDie(strlen(fileBaseName) < kMaxFileBaseNameSize, TY_EXIT_FAILURE);

    return sSettingsFile.Init(fileBaseName);
}

void tyPlatSettingsInit(tinyInstance *aInstance, const uint16_t *aSensitiveKeys, uint16_t aSensitiveKeysLength)
{
#if !TY_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
    TY_UNUSED_VARIABLE(aSensitiveKeys);
    TY_UNUSED_VARIABLE(aSensitiveKeysLength);
#endif

#if TY_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
    sSensitiveKeys       = aSensitiveKeys;
    sSensitiveKeysLength = aSensitiveKeysLength;
#endif

    // Don't touch the settings file the system runs in dry-run mode.
    // VerifyOrExit(!IsSystemDryRun());
    SuccessOrExit(settingsFileInit(aInstance));

#if TY_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
    otPosixSecureSettingsInit(aInstance);
#endif

exit:
    return;
}

void tyPlatSettingsDeinit(tinyInstance *aInstance)
{
    TY_UNUSED_VARIABLE(aInstance);

    // VerifyOrExit(!IsSystemDryRun());

#if TY_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
    otPosixSecureSettingsDeinit(aInstance);
#endif

    sSettingsFile.Deinit();

exit:
    return;
}

tinyError tyPlatSettingsGet(tinyInstance *aInstance, uint16_t aKey, int aIndex, uint8_t *aValue, uint16_t *aValueLength)
{
    TY_UNUSED_VARIABLE(aInstance);

    tinyError error = TY_ERROR_NOT_FOUND;

    // VerifyOrExit(!IsSystemDryRun());
#if TY_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
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
    VerifyOrDie(error != TY_ERROR_PARSE, TY_EXIT_FAILURE);
    return error;
}

tinyError tyPlatSettingsSet(tinyInstance *aInstance, uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength)
{
    TY_UNUSED_VARIABLE(aInstance);

    tinyError error = TY_ERROR_NONE;

#if TY_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
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

tinyError tyPlatSettingsAdd(tinyInstance *aInstance, uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength)
{
    TY_UNUSED_VARIABLE(aInstance);

    tinyError error = TY_ERROR_NONE;

#if TY_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
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

tinyError tyPlatSettingsDelete(tinyInstance *aInstance, uint16_t aKey, int aIndex)
{
    TY_UNUSED_VARIABLE(aInstance);

    tinyError error;

#if TY_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
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

void tyPlatSettingsWipe(tinyInstance *aInstance)
{
    TY_UNUSED_VARIABLE(aInstance);
#if TY_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
    otPosixSecureSettingsWipe(aInstance);
#endif

    sSettingsFile.Wipe();
}

namespace ot {
namespace Posix {
#if TY_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
void PlatformSettingsGetSensitiveKeys(tinyInstance *aInstance, const uint16_t **aKeys, uint16_t *aKeysLength)
{
    TY_UNUSED_VARIABLE(aInstance);

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
    TY_UNUSED_VARIABLE(aFormat);
}

const char *otExitCodeToString(uint8_t aExitCode)
{
    TY_UNUSED_VARIABLE(aExitCode);
    return "";
}

void tyPlatRadioGetIeeeEui64(tinyInstance *aInstance, uint8_t *aIeeeEui64)
{
    TY_UNUSED_VARIABLE(aInstance);

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

    tyPlatSettingsInit(instance, nullptr, 0);

    // verify empty situation
    tyPlatSettingsWipe(instance);
    {
        uint8_t  value[sizeof(data)];
        uint16_t length = sizeof(value);

        assert(tyPlatSettingsGet(instance, 0, 0, value, &length) == TY_ERROR_NOT_FOUND);
        assert(tyPlatSettingsDelete(instance, 0, 0) == TY_ERROR_NOT_FOUND);
        assert(tyPlatSettingsDelete(instance, 0, -1) == TY_ERROR_NOT_FOUND);
    }

    // verify write one record
    assert(tyPlatSettingsSet(instance, 0, data, sizeof(data) / 2) == TY_ERROR_NONE);
    {
        uint8_t  value[sizeof(data)];
        uint16_t length = sizeof(value);

        assert(tyPlatSettingsGet(instance, 0, 0, nullptr, nullptr) == TY_ERROR_NONE);
        assert(tyPlatSettingsGet(instance, 0, 0, nullptr, &length) == TY_ERROR_NONE);
        assert(length == sizeof(data) / 2);

        length = sizeof(value);
        assert(tyPlatSettingsGet(instance, 0, 0, value, &length) == TY_ERROR_NONE);
        assert(length == sizeof(data) / 2);
        assert(0 == memcmp(value, data, length));

        // insufficient buffer
        length -= 1;
        value[length] = 0;
        assert(tyPlatSettingsGet(instance, 0, 0, value, &length) == TY_ERROR_NONE);
        // verify length becomes the actual length of the record
        assert(length == sizeof(data) / 2);
        // verify this byte is not changed
        assert(value[length] == 0);

        // wrong index
        assert(tyPlatSettingsGet(instance, 0, 1, nullptr, nullptr) == TY_ERROR_NOT_FOUND);
        // wrong key
        assert(tyPlatSettingsGet(instance, 1, 0, nullptr, nullptr) == TY_ERROR_NOT_FOUND);
    }
    tyPlatSettingsWipe(instance);

    // verify write two records
    assert(tyPlatSettingsSet(instance, 0, data, sizeof(data)) == TY_ERROR_NONE);
    assert(tyPlatSettingsAdd(instance, 0, data, sizeof(data) / 2) == TY_ERROR_NONE);
    {
        uint8_t  value[sizeof(data)];
        uint16_t length = sizeof(value);

        assert(tyPlatSettingsGet(instance, 0, 1, value, &length) == TY_ERROR_NONE);
        assert(length == sizeof(data) / 2);
        assert(0 == memcmp(value, data, length));

        length = sizeof(value);
        assert(tyPlatSettingsGet(instance, 0, 0, value, &length) == TY_ERROR_NONE);
        assert(length == sizeof(data));
        assert(0 == memcmp(value, data, length));
    }
    tyPlatSettingsWipe(instance);

    // verify write two records of different keys
    assert(tyPlatSettingsSet(instance, 0, data, sizeof(data)) == TY_ERROR_NONE);
    assert(tyPlatSettingsAdd(instance, 1, data, sizeof(data) / 2) == TY_ERROR_NONE);
    {
        uint8_t  value[sizeof(data)];
        uint16_t length = sizeof(value);

        assert(tyPlatSettingsGet(instance, 1, 0, value, &length) == TY_ERROR_NONE);
        assert(length == sizeof(data) / 2);
        assert(0 == memcmp(value, data, length));

        length = sizeof(value);
        assert(tyPlatSettingsGet(instance, 0, 0, value, &length) == TY_ERROR_NONE);
        assert(length == sizeof(data));
        assert(0 == memcmp(value, data, length));
    }
    tyPlatSettingsWipe(instance);

    // verify delete record
    assert(tyPlatSettingsAdd(instance, 0, data, sizeof(data)) == TY_ERROR_NONE);
    assert(tyPlatSettingsAdd(instance, 0, data, sizeof(data) / 2) == TY_ERROR_NONE);
    assert(tyPlatSettingsAdd(instance, 0, data, sizeof(data) / 3) == TY_ERROR_NONE);
    {
        uint8_t  value[sizeof(data)];
        uint16_t length = sizeof(value);

        // wrong key
        assert(tyPlatSettingsDelete(instance, 1, 0) == TY_ERROR_NOT_FOUND);
        assert(tyPlatSettingsDelete(instance, 1, -1) == TY_ERROR_NOT_FOUND);

        // wrong index
        assert(tyPlatSettingsDelete(instance, 0, 3) == TY_ERROR_NOT_FOUND);

        // delete one record
        assert(tyPlatSettingsDelete(instance, 0, 1) == TY_ERROR_NONE);
        assert(tyPlatSettingsGet(instance, 0, 1, value, &length) == TY_ERROR_NONE);
        assert(length == sizeof(data) / 3);
        assert(0 == memcmp(value, data, length));

        // delete all records
        assert(tyPlatSettingsDelete(instance, 0, -1) == TY_ERROR_NONE);
        assert(tyPlatSettingsGet(instance, 0, 0, nullptr, nullptr) == TY_ERROR_NOT_FOUND);
    }
    tyPlatSettingsWipe(instance);

    // verify delete all records of a type
    assert(tyPlatSettingsAdd(instance, 0, data, sizeof(data)) == TY_ERROR_NONE);
    assert(tyPlatSettingsAdd(instance, 1, data, sizeof(data) / 2) == TY_ERROR_NONE);
    assert(tyPlatSettingsAdd(instance, 0, data, sizeof(data) / 3) == TY_ERROR_NONE);
    {
        uint8_t  value[sizeof(data)];
        uint16_t length = sizeof(value);

        assert(tyPlatSettingsDelete(instance, 0, -1) == TY_ERROR_NONE);
        assert(tyPlatSettingsGet(instance, 0, 0, value, &length) == TY_ERROR_NOT_FOUND);
        assert(tyPlatSettingsGet(instance, 1, 0, value, &length) == TY_ERROR_NONE);
        assert(length == sizeof(data) / 2);
        assert(0 == memcmp(value, data, length));

        assert(tyPlatSettingsDelete(instance, 0, 0) == TY_ERROR_NOT_FOUND);
        assert(tyPlatSettingsGet(instance, 0, 0, nullptr, nullptr) == TY_ERROR_NOT_FOUND);
    }
    tyPlatSettingsWipe(instance);
    tyPlatSettingsDeinit(instance);

    return 0;
}
#endif
