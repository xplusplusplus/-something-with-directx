#pragma once
#include "Window.h"
#include "SwapChain.h"
#include "GraphicsEngine.h"
#include "DeviceContext.h"
#include "VertexBuffer.h"
#include "VertexShader.h"

class AppWindow : public Window
{
public:
    ~AppWindow();

    // Inherited via Window
    void onCreate() override;
    void onUpdate() override;
    void onDestroy() override;

private:
    SwapChain* m_swap_chain = nullptr;
    VertexBuffer* m_vb = nullptr;
    VertexShader* m_vs = nullptr;
};

