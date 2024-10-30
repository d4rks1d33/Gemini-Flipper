#include "../gemini_app_i.h"

#define TEXT_BUFFER_SIZE 64
static char text_buffer[TEXT_BUFFER_SIZE];
static bool name_command_sent = false; // Nueva variable para verificar si el comando ya fue enviado

enum {
    GeminiSceneChangeNameEventOk,
} GeminiSceneChangeNameEvent;

void gemini_scene_change_name_text_input_callback(void* context) {
    GeminiApp* app = context;
    scene_manager_handle_custom_event(app->scene_manager, GeminiSceneChangeNameEventOk);
}

bool gemini_app_send_new_name_command(GeminiApp* app) {
    const char* command = "newname";
    bool sent = false;

    // Solo envía el comando si aún no ha sido enviado
    if(!name_command_sent) {
        uart_helper_send(app->uart_helper, command, strlen(command));
        name_command_sent = true; // Marcar como enviado
        sent = true;
    }

    return sent;
}

void gemini_scene_change_name_on_enter(void* context) {
    GeminiApp* app = context;

    // Enviar el comando "newname" solo una vez
    gemini_app_send_new_name_command(app);

    text_input_set_header_text(app->text_input, "Enter your name");
    text_input_set_minimum_length(app->text_input, 1);
    text_buffer[0] = '\0';
    text_input_set_result_callback(app->text_input, gemini_scene_change_name_text_input_callback, app, text_buffer, TEXT_BUFFER_SIZE, true);
    view_dispatcher_switch_to_view(app->view_dispatcher, GeminiViewTextInput);
}

bool gemini_scene_change_name_on_event(void* context, SceneManagerEvent event) {
    GeminiApp* app = context;
    bool consumed = false;
    if (event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
            case GeminiSceneChangeNameEventOk:
                uart_helper_send(app->uart_helper, text_buffer, strlen(text_buffer));
                gemini_scene_receive_serial_set_next(app, GeminiSceneMainMenu); // Cambiar a escena principal
                scene_manager_search_and_switch_to_another_scene(app->scene_manager, GeminiSceneReceiveSerial);
                consumed = true;
                break;
        }
    }

    return consumed;
}

void gemini_scene_change_name_on_exit(void* context) {
    // Reiniciar name_command_sent para que al reingresar desde el menú se vuelva a enviar el comando
    name_command_sent = false;
    UNUSED(context);
}
