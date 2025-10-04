#include <stddef.h>
#include "ui_core.h"

/**
 * @file ui_core.c
 * @brief Implementation of provider registry and menu navigation.
 * 
 * Providers are declared through the X-macro file `providers.def`. This file
 * constructs the provider table, maintains navigation state, and exposes basic
 * list and traversal utilities used by higher-level UI layers.
 */

// Declare providers from providers.def
#define X(name) extern const ui_menu_provider_t name;
#include "providers.def"
#undef X

#define X(name) &name,
/** Static registry table; order matches providers.def */
static const ui_menu_provider_t *g_providers[] = {
#include "providers.def"
};
#undef X

/** Total number of registered providers. */
static const uint8_t g_provider_count = sizeof g_providers / sizeof g_providers[0];

// -----------------

// Nav state
static uint8_t cur_provider = 0xFF;
static uint32_t cur_local_id = 0;

bool ui_core_init(void) {
    // ABI filter, potentially filter out incompatible modules?
    return true;
}

uint8_t ui_provider_count(void) { return g_provider_count; }
const ui_menu_provider_t* ui_get_provider(uint8_t i) {
    return (i < g_provider_count) ? g_providers[i] : NULL;
}

bool ui_nav_enter_provider(uint8_t idx) {
    if (idx >= g_provider_count) return false;
    cur_provider = idx;
    cur_local_id = g_providers[idx]->vtbl->get_root();
    return true;
}
const ui_node_desc_t* ui_nav_current_node(void) {
    if (cur_provider == 0xFF) return NULL;
    return g_providers[cur_provider]->vtbl->get_node(cur_local_id);
}

uint16_t ui_list_children(uint32_t folder_id, uint32_t *out_ids, uint16_t max) {
    if (cur_provider == 0xFF) return 0;
    const ui_menu_provider_t *p = g_providers[cur_provider];

    // Enumerate static children from descriptor array
    uint16_t n = 0;
    for (uint16_t i = 0; i < p->node_count && n < max; ++i) {
        const ui_node_desc_t *nd = &p->nodes[i];
        if (nd->parent_local_id == folder_id) out_ids[n++] = nd->local_id;
    }

    // Append dynamic children if any
    if (p->vtbl->children_begin && p->vtbl->children_next) {
        ui_iter_t it;
        uint32_t id;
        if (p->vtbl->children_begin(folder_id, &it)) {
            while (n < max && p->vtbl->children_next(folder_id, &it, &id)) {
                out_ids[n++] = id;
            }
        }
    }
    return n;
}

// Example: move "into" by visible index on current folder
bool ui_nav_into_child(uint16_t idx) {
    const ui_node_desc_t *cur = ui_nav_current_node();
    if (!cur || cur->type != UI_NODE_FOLDER) return false;
    uint32_t ids[16];
    uint16_t cnt = ui_list_children(cur->local_id, ids, 16);
    if (idx >= cnt) return false;
    cur_local_id = ids[idx];
    return true;
}
bool ui_nav_back(void) {
    if (cur_provider == 0xFF) return false;
    const ui_menu_provider_t *p = g_providers[cur_provider];
    const ui_node_desc_t *nd = p->vtbl->get_node(cur_local_id);
    if (!nd || nd->parent_local_id == 0xFFFFFFFF) return false;
    cur_local_id = nd->parent_local_id;
    return true;
}