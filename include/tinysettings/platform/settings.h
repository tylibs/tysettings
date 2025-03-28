// SPDX-FileCopyrightText: Copyright 2025 Clever Design (Switzerland) GmbH
// SPDX-License-Identifier: Apache-2.0

/**
 * @file
 * @brief
 *   Main include for C based applications
 */

#ifndef TINYSETTINGS_SETTINGS_H
#define TINYSETTINGS_SETTINGS_H

#include <tiny/instance.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Defines the keys of settings.
 *
 * Note: When adding a new settings key, if the settings corresponding to the key contains security sensitive
 *       information, the developer MUST add the key to the array `aSensitiveKeys` which is passed in
 *       `tiSettingsInit()`.
 */
enum
{
    TY_SETTINGS_KEY_ACTIVE_DATASET  = 0x0001, ///< Active Operational Dataset.
    TY_SETTINGS_KEY_PENDING_DATASET = 0x0002, ///< Pending Operational Dataset.
    TY_SETTINGS_KEY_NETWORK_INFO    = 0x0003, ///< Thread network information.

    // Keys in range 0x8000-0xffff are reserved for vendor-specific use.
    TY_SETTINGS_KEY_VENDOR_RESERVED_MIN = 0x8000,
    TY_SETTINGS_KEY_VENDOR_RESERVED_MAX = 0xffff,
};

/**
 * Performs any initialization for the settings subsystem, if necessary.
 *
 * Also sets the sensitive keys that should be stored in the secure area.
 *
 * Note that the memory pointed by @p aSensitiveKeys MUST not be released before @p aInstance is destroyed.
 *
 * @param[in]  aInstance             The OpenThread instance structure.
 * @param[in]  aSensitiveKeys        A pointer to an array containing the list of sensitive keys. May be NULL only if
 *                                   @p aSensitiveKeysLength is 0, which means that there is no sensitive keys.
 * @param[in]  aSensitiveKeysLength  The number of entries in the @p aSensitiveKeys array.
 */
void tyPlatSettingsInit(tinyInstance *aInstance, const uint16_t *aSensitiveKeys, uint16_t aSensitiveKeysLength);

/**
 * Performs any de-initialization for the settings subsystem, if necessary.
 *
 * @param[in]  aInstance The OpenThread instance structure.
 */
void tyPlatSettingsDeinit(tinyInstance *aInstance);

/**
 * Fetches the value of a setting.
 *
 * Fetches the value of the setting identified
 * by @p aKey and write it to the memory pointed to by aValue.
 * It then writes the length to the integer pointed to by
 * @p aValueLength. The initial value of @p aValueLength is the
 * maximum number of bytes to be written to @p aValue.
 *
 * Can be used to check for the existence of
 * a key without fetching the value by setting @p aValue and
 * @p aValueLength to NULL. You can also check the length of
 * the setting without fetching it by setting only aValue
 * to NULL.
 *
 * Note that the underlying storage implementation is not
 * required to maintain the order of settings with multiple
 * values. The order of such values MAY change after ANY
 * write operation to the store.
 *
 * @param[in]      aInstance     The OpenThread instance structure.
 * @param[in]      aKey          The key associated with the requested setting.
 * @param[in]      aIndex        The index of the specific item to get.
 * @param[out]     aValue        A pointer to where the value of the setting should be written. May be set to NULL if
 *                               just testing for the presence or length of a setting.
 * @param[in,out]  aValueLength  A pointer to the length of the value. When called, this pointer should point to an
 *                               integer containing the maximum value size that can be written to @p aValue. At return,
 *                               the actual length of the setting is written. This may be set to NULL if performing
 *                               a presence check.
 *
 * @retval TY_ERROR_NONE             The given setting was found and fetched successfully.
 * @retval TY_ERROR_NOT_FOUND        The given setting was not found in the setting store.
 * @retval TY_ERROR_NOT_IMPLEMENTED  This function is not implemented on this platform.
 */
tinyError tyPlatSettingsGet(tinyInstance *aInstance,
                            uint16_t      aKey,
                            int           aIndex,
                            uint8_t      *aValue,
                            uint16_t     *aValueLength);

