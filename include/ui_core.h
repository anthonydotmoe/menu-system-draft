#pragma once

/**
 * @file ui_core.h
 * @brief Core navigation and provider registry interface.
 * 
 * This module contains the global list of menu providers,
 * navigation state, and child enumeration utilities.
 * It has no knowledge of rendering or input hardware.
 */

#include "ui_menu_abi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Pair identifying a specific node within a provider.
 */
typedef struct {
    uint8_t provider_index; // index into registry
    uint32_t local_id;      // node within provider
} ui_path_entry_t;

/**
 * @brief Initialize the UI core and provider registry.
 * @return true on success.
 */
bool ui_core_init(void);

/**
 * @brief Return the number of registered menu providers.
 */
uint8_t ui_provider_count(void);

/**
 * @brief Get a provider handle by index.
 * @param index Provider index (0..count-1).
 * @return Pointer to provider or NULL if out of range.
 */
const ui_menu_provider_t* ui_get_provider(uint8_t index);

/**
 * @brief Enter the root menu of a provider.
 * @param index Provider index (0..count-1)
 * @return true if entered successfully.
 */
bool ui_nav_enter_provider(uint8_t provider_index);

/**
 * @brief Move navigation focus into the Nth child of the current folder.
 * @param child_idx_on_page Zero-based child index as displayed.
 * @return true if the move succeeded.
 */
bool ui_nav_into_child(uint16_t child_idx_on_page); // by visual index

/**
 * @brief Move navigation focus back to the parent node.
 * @return true if moved up, false if already at root.
 */
bool ui_nav_back(void);

/**
 * @brief Get a pointer to the node descriptor currently in focus.
 */
const ui_node_desc_t* ui_nav_current_node(void);

/**
 * @brief Enumerate all static and dynamic children of a folder node.
 * 
 * @param folder_local_id Local ID of the folder node.
 * @param out_ids Output buffer for child IDs.
 * @param max Maximum number of IDs to write.
 * @return Number of children written to @p out_ids
 */
uint16_t ui_list_children(uint32_t folder_local_id, uint32_t *out_ids, uint16_t max);


#ifdef __cplusplus
}
#endif