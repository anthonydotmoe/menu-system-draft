#pragma once
#include <stdint.h>
#include <stdbool.h>

/**
 * @file ui_menu_abi.h
 * @brief Core ABI definitions shared by the UI core and all modules.
 *
 * This header defines the structures, enumerations, and function pointer
 * interfaces used for communication between the central UI system
 * and individual feature modules (menu providers).  
 * All modules must conform to this ABI to integrate into the menu system.
 */

#define UI_MENU_ABI_VERSION 1

/**
 * @enum ui_node_type_t
 * @brief Classifies the function of a menu node.
 */
typedef enum {
    UI_NODE_FOLDER = 0,
    UI_NODE_ACTION = 1,
    UI_NODE_VALUE = 2,
    UI_NODE_DYNAMIC = 3,
} ui_node_type_t;

/**
 * @enum ui_status_t
 * @brief Generic status codes returned by provider functions
 */
typedef enum {
    UI_OK = 0,
    UI_ERR = -1,
    UI_DENIED = -2,
    UI_BUSY = 1,
} ui_status_t;

/**
 * @enum ui_vt_t
 * @brief Describes the type of value stored in a UI_VALUE node.
 */
typedef enum {
    UI_VT_BOOL = 1,
    UI_VT_I32 = 2,
    UI_VT_ENUM = 3,
    UI_VT_TEXT = 4,
    UI_VT_U64 = 5,
} ui_vt_t;

/**
 * @enum ui_unit_t
 * @brief Physical or semantic unit for a value (optional metadata)
 */
typedef enum {
    UI_UNIT_NONE = 0,
    UI_UNIT_HZ = 1,
    UI_UNIT_MS = 2,
    UI_UNIT_DBM = 3,
    UI_UNIT_PERCENT = 4,
} ui_unit_t;

/**
 * @enum ui_hint_t
 * @brief UI hint used by the editor selector to pick specialized widgets.
 */
typedef enum {
    UI_HINT_NONE = 0,
    UI_HINT_FREQUENCY = 1,
    UI_HINT_PASSWORD = 2,
    UI_HINT_PERCENT = 3,
} ui_hint_t;

/**
 * @struct ui_value_meta_t
 * @brief Schema information for a configurable value.
 * 
 * This structure defines bounds, enumeration tables, or text constraints
 * used by the UI to render and validate editors.
 */
typedef struct {
    ui_vt_t vt;         /**< Value type. */
    ui_unit_t unit;     /**< Unit for display/formatting. */
    ui_hint_t hint;     /**< Hint for preferred editor. */

    union {
        struct { int32_t min,max,step; } i32;   /**< Integer constraints. */
        struct { uint64_t min,max,step; } u64;  /**< Unsigned constraints. */
        struct { const char *const *names; uint16_t count; } enm;   /**< Enum labels. */
        struct { uint16_t min_len, max_len; const char *regex; } text;  /**< Text limits. */
    };

    bool (*validate)(const void *candidate);    /**< Optional pure validation callback. */
    int  (*format)(char *buf, uint16_t n, const void* value); /**< Optional formatter */
} ui_value_meta_t;

/**
 * @struct ui_node_desc_t
 * @brief Static description of a single menu node.
 * 
 * Instances of this structure are normally placed in flash memory
 * inside each module's descriptor table.
 */
typedef struct {
    ui_node_type_t type;        /**< Node classification */
    uint32_t local_id;          /**< Unique within the provider. */
    uint32_t parent_local_id;   /**< ID of parent node or 0xFFFFFFFF for root. */
    uint16_t label_id;          /**< Index into string table, 0 if using label_ptr */
    const char *label_ptr;      /**< Direct label string (optional). */
    uint16_t flags;             /**< Reserved for disabled/hidden bits. */
    union {
        struct { /* none */ } folder;           /**< Placeholder for folder nodes. */
        struct { uint16_t action_id; } action;  /**< Action identifier. */
        struct { ui_value_meta_t meta; } value; /**< Metadata for VALUE nodes. */
    } u;
} ui_node_desc_t;

struct ui_menu_provider_vtbl;
/**
 * @struct ui_menu_provider_t
 * @brief Top-level registration record exported by each module.
 * 
 * The UI core discovers all providers at startup via an explicit registry
 * table. Each provider exposes one or more nodes and the functions needed to
 * interact with them.
 */
typedef struct ui_menu_provider {
    uint16_t provider_id;           /**< Unique provider identifier */
    uint16_t abi_version;           /**< Must match UI_MENU_ABI_VERSION */
    uint32_t capabilities;          /**< Reserved for capability flags. */
    const struct ui_menu_provider_vtbl *vtbl;   /**< Pointer to virtual table */
    const ui_node_desc_t *nodes;    /**< Static node array in flash. */
    uint16_t node_count;            /**< Number of entries in @ref nodes. */
} ui_menu_provider_t;

typedef struct ui_iter {
    uintptr_t opaque; // provider-defined iterator token
} ui_iter_t;

/**
 * @struct ui_menu_provider_vtbl_t
 * @brief Virtual table implemented by each module.
 * 
 * These callbacks provide access to the module's menu tree
 * and implement behavior for actions and editable values.
 */
typedef struct ui_menu_provider_vtbl {
    uint32_t (*get_root)(void);
    const ui_node_desc_t* (*get_node)(uint32_t local_id);
    // dynamic listing (optional)
    bool (*children_begin)(uint32_t folder_id, ui_iter_t *it);
    bool (*children_next)(uint32_t folder_id, ui_iter_t *it, uint32_t *out_child_id);
    // actions/values
    ui_status_t (*invoke_action)(uint32_t local_id);
    ui_status_t (*value_get_i32)(uint32_t local_id, int32_t *out);
    ui_status_t (*value_set_i32)(uint32_t local_id, int32_t v);
    ui_status_t (*value_get_u64)(uint32_t local_id, uint64_t *out);
    ui_status_t (*value_set_u64)(uint32_t local_id, uint64_t v);
    ui_status_t (*value_get_text)(uint32_t local_id, char *buf, uint16_t n);
    ui_status_t (*value_set_text)(uint32_t local_id, char *buf, const char *s);
} ui_menu_provider_vtbl_t;