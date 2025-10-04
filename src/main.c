#include <stdio.h>
#include "ui_core.h"
#include "ui_editors.h"

//static void register_builtin_editors(void);

int main(void) {
    ui_core_init();
    //register_builtin_editors();

    // Enter first provider
    if (!ui_nav_enter_provider(0)) { printf("No providers.\n"); return 0; }

    for (;;) {
        const ui_node_desc_t *nd = ui_nav_current_node();
        if (!nd) { printf("No node\n"); break; }
        
        // Eventual i18n
        //const char *label = nd->label_ptr ? nd->label_ptr : ui_string(locale_id, nd->label_id)

        const char *label = nd->label_ptr;
        printf("\n== %s ==\n", label);

        if (nd->type == UI_NODE_FOLDER) {
            uint32_t ids[16]; uint16_t n = ui_list_children(nd->local_id, ids, 16);
            for (uint16_t i = 0; i < n; i++) {
                const ui_node_desc_t *c = ui_get_provider(0)->vtbl->get_node(ids[i]);

                //const char *cl = c->label_ptr ? c->label_ptr : ui_string(locale_id, c->label_id)
                const char *cl = c->label_ptr;

                printf("[%u] %s\n", i, cl);
            }
            printf("Select index, b=back, q=quit: ");
            int ch = getchar(); if (ch == 'q') break; if (ch == 'b') { ui_nav_back(); continue; }
            if (ch >= '0' && ch <= '9') ui_nav_into_child((uint16_t)(ch-'0'));
        } else if (nd->type == UI_NODE_ACTION) {
            printf("(Action) press e=execute, b=back: ");
            int ch = getchar(); if (ch == 'b') { ui_nav_back(); continue; }
            if (ch == 'e') {
                ui_status_t s = ui_get_provider(0)->vtbl->invoke_action(nd->local_id);
                printf("Action status: %d\n", s);
                ui_nav_back();
            }
        } else if (nd->type == UI_NODE_VALUE) {
            printf("(Value) press e=edit, b=back: ");
            int ch = getchar(); if (ch == 'b') { ui_nav_back(); continue; }
            if (ch == 'e') {
                const ui_editor_vtbl_t *ed = ui_pick_editor(&nd->u.value.meta);
                if (!ed) { printf("No editor (fallback TBD)\n"); continue; }
                char line[64];
                if (ed->begin) ed->begin(nd);
                // For prototype, do one render + one commit
                if (ed->render) { ed->render(line, sizeof line); printf("Edit: %s\n", line); }
                if (ed->commit && ed->commit()) printf("Committed.\n");
                ui_nav_back();
            }
        }
        // ??? flush stdin for simple console loops if needed.
    }
    return 0;
}