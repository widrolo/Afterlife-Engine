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
    if (Haptic::GetDebugKeyHeld(10))
        mCurrentGizmoMode = ImGuizmo::LOCAL;

    if (EditorState::SelectedEntry == nullptr)
        return;

    auto* renderHandler = WEngine::CoreSystems::GetRenderHandler();

    const glm::mat4& cameraProjection = renderHandler->GetProjectionMatrix();
    glm::mat4 gizmoProjection = cameraProjection;
    gizmoProjection[1][1] *= -1.0f;

    const glm::mat4& cameraView = renderHandler->GetViewMatrix();

    WEngine::Transform entryTransform = EditorState::SelectedEntry->transform;
    glm::mat4 matrix = WEngine::RenderHandler::CalcModelMatrixGLM(entryTransform);

    glm::mat4 deltaMatrix = glm::mat4(1.0f);
    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(viewportPos.x, viewportPos.y, viewportSize.x, viewportSize.y);
    if (ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(gizmoProjection),
        mCurrentGizmoOperation, mCurrentGizmoMode, glm::value_ptr(matrix), glm::value_ptr(deltaMatrix)))
    {
        WEngine::Transform& transform = EditorState::SelectedEntry->transform;

        transform.position = { matrix[3][0], -matrix[3][1], matrix[3][2] };

        if (mCurrentGizmoOperation == ImGuizmo::ROTATE)
        {
            // bullshit fuckface shitass code.
            glm::quat prevRotation(entryTransform.rotation.w, entryTransform.rotation.x,
                entryTransform.rotation.y, entryTransform.rotation.z);
            glm::quat deltaRotation = glm::quat_cast(glm::mat3(deltaMatrix));
            glm::quat newRotation;
            if (mCurrentGizmoMode == ImGuizmo::WORLD)
            {
                newRotation = glm::normalize(deltaRotation * prevRotation);
            }
            else
            {
                glm::quat rawDelta = prevRotation * deltaRotation * glm::conjugate(prevRotation);
                newRotation = glm::normalize(prevRotation * rawDelta);
            }
            transform.rotation = { newRotation.x, newRotation.y, newRotation.z, newRotation.w };
        }
        else if (mCurrentGizmoOperation == ImGuizmo::SCALE)
        {
            glm::vec3 scale = {
                glm::length(glm::vec3(matrix[0])),
                glm::length(glm::vec3(matrix[1])),
                glm::length(glm::vec3(matrix[2]))
            };
            transform.size = { scale.x, scale.y, scale.z };
        }
    }
}
