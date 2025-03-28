// SPDX-FileCopyrightText: Copyright 2025 Clever Design (Switzerland) GmbH
// SPDX-License-Identifier: Apache-2.0

/**
 * @file
 *   This file implements the settings file module for getting, setting and deleting the key-value pairs.
 */

#include <fcntl.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <tiny/common/code_utils.hpp>
#include <tiny/common/debug.hpp>
#include <tiny/platform/exit_code.h>

#include "settings_file.hpp"

namespace tiny {
namespace Posix {

tinyError SettingsFile::Init(const char *aSettingsFileBaseName)
{
    tinyError   error     = TY_ERROR_NONE;
    const char *directory = TY_CONFIG_POSIX_SETTINGS_PATH;

    TY_ASSERT((aSettingsFileBaseName != nullptr) && (strlen(aSettingsFileBaseName) < kMaxFileBaseNameSize));
    strncpy(mSettingFileBaseName, aSettingsFileBaseName, sizeof(mSettingFileBaseName) - 1);

    {
        struct stat st;

        if (stat(directory, &st) == -1)
        {
            VerifyOrDie(mkdir(directory, 0755) == 0, TY_EXIT_ERROR_ERRNO);
        }
    }

    {
        char fileName[kMaxFilePathSize];

        GetSettingsFilePath(fileName, false);
        mSettingsFd = open(fileName, O_RDWR | O_CREAT | O_CLOEXEC, 0600);
    }

    VerifyOrDie(mSettingsFd != -1, TY_EXIT_ERROR_ERRNO);

    for (off_t size = lseek(mSettingsFd, 0, SEEK_END), offset = lseek(mSettingsFd, 0, SEEK_SET); offset < size;)
    {
        uint16_t key;
        uint16_t length;
        ssize_t  rval;

        rval = read(mSettingsFd, &key, sizeof(key));
        VerifyOrExit(rval == sizeof(key), error = TY_ERROR_PARSE);

        rval = read(mSettingsFd, &length, sizeof(length));
        VerifyOrExit(rval == sizeof(length), error = TY_ERROR_PARSE);

        offset += sizeof(key) + sizeof(length) + length;
        VerifyOrExit(offset == lseek(mSettingsFd, length, SEEK_CUR), error = TY_ERROR_PARSE);
    }

exit:
    if (error == TY_ERROR_PARSE)
    {
        VerifyOrDie(ftruncate(mSettingsFd, 0) == 0, TY_EXIT_ERROR_ERRNO);
    }

    return error;
}

void SettingsFile::Deinit(void)
{
    VerifyOrExit(mSettingsFd != -1);
    VerifyOrDie(close(mSettingsFd) == 0, TY_EXIT_ERROR_ERRNO);
    mSettingsFd = -1;

exit:
    return;
}

tinyError SettingsFile::Get(uint16_t aKey, int aIndex, uint8_t *aValue, uint16_t *aValueLength)
{
    tinyError error = TY_ERROR_NOT_FOUND;
    off_t     size;
    off_t     offset;

    TY_ASSERT(mSettingsFd >= 0);

    size   = lseek(mSettingsFd, 0, SEEK_END);
    offset = lseek(mSettingsFd, 0, SEEK_SET);
    VerifyOrExit(offset == 0 && size >= 0, error = TY_ERROR_PARSE);

    while (offset < size)
    {
        uint16_t key;
        uint16_t length;
        ssize_t  rval;

        rval = read(mSettingsFd, &key, sizeof(key));
        VerifyOrExit(rval == sizeof(key), error = TY_ERROR_PARSE);

        rval = read(mSettingsFd, &length, sizeof(length));
        VerifyOrExit(rval == sizeof(length), error = TY_ERROR_PARSE);

        if (key == aKey)
        {
            if (aIndex == 0)
            {
                error = TY_ERROR_NONE;

                if (aValueLength)
                {
                    if (aValue)
                    {
                        uint16_t readLength = (length <= *aValueLength ? length : *aValueLength);

                        VerifyOrExit(read(mSettingsFd, aValue, readLength) == readLength, error = TY_ERROR_PARSE);
                    }

                    *aValueLength = length;
                }

                break;
            }
            else
            {
                --aIndex;
            }
        }

        offset += sizeof(key) + sizeof(length) + length;
        VerifyOrExit(offset == lseek(mSettingsFd, length, SEEK_CUR), error = TY_ERROR_PARSE);
    }

exit:
    return error;
}

void SettingsFile::Set(uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength)
{
    int swapFd = -1;

    TY_ASSERT(mSettingsFd >= 0);

    switch (Delete(aKey, -1, &swapFd))
    {
    case TY_ERROR_NONE:
    case TY_ERROR_NOT_FOUND:
        break;

    default:
        TY_ASSERT(false);
        break;
    }

    VerifyOrDie(write(swapFd, &aKey, sizeof(aKey)) == sizeof(aKey) &&
                    write(swapFd, &aValueLength, sizeof(aValueLength)) == sizeof(aValueLength) &&
                    write(swapFd, aValue, aValueLength) == aValueLength,
                TY_EXIT_FAILURE);

    SwapPersist(swapFd);
}

void SettingsFile::Add(uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength)
{
    off_t size;
    int   swapFd;

    TY_ASSERT(mSettingsFd >= 0);

    size   = lseek(mSettingsFd, 0, SEEK_END);
    swapFd = SwapOpen();

    if (size > 0)
    {
        VerifyOrDie(0 == lseek(mSettingsFd, 0, SEEK_SET), TY_EXIT_ERROR_ERRNO);
        SwapWrite(swapFd, static_cast<uint16_t>(size));
    }

    VerifyOrDie(write(swapFd, &aKey, sizeof(aKey)) == sizeof(aKey) &&
                    write(swapFd, &aValueLength, sizeof(aValueLength)) == sizeof(aValueLength) &&
                    write(swapFd, aValue, aValueLength) == aValueLength,
                TY_EXIT_FAILURE);

    SwapPersist(swapFd);
}

tinyError SettingsFile::Delete(uint16_t aKey, int aIndex)
{
    return Delete(aKey, aIndex, nullptr);
}

tinyError SettingsFile::Delete(uint16_t aKey, int aIndex, int *aSwapFd)
{
    tinyError error = TY_ERROR_NOT_FOUND;
    off_t     size;
    off_t     offset;
    int       swapFd;

    TY_ASSERT(mSettingsFd >= 0);

    size   = lseek(mSettingsFd, 0, SEEK_END);
    offset = lseek(mSettingsFd, 0, SEEK_SET);
    swapFd = SwapOpen();

    TY_ASSERT(swapFd != -1);
    TY_ASSERT(offset == 0);
    VerifyOrExit(offset == 0 && size >= 0, error = TY_ERROR_FAILED);

    while (offset < size)
    {
        uint16_t key;
        uint16_t length;
        ssize_t  rval;

        rval = read(mSettingsFd, &key, sizeof(key));
        VerifyOrExit(rval == sizeof(key), error = TY_ERROR_FAILED);

        rval = read(mSettingsFd, &length, sizeof(length));
        VerifyOrExit(rval == sizeof(length), error = TY_ERROR_FAILED);

        offset += sizeof(key) + sizeof(length) + length;

        if (aKey == key)
        {
            if (aIndex == 0)
            {
                VerifyOrExit(offset == lseek(mSettingsFd, length, SEEK_CUR), error = TY_ERROR_FAILED);
                SwapWrite(swapFd, static_cast<uint16_t>(size - offset));
                error = TY_ERROR_NONE;
                break;
            }
            else if (aIndex == -1)
            {
                VerifyOrExit(offset == lseek(mSettingsFd, length, SEEK_CUR), error = TY_ERROR_FAILED);
                error = TY_ERROR_NONE;
                continue;
            }
            else
            {
                --aIndex;
            }
        }

        rval = write(swapFd, &key, sizeof(key));
        VerifyOrExit(rval == sizeof(key), error = TY_ERROR_FAILED);

        rval = write(swapFd, &length, sizeof(length));
        VerifyOrExit(rval == sizeof(length), error = TY_ERROR_FAILED);

        SwapWrite(swapFd, length);
    }

exit:
    if (aSwapFd != nullptr)
    {
        *aSwapFd = swapFd;
    }
    else if (error == TY_ERROR_NONE)
    {
        SwapPersist(swapFd);
    }
    else if (error == TY_ERROR_NOT_FOUND)
    {
        SwapDiscard(swapFd);
    }
    else if (error == TY_ERROR_FAILED)
    {
        SwapDiscard(swapFd);
        DieNow(error);
    }

    return error;
}

void SettingsFile::Wipe(void)
{
    VerifyOrDie(0 == ftruncate(mSettingsFd, 0), TY_EXIT_ERROR_ERRNO);
}

void SettingsFile::GetSettingsFilePath(char aFileName[kMaxFilePathSize], bool aSwap)
{
    snprintf(aFileName, kMaxFilePathSize, TY_CONFIG_POSIX_SETTINGS_PATH "/%s.%s", mSettingFileBaseName,
             (aSwap ? "Swap" : "data"));
}

int SettingsFile::SwapOpen(void)
{
    char fileName[kMaxFilePathSize];
    int  fd;

    GetSettingsFilePath(fileName, true);

    fd = open(fileName, O_RDWR | O_CREAT | O_TRUNC | O_CLOEXEC, 0600);
    VerifyOrDie(fd != -1, TY_EXIT_ERROR_ERRNO);

    return fd;
}

void SettingsFile::SwapWrite(int aFd, uint16_t aLength)
{
    const size_t kBlockSize = 512;
    uint8_t      buffer[kBlockSize];

    while (aLength > 0)
    {
        uint16_t count = aLength >= sizeof(buffer) ? sizeof(buffer) : aLength;
        ssize_t  rval  = read(mSettingsFd, buffer, count);

        VerifyOrDie(rval > 0, TY_EXIT_FAILURE);
        count = static_cast<uint16_t>(rval);
        rval  = write(aFd, buffer, count);
        TY_ASSERT(rval == count);
        VerifyOrDie(rval == count, TY_EXIT_FAILURE);
        aLength -= count;
    }
}

void SettingsFile::SwapPersist(int aFd)
{
    char swapFile[kMaxFilePathSize];
    char dataFile[kMaxFilePathSize];

    GetSettingsFilePath(swapFile, true);
    GetSettingsFilePath(dataFile, false);

    VerifyOrDie(0 == close(mSettingsFd), TY_EXIT_ERROR_ERRNO);
    VerifyOrDie(0 == fsync(aFd), TY_EXIT_ERROR_ERRNO);
    VerifyOrDie(0 == rename(swapFile, dataFile), TY_EXIT_ERROR_ERRNO);

    mSettingsFd = aFd;
}

void SettingsFile::SwapDiscard(int aFd)
{
    char swapFileName[kMaxFilePathSize];

    VerifyOrDie(0 == close(aFd), TY_EXIT_ERROR_ERRNO);
    GetSettingsFilePath(swapFileName, true);
    VerifyOrDie(0 == unlink(swapFileName), TY_EXIT_ERROR_ERRNO);
}

} // namespace Posix
} // namespace tiny
