#include "gemini_app_i.h"

static bool gemini_app_custom_callback(void* context, uint32_t custom_event) {
    furi_assert(context);
    GeminiApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, custom_event);
}

static bool gemini_app_back_event_callback(void* context) {
    furi_assert(context);
    GeminiApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static GeminiApp* gemini_app_alloc() {
    GeminiApp* app = malloc(sizeof(GeminiApp));
    app->scene_manager = scene_manager_alloc(&gemini_scene_handlers, app);
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, gemini_app_custom_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, gemini_app_back_event_callback);
    app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, GeminiViewSubmenu, submenu_get_view(app->submenu));
    app->widget = widget_alloc();
    view_dispatcher_add_view(app->view_dispatcher, GeminiViewWidget, widget_get_view(app->widget));
    return app;
}

static void gemini_app_free(GeminiApp* app) {
    furi_assert(app);
    view_dispatcher_remove_view(app->view_dispatcher, GeminiViewSubmenu);
    view_dispatcher_remove_view(app->view_dispatcher, GeminiViewWidget);
    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);
    submenu_free(app->submenu);
    widget_free(app->widget);
    free(app);
}

int32_t gemini_app(void* p) {
    UNUSED(p);
    GeminiApp* app = gemini_app_alloc();

    Gui* gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    scene_manager_next_scene(app->scene_manager, GeminiSceneMainMenu);
    view_dispatcher_run(app->view_dispatcher);

    gemini_app_free(app);
    return 0;
}
