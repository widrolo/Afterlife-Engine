#include "Viewport.h"

#include "Editor/Types/EditorState.h"
#include "Engine/Core/Handlers/RenderHandler.h"
#include "Engine/Core/System/Iris.h"
#include "Engine/imgui/ImGuizmo.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Engine/Core/System/Haptic.h"
#include "Game/Gameplay/Freecam.h"

using namespace WEditor;

void Viewport::Setup()
{
    m_widgetName = "Viewport";
    m_windowFlags = ImGuiWindowFlags_NoResize;
}

void Viewport::RenderInternal()
{
    EditorState::ViewportSelected = ImGui::IsWindowFocused();

    ImVec2 viewportPos = ImGui::GetCursorScreenPos();
    ImVec2 viewportSize = ImGui::GetContentRegionAvail();

    Iris::FramebufferHandle fb = WEngine::CoreSystems::GetRenderHandler()->EditorGetViewportFramebuffer();
    auto image = Iris::GetFramebufferImGuiImage(fb);
    ImGui::Image(image.GetValue(), viewportSize);

    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);

    if (Haptic::GetDebugKeyHeld(5))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    if (Haptic::GetDebugKeyHeld(6))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    if (Haptic::GetDebugKeyHeld(7))
        mCurrentGizmoOperation = ImGuizmo::SCALE;
    if (Haptic::GetDebugKeyHeld(9))
        mCurrentGizmoMode = ImGuizmo::WORLD;
    if (Haptic::GetDebugKeyHeld(9))
        mCurrentGizmoMode = ImGuizmo::LOCAL;

    // vibe coded bullshit doesnt even work and now i have to fix it.

    if (EditorState::SelectedEntry == nullptr)
        return;

    auto* renderHandler = WEngine::CoreSystems::GetRenderHandler();

    const WEngine::Transform& cam = renderHandler->GetRenderedCameraTransform();
    const glm::mat4& cameraProjection = renderHandler->GetProjectionMatrix();

    glm::quat camRot(cam.rotation.w, -cam.rotation.x, cam.rotation.y, -cam.rotation.z);
    glm::mat4 cameraView = glm::mat4_cast(glm::conjugate(camRot));
    cameraView = glm::translate(cameraView, glm::vec3(-cam.position.x, -cam.position.y, -cam.position.z));

    WEngine::Transform entryTransform = EditorState::SelectedEntry->transform;
    entryTransform.position.y = 0 - entryTransform.position.y;
    glm::mat4 matrix = WEngine::RenderHandler::CalcModelMatrixGLM(entryTransform);

    glm::mat4 deltaMatrix = glm::mat4(1.0f);
    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(viewportPos.x, viewportPos.y, viewportSize.x, viewportSize.y);
    if (ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
        mCurrentGizmoOperation, mCurrentGizmoMode, glm::value_ptr(matrix), glm::value_ptr(deltaMatrix)))
    {
        WEngine::Transform& transform = EditorState::SelectedEntry->transform;

        transform.position = { matrix[3][0], matrix[3][1], matrix[3][2] };

        glm::vec3 scale = {
            glm::length(glm::vec3(matrix[0])),
            glm::length(glm::vec3(matrix[1])),
            glm::length(glm::vec3(matrix[2]))
        };
        transform.size = { scale.x, scale.y, scale.z };

        if (mCurrentGizmoOperation == ImGuizmo::ROTATE)
        {
            // the engine renders the world Y-mirrored, which flips the rotation
            // direction of the x and z axes (y is untouched). mirror the delta
            // quaternion back so the object rotates the way the gizmo is dragged.
            glm::quat prevRotation(entryTransform.rotation.w, entryTransform.rotation.x,
                entryTransform.rotation.y, entryTransform.rotation.z);
            glm::quat deltaRotation = glm::quat_cast(glm::mat3(deltaMatrix));
            deltaRotation.x = -deltaRotation.x;
            deltaRotation.z = -deltaRotation.z;
            // right-multiplying composes in local space, left-multiplying in world space.
            glm::quat newRotation = mCurrentGizmoMode == ImGuizmo::WORLD
                ? glm::normalize(deltaRotation * prevRotation)
                : glm::normalize(prevRotation * deltaRotation);
            transform.rotation = { newRotation.x, newRotation.y, newRotation.z, newRotation.w };
        }
        else
        {
            glm::mat3 rotationMatrix = glm::mat3(glm::vec3(matrix[0]) / scale.x,
                                                 glm::vec3(matrix[1]) / scale.y,
                                                 glm::vec3(matrix[2]) / scale.z);
            glm::quat rotation = glm::quat_cast(rotationMatrix);
            transform.rotation = { rotation.x, rotation.y, rotation.z, rotation.w };
        }
    }
}