// const data module for FM

#include "ui_menu_abi.h"

enum {
    FM_ROOT       = 1,
    FM_CTCSS_TONE = 2,
    FM_CTCSS_EN   = 3,
};

static const char* const enable_names[] = {
    "None",
    "Decode",
    "Encode",
    "Both",
};

static const char* const ctcss_tones[] = {
    "67.0", "69.3", "71.9", "etc."
};

static const ui_node_desc_t NODES[] = {
    { UI_NODE_FOLDER, FM_ROOT,       0xFFFFFFFF, 0, "FM",         0, {{0}} },
    { UI_NODE_VALUE,  FM_CTCSS_TONE, FM_ROOT,    0, "CTCSS Tone", 0,
        .u.value = { .meta = {
            .vt = UI_VT_ENUM, .unit=UI_UNIT_HZ, .hint=UI_HINT_NONE,
            .enm = { ctcss_tones, 4 },
            .validate = 0, .format = 0
    }}},
    { UI_NODE_VALUE,  FM_CTCSS_EN,   FM_ROOT,    0, "CTCSS En.",  0,
        .u.value = { .meta = {
            .vt = UI_VT_ENUM, .unit=UI_UNIT_NONE, .hint=UI_HINT_NONE,
            .enm = { enable_names, 4 },
            .validate = 0, .format = 0
    }}},
};

static uint32_t get_root(void) { return FM_ROOT; }
static const ui_node_desc_t* get_node(uint32_t id) {
    for (unsigned i = 0; i < sizeof NODES/sizeof NODES[0]; ++i)
        if (NODES[i].local_id == id)
            return &NODES[i];
    return 0;
}
static ui_status_t invoke_action(uint32_t id) {
    // No actions defined for this module
    return UI_ERR;
}

static const ui_menu_provider_vtbl_t V = {
    .get_root=get_root,
    .get_node=get_node,
    .children_begin=0, .children_next=0,
    .invoke_action=invoke_action,
    .value_get_i32=0, .value_set_i32=0, // Not defined
    .value_get_u64=0, .value_set_u64=0, .value_get_text=0, .value_set_text=0
};

const ui_menu_provider_t FM_MENU_PROVIDER = {
    .provider_id=0x0102, .abi_version=UI_MENU_ABI_VERSION,
    .capabilities = 0,
    .vtbl = &V,
    .nodes=NODES,
    .node_count=sizeof(NODES)/sizeof(NODES[0])
};