// SPDX-FileCopyrightText: Copyright 2025 Clever Design (Switzerland) GmbH
// SPDX-License-Identifier: Apache-2.0

#ifndef TY_POSIX_PLATFORM_SETTINGS_HPP_
#define TY_POSIX_PLATFORM_SETTINGS_HPP_

namespace tiny {
namespace Posix {

/**
 * Gets the sensitive keys that should be stored in the secure area.
 *
 * @param[in]   aInstance    The OpenThread instance structure.
 * @param[out]  aKeys        A pointer to where the pointer to the array containing sensitive keys should be written.
 * @param[out]  aKeysLength  A pointer to where the count of sensitive keys should be written.
 */
void PlatformSettingsGetSensitiveKeys(tinyInstance *aInstance, const uint16_t **aKeys, uint16_t *aKeysLength);

} // namespace Posix
} // namespace tiny

#endif // TY_POSIX_PLATFORM_SETTINGS_HPP
