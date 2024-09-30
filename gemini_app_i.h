#pragma once

#include "gemini_app.h"

#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/widget.h>

#include "scenes/gemini_scene.h"

typedef struct GeminiApp GeminiApp;
struct GeminiApp {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    Widget* widget;
};
