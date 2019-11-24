// Satellite
#include <Panels/RendererControls.hpp>
#include <Messages.hpp>

// Imgui
#include <imgui.h>

// libc
#include <cstring>

using namespace ImGui;

RendererControls::RendererControls(const PanelSharedPtr& parent) {

}

RendererControls::~RendererControls() {
    for (char* s : _renderer_names) {
        delete s;
    }

    for (char* s : _camera_names) {
        delete s;
    }

    for (char* s : _camera_control_names) {
        delete s;
    }

    _camera_control_names.clear();
    _camera_names.clear();
    _renderer_names.clear();
}

void RendererControls::add_renderer(const char* name) {
    char* s = new char[strlen(name)];
    strcpy(s, name);
    _renderer_names.emplace_back(s);
}

void RendererControls::add_camera(const char* name) {
    char* s = new char[strlen(name)];
    strcpy(s, name);
    _camera_names.emplace_back(s);
}

void RendererControls::add_camera_control(const char* name) {
    char* s = new char[strlen(name)];
    strcpy(s, name);
    _camera_control_names.emplace_back(s);
}

void RendererControls::init() {
    add_renderer("object");
    add_renderer("terra");

    add_camera("orthographic");
    add_camera("perspective");

    add_camera_control("First person");
}

void RendererControls::draw() {
    if (BeginMainMenuBar()) {
        if (!_renderer_names.empty()) {
            if (BeginMenu("File")) {
                if (MenuItem("Add")) {
                    MessageLoadScene msg;
                    msg.path = "test";
                    SEND_MESSAGE(MSG_LOAD_SCENE, MessageLoadScene, msg);
                }

                if (MenuItem("Save")) {
                    MessageSaveRenderTarget msg;
                    msg.path = "test";
                    SEND_MESSAGE(MSG_SAVE_RENDER_TARGET, MessageSaveRenderTarget, msg);
                }

                ImGui::EndMenu();
            }

            if (BeginMenu("Window")) {
                ImGui::EndMenu();
            }

            if (BeginMenu("Console")) {
                ImGui::EndMenu();
            }

            PushItemWidth(100);
            if (Combo("Renderer", &_active_renderer, _renderer_names.data(), _renderer_names.size())) {
                _on_select_renderer();
            }
            if (Combo("Camera", &_active_camera, _camera_names.data(), _camera_names.size())) {
                _on_select_camera();
            }
            if (Combo("Camera Controls", &_active_camera_controls, _camera_control_names.data(), _camera_control_names.size())) {
                _on_select_camera_controls();
            }
        }
    
        EndMainMenuBar();
    }
}

void RendererControls::_on_select_renderer() {
    MessageSetRenderer msg;
    msg.type = _renderer_names[_active_renderer];
    SEND_MESSAGE(MSG_SET_RENDERER, MessageSetRenderer, msg);
}

void RendererControls::_on_select_camera() {
    MessageSetCamera msg;
    msg.type = _camera_names[_active_renderer];
    SEND_MESSAGE(MSG_SET_CAMERA, MessageSetCamera, msg);
}

void RendererControls::_on_select_camera_controls() {
    MessageSetCameraControls msg;
    msg.type = _camera_names[_active_renderer];
    SEND_MESSAGE(MSG_SET_CAMERA_CONTROLS, MessageSetCameraControls, msg);
}