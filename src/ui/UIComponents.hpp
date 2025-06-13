#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace VRMusicStudio {
namespace UI {

class WindowsUIComponent {
public:
    WindowsUIComponent();
    ~WindowsUIComponent();

    bool initialize(HWND hwnd);
    void render();
    void resize(uint32_t width, uint32_t height);

private:
    // DirectX Komponenten
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_deviceContext;
    IDXGISwapChain* m_swapChain;
    ID3D11RenderTargetView* m_renderTargetView;
    
    // Shader Ressourcen
    ID3D11VertexShader* m_vertexShader;
    ID3D11PixelShader* m_pixelShader;
    ID3D11InputLayout* m_inputLayout;
    
    // Buffer
    ID3D11Buffer* m_vertexBuffer;
    ID3D11Buffer* m_constantBuffer;

    // UI Elemente
    struct UIElement {
        glm::vec2 position;
        glm::vec2 size;
        std::string text;
        bool isVisible;
    };
    std::vector<UIElement> m_elements;

    bool createDeviceAndSwapChain(HWND hwnd);
    bool createRenderTarget();
    bool createShaders();
    bool createBuffers();
    void cleanup();
};

} // namespace UI
} // namespace VRMusicStudio 