/**
 * Sets or replaces the value of a setting.
 *
 * Sets or replaces the value of a setting
 * identified by @p aKey.
 *
 * Calling this function successfully may cause unrelated
 * settings with multiple values to be reordered.
 *
 * OpenThread stack guarantees to use `tiSettingsSet()`
 * method for a @p aKey that was either previously set using
 * `tiSettingsSet()` (i.e., contains a single value) or
 * is empty and/or fully deleted (contains no value).
 *
 * Platform layer can rely and use this fact for optimizing
 * its implementation.
 *
 * @param[in]  aInstance     The OpenThread instance structure.
 * @param[in]  aKey          The key associated with the setting to change.
 * @param[in]  aValue        A pointer to where the new value of the setting should be read from. MUST NOT be NULL if
 *                           @p aValueLength is non-zero.
 * @param[in]  aValueLength  The length of the data pointed to by aValue. May be zero.
 *
 * @retval TY_ERROR_NONE             The given setting was changed or staged.
 * @retval TY_ERROR_NTY_IMPLEMENTED  This function is not implemented on this platform.
 * @retval TY_ERROR_NO_BUFS          No space remaining to store the given setting.
 */
tinyError tyPlatSettingsSet(tinyInstance *aInstance, uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength);

/**
 * Adds a value to a setting.
 *
 * Adds the value to a setting
 * identified by @p aKey, without replacing any existing
 * values.
 *
 * Note that the underlying implementation is not required
 * to maintain the order of the items associated with a
 * specific key. The added value may be added to the end,
 * the beginning, or even somewhere in the middle. The order
 * of any pre-existing values may also change.
 *
 * Calling this function successfully may cause unrelated
 * settings with multiple values to be reordered.
 *
 * OpenThread stack guarantees to use `tiSettingsAdd()`
 * method for a @p aKey that was either previously managed by
 * `tiSettingsAdd()` (i.e., contains one or more items) or
 * is empty and/or fully deleted (contains no value).
 *
 * Platform layer can rely and use this fact for optimizing
 * its implementation.
 *
 * @param[in]  aInstance     The OpenThread instance structure.
 * @param[in]  aKey          The key associated with the setting to change.
 * @param[in]  aValue        A pointer to where the new value of the setting should be read from. MUST NOT be NULL
 *                           if @p aValueLength is non-zero.
 * @param[in]  aValueLength  The length of the data pointed to by @p aValue. May be zero.
 *
 * @retval TY_ERROR_NONE             The given setting was added or staged to be added.
 * @retval TY_ERROR_NTY_IMPLEMENTED  This function is not implemented on this platform.
 * @retval TY_ERROR_NO_BUFS          No space remaining to store the given setting.
 */
tinyError tyPlatSettingsAdd(tinyInstance *aInstance, uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength);

/**
 * Removes a setting from the setting store.
 *
 * Deletes a specific value from the
 * setting identified by aKey from the settings store.
 *
 * Note that the underlying implementation is not required
 * to maintain the order of the items associated with a
 * specific key.
 *
 * @param[in] aInstance  The OpenThread instance structure.
 * @param[in] aKey       The key associated with the requested setting.
 * @param[in] aIndex     The index of the value to be removed. If set to -1, all values for this @p aKey will be
 *                       removed.
 *
 * @retval TY_ERROR_NONE             The given key and index was found and removed successfully.
 * @retval TY_ERROR_NOT_FOUND        The given key or index was not found in the setting store.
 * @retval TY_ERROR_NOT_IMPLEMENTED  This function is not implemented on this platform.
 */
tinyError tyPlatSettingsDelete(tinyInstance *aInstance, uint16_t aKey, int aIndex);

/**
 * Removes all settings from the setting store.
 *
 * Deletes all settings from the settings
 * store, resetting it to its initial factory state.
 *
 * @param[in] aInstance  The OpenThread instance structure.
 */
void tyPlatSettingsWipe(tinyInstance *aInstance);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TINYSETTINGS_SETTINGS_H
