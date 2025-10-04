#include <stddef.h>
#include "ui_editors.h"

// fixed small registry
#ifndef UI_MAX_EDITORS
#define UI_MAX_EDITORS 12
#endif
static const ui_editor_class_t *g_editors[UI_MAX_EDITORS];
static uint8_t g_editor_count = 0;

bool ui_register_editor(const ui_editor_class_t *cls) {
    if (g_editor_count >= UI_MAX_EDITORS) return false;
    g_editors[g_editor_count++] = cls;
    return true;
}

static int score(const ui_editor_class_t *e, const ui_value_meta_t *m) {
    int s = 0;
    if (e->vt == m->vt) s += 4; else return -1; // type must match
    if (e->hint == m->hint) s += 2;
    if (e->unit == m->unit) s += 1;
    return s*10 + e->priority; // tiebreaker
}
const ui_editor_vtbl_t* ui_pick_editor(const ui_value_meta_t *meta) {
    int best = -1;
    const ui_editor_vtbl_t *out = NULL;
    for (uint8_t i = 0; i < g_editor_count; i++) {
        int sc = score(g_editors[i], meta);
        if (sc > best) { best = sc; out = g_editors[i]->vtbl; }
    }
    return out; // can be NULL
}