#include "../gemini_app_i.h"

#define REFRESH_RATE 250

static FuriString* contents = NULL;
static FuriString* line = NULL;

void gemini_scene_receive_serial_timer_callback(void* context) {
    GeminiApp* app = context;
    furi_string_reset(line);
    bool read = uart_helper_read(app->uart_helper, line, 0);
    if (read) {
        text_box_set_text(app->text_box, "");
        size_t new_len = furi_string_size(contents) + furi_string_size(line);
        if(new_len >= 4096) {
            furi_string_right(contents, new_len / 2);
        }
        furi_string_cat(contents, line);
        text_box_set_text(app->text_box, furi_string_get_cstr(contents));
    }
}

void gemini_scene_receive_serial_on_enter(void* context) {
    GeminiApp* app = context;
    contents = furi_string_alloc();
    line = furi_string_alloc();
    text_box_set_text(app->text_box, "");
    text_box_set_focus(app->text_box, TextBoxFocusEnd);
    app->timer = furi_timer_alloc(gemini_scene_receive_serial_timer_callback, FuriTimerTypePeriodic, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, GeminiViewTextBox);
    furi_timer_start(app->timer, REFRESH_RATE);
}

bool gemini_scene_receive_serial_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false; // event not handled.
}

void gemini_scene_receive_serial_on_exit(void* context) {
    GeminiApp* app = context;
    furi_timer_stop(app->timer);
    furi_string_free(line);
    furi_string_free(contents);

    // NOTE: The "stop" command doesn't seem to be implemented by the ESP32 firmware?
    uart_helper_send(app->uart_helper, "stop", 0);
}