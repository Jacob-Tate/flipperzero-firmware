#include "../totp_i.h"

enum SubmenuIndex {
    SubmenuIndexRead,
    SubmenuIndexRunScript,
    SubmenuIndexSaved,
};

void totp_scene_start_submenu_callback(void* context, uint32_t index) {
    Totp* totp = (Totp*)context;

    view_dispatcher_send_custom_event(totp->view_dispatcher, index);
}

void totp_scene_start_on_enter(void* context) {
    Totp* totp = (Totp*)context;
    Submenu* submenu = totp->submenu;

    submenu_add_item(
        submenu, "Read card", SubmenuIndexRead, totp_scene_start_submenu_callback, totp);
    submenu_add_item(
        submenu, "Saved cards", SubmenuIndexSaved, totp_scene_start_submenu_callback, totp);
    submenu_add_item(
        submenu,
        "Run special action",
        SubmenuIndexRunScript,
        totp_scene_start_submenu_callback,
        totp);
    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(totp->scene_manager, TotpSceneStart));


    view_dispatcher_switch_to_view(totp->view_dispatcher, TotpViewMenu);
}

bool totp_scene_start_on_event(void* context, SceneManagerEvent event) {
    Totp* totp = (Totp*)context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexRead) {
            //scene_manager_set_scene_state(totp->scene_manager, TotpSceneStart, SubmenuIndexRead);
            //scene_manager_next_scene(totp->scene_manager, TotpSceneReadCard);
            consumed = true;
        } else if(event.event == SubmenuIndexRunScript) {
            // scene_manager_set_scene_state(
            //     totp->scene_manager, TotpSceneStart, SubmenuIndexRunScript);
            // scene_manager_next_scene(totp->scene_manager, TotpSceneScriptsMenu);
            consumed = true;
        } else if(event.event == SubmenuIndexSaved) {
            scene_manager_set_scene_state(totp->scene_manager, TotpSceneStart, SubmenuIndexSaved);
            scene_manager_next_scene(totp->scene_manager, TotpSceneFileSelect);
            consumed = true;
        }
    }
    return consumed;
}

void totp_scene_start_on_exit(void* context) {
    Totp* totp = (Totp*)context;

    submenu_free(totp->submenu);
}
