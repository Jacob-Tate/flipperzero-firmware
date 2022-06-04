#include "../totp_i.h"
#include "../totp_device.h"

void totp_scene_file_select_on_enter(void* context) {
    Totp* app = (Totp*)context;
    if(totp_file_select(app->dev)) {
        //scene_manager_next_scene(app->scene_manager, TotpSceneSavedMenu);
    } else {
        scene_manager_search_and_switch_to_previous_scene(app->scene_manager, TotpSceneStart);
    }
}

bool totp_scene_file_select_on_event(void* context, SceneManagerEvent event) {
    return false;
}

void totp_scene_file_select_on_exit(void* context) {
}
