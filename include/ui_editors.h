#pragma once
#include "ui_menu_abi.h"

typedef struct {
    // Editor interface
    void (*begin)(const ui_node_desc_t*);
    void (*event)(int keycode);             // Up/Down/Left/Right/Enter/Back
    void (*render)(char *line, int n);      // write preview
    bool (*commit)(void);                   // calls provider set()
    void (*cancel)(void);
} ui_editor_vtbl_t;

typedef struct {
    ui_vt_t vt;
    ui_unit_t unit;     // or UI_UNIT_NONE as wildcard
    ui_hint_t hint;     // or UI_HINT_NONE as wildcard
    const ui_editor_vtbl_t *vtbl;
    uint8_t priority;
} ui_editor_class_t;

// Selector & registry
bool ui_register_editor(const ui_editor_class_t *cls);
const ui_editor_vtbl_t *ui_pick_editor(const ui_value_meta_t *meta);