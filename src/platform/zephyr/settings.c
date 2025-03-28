// SPDX-FileCopyrightText: Copyright 2025 Clever Design (Switzerland) GmbH
// SPDX-License-Identifier: Apache-2.0

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/random/random.h>
#include <zephyr/settings/settings.h>

#include <tiny/error.h>
#include <tiny/instance.h>
#include <tiny/platform/toolchain.h>

/* #include <tiny/platform/settings.h> */
#define CONFIG_TY_L2_LOG_LEVEL LOG_LEVEL_DBG
LOG_MODULE_REGISTER(net_tyPlat_settings, CONFIG_TY_L2_LOG_LEVEL);

#define TY_SETTINGS_ROTY_KEY "tiny"
#define TY_SETTINGS_MAX_PATH_LEN 32

struct tiny_setting_delete_ctx
{
    /* Setting subtree to delete. */
    const char *subtree;

    /* Current entry index, used to iterate over multiple setting
     * instances.
     */
    int index;

    /* Target index to delete. -1 to delete entire subtree. */
    int target_index;

    /* Operation result. */
    int status;

    /* Indicates if delete subtree root. */
    bool delete_subtree_root;
};

static int tiny_setting_delete_cb(const char *key, size_t len, settings_read_cb read_cb, void *cb_arg, void *param)
{
    int                             ret;
    char                            path[TY_SETTINGS_MAX_PATH_LEN];
    struct tiny_setting_delete_ctx *ctx = (struct tiny_setting_delete_ctx *)param;

    ARG_UNUSED(len);
    ARG_UNUSED(read_cb);
    ARG_UNUSED(cb_arg);

    if ((ctx->target_index != -1) && (ctx->target_index != ctx->index))
    {
        ctx->index++;
        return 0;
    }

    if (key == NULL && ctx->delete_subtree_root == false)
    {
        return 0;
    }

    ret = snprintk(path, sizeof(path), "%s%s%s", ctx->subtree, key ? "/" : "", key ? key : "");
    __ASSERT(ret < sizeof(path), "Setting path buffer too small.");

    LOG_DBG("Removing: %s", path);

    ret = settings_delete(path);
    if (ret != 0)
    {
        LOG_ERR("Failed to remove setting %s, ret %d", path, ret);
        __ASSERT_NO_MSG(false);
    }

    ctx->status = 0;

    if (ctx->target_index == ctx->index)
    {
        /* Break the loop on index match, otherwise it was -1
         * (delete all).
         */
        return 1;
    }

    return 0;
}

static int tiny_setting_delete_subtree(int key, int index, bool delete_subtree_root)
{
    int                            ret;
    char                           subtree[TY_SETTINGS_MAX_PATH_LEN];
    struct tiny_setting_delete_ctx delete_ctx = {
        .subtree             = subtree,
        .status              = -ENOENT,
        .target_index        = index,
        .delete_subtree_root = delete_subtree_root,
    };

    if (key == -1)
    {
        ret = snprintk(subtree, sizeof(subtree), "%s", TY_SETTINGS_ROTY_KEY);
    }
    else
    {
        ret = snprintk(subtree, sizeof(subtree), "%s/%x", TY_SETTINGS_ROTY_KEY, key);
    }
    __ASSERT(ret < sizeof(subtree), "Setting path buffer too small.");

    ret = settings_load_subtree_direct(subtree, tiny_setting_delete_cb, &delete_ctx);
    if (ret != 0)
    {
        LOG_ERR("Failed to delete OT subtree %s, index %d, ret %d", subtree, index, ret);
        __ASSERT_NO_MSG(false);
    }

    return delete_ctx.status;
}

static int tiny_setting_exists_cb(const char *key, size_t len, settings_read_cb read_cb, void *cb_arg, void *param)
{
    bool *exists = (bool *)param;

    ARG_UNUSED(len);
    ARG_UNUSED(read_cb);
    ARG_UNUSED(cb_arg);
    ARG_UNUSED(key);

    *exists = true;

    return 1;
}

static bool tiny_setting_exists(const char *path)
{
    bool exists = false;

    (void)settings_load_subtree_direct(path, tiny_setting_exists_cb, &exists);

    return exists;
}

struct tiny_setting_read_ctx
{
    /* Buffer for the setting. */
    uint8_t *value;

    /* Buffer length on input, setting length read on output. */
    uint16_t *length;

    /* Current entry index, used to iterate over multiple setting
     * instances.
     */
    int index;

    /* Target instance to read. */
    int target_index;

    /* Operation result. */
    int status;
};

