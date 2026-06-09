#include "DefaultCameraControllerSystem.h"

void DefaultCameraControllerSystem::Startup(const EngineStartupParams& params, World& world)
{

}
void DefaultCameraControllerSystem::PostStartup(const EngineStartupParams& params, World& world)
{

}
void DefaultCameraControllerSystem::Shutdown(const EngineShutdownParams& params, World& world)
{

}

void DefaultCameraControllerSystem::Update(LocalSystemContext& ctx, float deltaTime)
{
    const InputState& input = ctx.GetResource<const InputState>();

    ctx.ForEachComponentOfType<const DefaultCameraControllerComponent, TransformComponent>([&](const Entity& e, const DefaultCameraControllerComponent& cameraControllerComponent, TransformComponent& transform)
        {
            float movementSpeed = 5.0f;
            if(input.IsKeyDown(RFKeyCode::D))
                ctx.GetComponent<TransformComponent>(e).location += ctx.GetComponent<TransformComponent>(e).GetRight() * movementSpeed * ctx.GetResource<const Time>().deltaTime;
            if(input.IsKeyDown(RFKeyCode::A))
                ctx.GetComponent<TransformComponent>(e).location -= ctx.GetComponent<TransformComponent>(e).GetRight() * movementSpeed * ctx.GetResource<const Time>().deltaTime;
            if(input.IsKeyDown(RFKeyCode::SPACE))
                ctx.GetComponent<TransformComponent>(e).location += glm::vec3(0, 1, 0) * movementSpeed * ctx.GetResource<const Time>().deltaTime;
            if(input.IsKeyDown(RFKeyCode::LSHIFT))
                ctx.GetComponent<TransformComponent>(e).location -= glm::vec3(0, 1, 0) * movementSpeed * ctx.GetResource<const Time>().deltaTime;
            if(input.IsKeyDown(RFKeyCode::W))
                ctx.GetComponent<TransformComponent>(e).location += ctx.GetComponent<TransformComponent>(e).GetForward() * movementSpeed * ctx.GetResource<const Time>().deltaTime;
            if(input.IsKeyDown(RFKeyCode::S))
                ctx.GetComponent<TransformComponent>(e).location -= ctx.GetComponent<TransformComponent>(e).GetForward() * movementSpeed * ctx.GetResource<const Time>().deltaTime;

            float lookSpeed = 10.0f;
            if(input.IsMouseButtonDown(MouseButtonCode::Left))
            {
                static const float MAX_PITCH_ANGLE = 85.0f;

		        glm::dvec2 mouseDelta = input.mouseDelta;
                if(mouseDelta == glm::zero<glm::dvec2>())
                    return;

                float yawAmount = mouseDelta.x * lookSpeed * ctx.GetResource<const Time>().deltaTime;
                float pitchAmount = mouseDelta.y * lookSpeed * ctx.GetResource<const Time>().deltaTime;

                // Rotation around world Y-axis
                glm::quat yawRotation = glm::angleAxis(-yawAmount, glm::vec3(0.0f, 1.0f, 0.0f));
                transform.rotation = yawRotation * transform.rotation;

                // Rotation around camera's right axis
                glm::quat pitchRotation = glm::angleAxis(-pitchAmount, transform.GetRight());

                // Forward after applying pitch
                glm::vec3 newForward = (pitchRotation * transform.rotation) * glm::vec3(0.0f, 0.0f, -1.0f);
                float newPitchAngle = glm::degrees(glm::asin(newForward.y));

                // Apply pitch only if within bounds
                if(newPitchAngle < MAX_PITCH_ANGLE && newPitchAngle > -MAX_PITCH_ANGLE)
                    transform.rotation = pitchRotation * transform.rotation;

                // Enforce roll lock by using world up as the up vector for lookAt
                transform.rotation = glm::quatLookAt(transform.GetForward(), glm::vec3(0.0f, 1.0f, 0.0f));
                transform.rotation = glm::normalize(transform.rotation);
            }

        });
}