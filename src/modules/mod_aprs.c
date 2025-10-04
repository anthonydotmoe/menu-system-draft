// const data module for APRS

#include "ui_menu_abi.h"

enum {
    APRS_ROOT = 1,
    APRS_CFG = 2,
    APRS_RATE = 3,
    APRS_TESTTX = 4,
    APRS_SSID = 5,
};

static const char* const rate_names[] = {
    "Fast",
    "Normal",
    "Slow",
};

static const char* const ax25_ssids[] = {
    "-0",
    "-1",
    "-2",
    "-3",
    "-4",
    "-5",
    "-6",
    "-7",
    "-8",
    "-9",
    "-10",
    "-11",
    "-12",
    "-13",
    "-14",
    "-15",
};

static const ui_node_desc_t NODES[] = {
    { UI_NODE_FOLDER, APRS_ROOT,   0xFFFFFFFF, 1000, "APRS",        0, {{0}} },
    { UI_NODE_FOLDER, APRS_CFG,    APRS_ROOT,  1001, "Config",      0, {{0}} },
    { UI_NODE_VALUE,  APRS_RATE,   APRS_CFG,   1002, "Beacon rate", 0,
        .u.value = { .meta = {
            .vt = UI_VT_ENUM, .unit=UI_UNIT_NONE, .hint=UI_HINT_NONE,
            .enm = { rate_names, 3 },
            .validate = 0, .format = 0
    }}},
    { UI_NODE_VALUE,  APRS_SSID,   APRS_CFG,   1003, "SSID",        0,
        .u.value = { .meta = {
            .vt = UI_VT_ENUM, .unit=UI_UNIT_NONE, .hint=UI_HINT_NONE,
            .enm = { ax25_ssids, 16 },
            .validate = 0, .format = 0
    }}},
    { UI_NODE_ACTION, APRS_TESTTX, APRS_ROOT,  1004, "Test TX",     0,
        .u.action = { .action_id = 1  }
    },
};

static uint32_t get_root(void) { return APRS_ROOT; }
static const ui_node_desc_t* get_node(uint32_t id) {
    for (unsigned i = 0; i < sizeof NODES/sizeof NODES[0]; ++i)
        if (NODES[i].local_id == id)
            return &NODES[i];
    return 0;
}
static ui_status_t invoke_action(uint32_t id) {
    if (id == APRS_TESTTX) {
        /* trigger something, set flag, say hello? */
        return UI_OK;
    }
    return UI_ERR;
}
static ui_status_t value_get_i32(uint32_t id, int32_t *out) {
    if (id == APRS_RATE) { *out = 1; return UI_OK; } return UI_ERR;
}
static ui_status_t value_set_i32(uint32_t id, int32_t v) {
    if (id == APRS_RATE && v >= 0 && v < 3) {
        /* persist the value somewhere */
        return UI_OK;
    }
    return UI_DENIED;
}

static const ui_menu_provider_vtbl_t V = {
    .get_root=get_root,
    .get_node=get_node,
    .children_begin=0, .children_next=0,
    .invoke_action=invoke_action,
    .value_get_i32=value_get_i32,
    .value_set_i32=value_set_i32,
    .value_get_u64=0, .value_set_u64=0, .value_get_text=0, .value_set_text=0
};

const ui_menu_provider_t APRS_MENU_PROVIDER = {
    .provider_id=0x0101, .abi_version=UI_MENU_ABI_VERSION,
    .capabilities = 0,
    .vtbl = &V,
    .nodes=NODES,
    .node_count=sizeof(NODES)/sizeof(NODES[0])
};