static int tiny_setting_read_cb(const char *key, size_t len, settings_read_cb read_cb, void *cb_arg, void *param)
{
    int                           ret;
    struct tiny_setting_read_ctx *ctx = (struct tiny_setting_read_ctx *)param;

    ARG_UNUSED(len);
    ARG_UNUSED(read_cb);
    ARG_UNUSED(cb_arg);

    if (ctx->target_index != ctx->index)
    {
        ctx->index++;
        return 0;
    }

    /* Found setting, break the loop. */

    if ((ctx->value == NULL) || (ctx->length == NULL))
    {
        goto out;
    }

    if (*(ctx->length) < len)
    {
        len = *(ctx->length);
    }

    ret = read_cb(cb_arg, ctx->value, len);
    if (ret <= 0)
    {
        LOG_ERR("Failed to read the setting, ret: %d", ret);
        ctx->status = -EIO;
        return 1;
    }

out:
    if (ctx->length != NULL)
    {
        *(ctx->length) = len;
    }

    ctx->status = 0;

    return 1;
}

/* Tiny APIs */

void tyPlatSettingsInit(tinyInstance *aInstance, const uint16_t *aSensitiveKeys, uint16_t aSensitiveKeysLength)
{
    int ret;

    ARG_UNUSED(aInstance);
    ARG_UNUSED(aSensitiveKeys);
    ARG_UNUSED(aSensitiveKeysLength);

    ret = settings_subsys_init();
    if (ret != 0)
    {
        LOG_ERR("settings_subsys_init failed (ret %d)", ret);
    }
}

tinyError tyPlatSettingsGet(tinyInstance *aInstance, uint16_t aKey, int aIndex, uint8_t *aValue, uint16_t *aValueLength)
{
    int                          ret;
    char                         path[TY_SETTINGS_MAX_PATH_LEN];
    struct tiny_setting_read_ctx read_ctx = {
        .value = aValue, .length = (uint16_t *)aValueLength, .status = -ENOENT, .target_index = aIndex};

    ARG_UNUSED(aInstance);

    LOG_DBG("%s Entry aKey %u aIndex %d", __func__, aKey, aIndex);

    ret = snprintk(path, sizeof(path), "%s/%x", TY_SETTINGS_ROTY_KEY, aKey);
    __ASSERT(ret < sizeof(path), "Setting path buffer too small.");

    ret = settings_load_subtree_direct(path, tiny_setting_read_cb, &read_ctx);
    if (ret != 0)
    {
        LOG_ERR("Failed to load OT setting aKey %d, aIndex %d, ret %d", aKey, aIndex, ret);
    }

    if (read_ctx.status != 0)
    {
        LOG_DBG("aKey %u aIndex %d not found", aKey, aIndex);
        return TY_ERROR_NOT_FOUND;
    }

    return TY_ERROR_NONE;
}

tinyError tyPlatSettingsSet(tinyInstance *aInstance, uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength)
{
    int  ret;
    char path[TY_SETTINGS_MAX_PATH_LEN];

    ARG_UNUSED(aInstance);

    LOG_DBG("%s Entry aKey %u", __func__, aKey);

    (void)tiny_setting_delete_subtree(aKey, -1, false);

    ret = snprintk(path, sizeof(path), "%s/%x", TY_SETTINGS_ROTY_KEY, aKey);
    __ASSERT(ret < sizeof(path), "Setting path buffer too small.");

    ret = settings_save_one(path, aValue, aValueLength);
    if (ret != 0)
    {
        LOG_ERR("Failed to store setting %d, ret %d", aKey, ret);
        return TY_ERROR_NO_BUFS;
    }

    return TY_ERROR_NONE;
}

tinyError tyPlatSettingsAdd(tinyInstance *aInstance, uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength)
{
    int  ret;
    char path[TY_SETTINGS_MAX_PATH_LEN];

    ARG_UNUSED(aInstance);

    LOG_DBG("%s Entry aKey %u", __func__, aKey);

    do
    {
        ret = snprintk(path, sizeof(path), "%s/%x/%08x", TY_SETTINGS_ROTY_KEY, aKey, sys_rand32_get());
        __ASSERT(ret < sizeof(path), "Setting path buffer too small.");
    } while (tiny_setting_exists(path));

    ret = settings_save_one(path, aValue, aValueLength);
    if (ret != 0)
    {
        LOG_ERR("Failed to store setting %d, ret %d", aKey, ret);
        return TY_ERROR_NO_BUFS;
    }

    return TY_ERROR_NONE;
}

tinyError tyPlatSettingsDelete(tinyInstance *aInstance, uint16_t aKey, int aIndex)
{
    int ret;

    ARG_UNUSED(aInstance);

    LOG_DBG("%s Entry aKey %u aIndex %d", __func__, aKey, aIndex);

    ret = tiny_setting_delete_subtree(aKey, aIndex, true);
    if (ret != 0)
    {
        LOG_DBG("Entry not found aKey %u aIndex %d", aKey, aIndex);
        return TY_ERROR_NOT_FOUND;
    }

    return TY_ERROR_NONE;
}

void tyPlatSettingsWipe(tinyInstance *aInstance)
{
    ARG_UNUSED(aInstance);

    (void)tiny_setting_delete_subtree(-1, -1, true);
}

void tyPlatSettingsDeinit(tinyInstance *aInstance)
{
    ARG_UNUSED(aInstance);
}
