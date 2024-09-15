#include "AdApplication.h"

#include <ECS/Component/AdLookAtCameraComponent.h>
#include <ECS/Component/AdTransformComponent.h>

#include "AdLog.h"
#include "Render/AdRenderContext.h"
#include "ECS/AdEntity.h"

namespace ade {
    AdAppContext AdApplication::sAppContext{};

    void AdApplication::Start(int argc, char **argv) {
        AdLog::Init();

        ParseArgs(argc, argv);
        OnConfiguration(&mAppSettings);

        mWindow = AdWindow::Create(mAppSettings.width, mAppSettings.height, mAppSettings.title);
        mRenderContext = std::make_shared<AdRenderContext>(mWindow.get());

        sAppContext.app = this;
        sAppContext.renderCxt = mRenderContext.get();

        OnInit();
        LoadScene();
        RegisterCameraMove();
        mStartTimePoint = std::chrono::steady_clock::now();
    }

    void AdApplication::Stop() {
        UnLoadScene();
        OnDestroy();
    }

    void AdApplication::MainLoop() {
        mLastTimePoint = std::chrono::steady_clock::now();
        while (!mWindow->ShouldClose()) {
            mWindow->PollEvents();

            float deltaTime = std::chrono::duration<float>(std::chrono::steady_clock::now() - mLastTimePoint).count();
            mLastTimePoint = std::chrono::steady_clock::now();
            mFrameIndex++;
            if (mWindow->IsMinWindow()) {
                continue;
            }
            if (!bPause) {
                OnUpdate(deltaTime);
            }
            OnRender();

            mWindow->SwapBuffer();
        }
    }

    void AdApplication::RegisterCameraMove() {
        mObserver = std::make_shared<AdEventObserver>();

        mScene->GetEcsRegistry().view<AdLookAtCameraComponent, AdTransformComponent>().each(
            [this](const auto &e, AdLookAtCameraComponent &cameraComp, AdTransformComponent &transform) {
                mObserver->OnEvent<ade::AdMouseScrollEvent>([this,&cameraComp](const ade::AdMouseScrollEvent &event) {
                    float radius = cameraComp.GetRadius() + event.mYOffset * -0.3f;
                    if (radius < 0.1f) {
                        radius = 0.1f;
                    }
                    cameraComp.SetRadius(radius);
                });

                mObserver->OnEvent<ade::AdKeyPressEvent>([this,&cameraComp](const ade::AdKeyPressEvent &event) {
                    if (event.mKeyCode == KEY_W) {
                        cameraComp.mForwardFlag = true;
                    }
                    if (event.mKeyCode == KEY_S) {
                        cameraComp.mBackwardFlag = true;
                    }
                    if (event.mKeyCode == KEY_A) {
                        cameraComp.mLeftFlag = true;
                    }
                    if (event.mKeyCode == KEY_D) {
                        cameraComp.mRightFlag = true;
                    }
                    if (event.mKeyCode == KEY_Q) {
                        cameraComp.mUpFlag = true;
                    }
                    if (event.mKeyCode == KEY_E) {
                        cameraComp.mDownFlag = true;
                    }
                });
                mObserver->OnEvent<ade::AdKeyReleaseEvent>([this,&cameraComp](const ade::AdKeyReleaseEvent &event) {
                    if (event.mKeyCode == KEY_W) {
                        cameraComp.mForwardFlag = false;
                    }
                    if (event.mKeyCode == KEY_S) {
                        cameraComp.mBackwardFlag = false;
                    }

                    if (event.mKeyCode == KEY_A) {
                        cameraComp.mLeftFlag = false;
                    }
                    if (event.mKeyCode == KEY_D) {
                        cameraComp.mRightFlag = false;
                    }
                    if (event.mKeyCode == KEY_Q) {
                        cameraComp.mUpFlag = false;
                    }
                    if (event.mKeyCode == KEY_E) {
                        cameraComp.mDownFlag = false;
                    }
                });

                mObserver->OnEvent<ade::AdMouseMovedEvent>(
                    [this,&cameraComp,&transform](const ade::AdMouseMovedEvent &event) {
                        if (!mWindow->IsMouseDown()) {
                            bFirstMouseDrag = true;
                            return;
                        }

                        glm::vec2 mousePos(event.mXPos,event.mYPos);
                        glm::vec2 mousePosDelta = {mLastMousePos.x - mousePos.x, mousePos.y - mLastMousePos.y};
                        mLastMousePos = mousePos;

                        if (abs(mousePosDelta.x) > 0.1f || abs(mousePosDelta.y) > 0.1f) {
                            if (bFirstMouseDrag) {
                                bFirstMouseDrag = false;
                            } else {
                                glm::vec3 rotation = transform.GetRotation();
                                rotation.x += mousePosDelta.x * mMouseSensitivity;
                                rotation.y += mousePosDelta.y * mMouseSensitivity;

                                if (rotation.x > 89.f) {
                                    rotation.x = 89.f;
                                }
                                if (rotation.y < -89.f) {
                                    rotation.y = -89.f;
                                }
                                transform.SetRotation(rotation);
                            }
                        }
                    });
            });
    }

    void AdApplication::ParseArgs(int argc, char **argv) {
        // TODO
    }

    bool AdApplication::LoadScene(const std::string &filePath) {
        if (mScene) {
            UnLoadScene();
        }
        mScene = std::make_unique<AdScene>();
        OnSceneInit(mScene.get());
        sAppContext.scene = mScene.get();
        return true;
    }

    void AdApplication::UnLoadScene() {
        if (mScene) {
            OnSceneDestroy(mScene.get());
            mScene.reset();
            sAppContext.scene = nullptr;
        }
    }
}
