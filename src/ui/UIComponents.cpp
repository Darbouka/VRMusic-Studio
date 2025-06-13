#include "UIComponents.hpp"
#include <spdlog/spdlog.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <d3dcompiler.h>

namespace VRMusicStudio {
namespace UI {

// Slider-Implementierung
void UISlider::render() {
    if (!visible) return;
    
    ImGui::SetCursorPos(ImVec2(position.x, position.y));
    if (ImGui::SliderFloat(label.c_str(), &value, min, max)) {
        if (onValueChanged) {
            onValueChanged(value);
        }
    }
}

// Toggle-Implementierung
void UIToggle::render() {
    if (!visible) return;
    
    ImGui::SetCursorPos(ImVec2(position.x, position.y));
    if (ImGui::Checkbox(label.c_str(), &value)) {
        if (onValueChanged) {
            onValueChanged(value);
        }
    }
}

// Input-Implementierung
void UIInput::render() {
    if (!visible) return;
    
    ImGui::SetCursorPos(ImVec2(position.x, position.y));
    if (ImGui::InputText(label.c_str(), buffer, bufferSize)) {
        if (onValueChanged) {
            onValueChanged(buffer);
        }
    }
}

// Button-Implementierung
void UIButton::render() {
    if (!visible) return;
    
    ImGui::SetCursorPos(ImVec2(position.x, position.y));
    if (ImGui::Button(label.c_str(), ImVec2(size.x, size.y))) {
        if (onClick) {
            onClick();
        }
    }
}

// Panel-Implementierung
void UIPanel::render() {
    if (!visible) return;
    
    ImGui::SetNextWindowPos(ImVec2(position.x, position.y));
    ImGui::SetNextWindowSize(ImVec2(size.x, size.y));
    
    ImGuiWindowFlags flags = 0;
    if (!draggable) flags |= ImGuiWindowFlags_NoMove;
    if (!resizable) flags |= ImGuiWindowFlags_NoResize;
    
    if (ImGui::Begin(title.c_str(), nullptr, flags)) {
        for (auto& element : elements) {
            element->render();
        }
    }
    ImGui::End();
}

// Menu-Implementierung
void UIMenu::render() {
    if (!visible) return;
    
    ImGui::SetCursorPos(ImVec2(position.x, position.y));
    if (ImGui::BeginCombo(label.c_str(), items[0].c_str())) {
        for (const auto& item : items) {
            if (ImGui::Selectable(item.c_str())) {
                if (onItemSelected) {
                    onItemSelected(item);
                }
            }
        }
        ImGui::EndCombo();
    }
}

// Waveform-Implementierung
void UIWaveform::render() {
    if (!visible || data.empty()) return;
    
    ImGui::SetCursorPos(ImVec2(position.x, position.y));
    ImGui::BeginChild(label.c_str(), ImVec2(size.x, size.y), true);
    
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();
    
    float maxValue = autoScale ? 
        *std::max_element(data.begin(), data.end()) : 1.0f;
    
    for (size_t i = 0; i < data.size() - 1; ++i) {
        float x1 = p.x + (i * size.x / (data.size() - 1));
        float x2 = p.x + ((i + 1) * size.x / (data.size() - 1));
        float y1 = p.y + size.y/2 + (data[i] * size.y/2 / maxValue);
        float y2 = p.y + size.y/2 + (data[i+1] * size.y/2 / maxValue);
        
        draw_list->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), 
                          ImGui::GetColorU32(ImVec4(1,1,1,1)));
    }
    
    ImGui::EndChild();
}

// Spectrum-Implementierung
void UISpectrum::render() {
    if (!visible || data.empty()) return;
    
    ImGui::SetCursorPos(ImVec2(position.x, position.y));
    ImGui::BeginChild(label.c_str(), ImVec2(size.x, size.y), true);
    
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();
    
    float maxValue = autoScale ? 
        *std::max_element(data.begin(), data.end()) : 1.0f;
    
    float barWidth = size.x / data.size();
    for (size_t i = 0; i < data.size(); ++i) {
        float height = (data[i] * size.y / maxValue);
        float x = p.x + (i * barWidth);
        float y = p.y + size.y - height;
        
        draw_list->AddRectFilled(
            ImVec2(x, y),
            ImVec2(x + barWidth - 1, p.y + size.y),
            ImGui::GetColorU32(ImVec4(1,1,1,1))
        );
    }
    
    ImGui::EndChild();
}

// Knob-Implementierung
void UIKnob::render() {
    if (!visible) return;
    
    ImGui::SetCursorPos(ImVec2(position.x, position.y));
    
    float knobSize = size.x;
    ImVec2 center = ImVec2(position.x + knobSize/2, position.y + knobSize/2);
    
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    // Knob-Kreis
    draw_list->AddCircle(center, knobSize/2, 
                        ImGui::GetColorU32(ImVec4(0.3f,0.3f,0.3f,1)));
    
    // Wert-Linie
    float angle = (value - min) / (max - min) * 2.0f * M_PI - M_PI/2;
    float x = center.x + cos(angle) * (knobSize/2 - 5);
    float y = center.y + sin(angle) * (knobSize/2 - 5);
    draw_list->AddLine(center, ImVec2(x,y), 
                      ImGui::GetColorU32(ImVec4(1,1,1,1)), 2);
    
    // Label
    ImGui::SetCursorPos(ImVec2(position.x, position.y + knobSize + 5));
    ImGui::Text("%s: %.2f", label.c_str(), value);
}

// Theme-Manager-Implementierung
void ThemeManager::setTheme(const std::string& theme) {
    if (theme == "dark") {
        backgroundColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
        textColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        accentColor = glm::vec4(0.2f, 0.6f, 1.0f, 1.0f);
    } else {
        backgroundColor = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
        textColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
        accentColor = glm::vec4(0.0f, 0.4f, 0.8f, 1.0f);
    }
}

void ThemeManager::applyTheme() {
    ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = 
        ImVec4(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    ImGui::GetStyle().Colors[ImGuiCol_Text] = 
        ImVec4(textColor.r, textColor.g, textColor.b, textColor.a);
    ImGui::GetStyle().Colors[ImGuiCol_Button] = 
        ImVec4(accentColor.r, accentColor.g, accentColor.b, accentColor.a);
}

// Layout-Manager-Implementierung
void LayoutManager::setLayout(const std::string& layout) {
    if (layout == "vertical") {
        arrangeVertical();
    } else if (layout == "horizontal") {
        arrangeHorizontal();
    } else if (layout == "grid") {
        arrangeGrid();
    }
}

void LayoutManager::arrangeVertical() {
    float currentY = position.y + padding;
    for (auto& element : elements) {
        element->setPosition(glm::vec2(position.x + padding, currentY));
        currentY += element->getSize().y + spacing;
    }
}

void LayoutManager::arrangeHorizontal() {
    float currentX = position.x + padding;
    for (auto& element : elements) {
        element->setPosition(glm::vec2(currentX, position.y + padding));
        currentX += element->getSize().x + spacing;
    }
}

void LayoutManager::arrangeGrid() {
    int columns = static_cast<int>(std::sqrt(elements.size()));
    float cellWidth = (size.x - (columns + 1) * padding) / columns;
    float cellHeight = (size.y - (elements.size() / columns + 1) * padding) / (elements.size() / columns);
    
    for (size_t i = 0; i < elements.size(); ++i) {
        int row = i / columns;
        int col = i % columns;
        float x = position.x + padding + col * (cellWidth + padding);
        float y = position.y + padding + row * (cellHeight + padding);
        elements[i]->setPosition(glm::vec2(x, y));
        elements[i]->setSize(glm::vec2(cellWidth, cellHeight));
    }
}

WindowsUIComponent::WindowsUIComponent()
    : m_device(nullptr)
    , m_deviceContext(nullptr)
    , m_swapChain(nullptr)
    , m_renderTargetView(nullptr)
    , m_vertexShader(nullptr)
    , m_pixelShader(nullptr)
    , m_inputLayout(nullptr)
    , m_vertexBuffer(nullptr)
    , m_constantBuffer(nullptr)
{
}

WindowsUIComponent::~WindowsUIComponent() {
    cleanup();
}

bool WindowsUIComponent::initialize(HWND hwnd) {
    if (!createDeviceAndSwapChain(hwnd)) {
        spdlog::error("Failed to create device and swap chain");
        return false;
    }

    if (!createRenderTarget()) {
        spdlog::error("Failed to create render target");
        return false;
    }

    if (!createShaders()) {
        spdlog::error("Failed to create shaders");
        return false;
    }

    if (!createBuffers()) {
        spdlog::error("Failed to create buffers");
        return false;
    }

    return true;
}

bool WindowsUIComponent::createDeviceAndSwapChain(HWND hwnd) {
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = 800;
    swapChainDesc.BufferDesc.Height = 600;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hwnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.Windowed = TRUE;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        createDeviceFlags,
        featureLevels,
        ARRAYSIZE(featureLevels),
        D3D11_SDK_VERSION,
        &swapChainDesc,
        &m_swapChain,
        &m_device,
        nullptr,
        &m_deviceContext
    );

    return SUCCEEDED(hr);
}

bool WindowsUIComponent::createRenderTarget() {
    ID3D11Texture2D* backBuffer;
    HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
    if (FAILED(hr)) return false;

    hr = m_device->CreateRenderTargetView(backBuffer, nullptr, &m_renderTargetView);
    backBuffer->Release();
    
    if (FAILED(hr)) return false;

    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, nullptr);
    return true;
}

bool WindowsUIComponent::createShaders() {
    // Einfacher Vertex Shader
    const char* vertexShaderSource = R"(
        struct VS_INPUT {
            float3 Pos : POSITION;
            float2 Tex : TEXCOORD0;
        };
        
        struct VS_OUTPUT {
            float4 Pos : SV_POSITION;
            float2 Tex : TEXCOORD0;
        };
        
        VS_OUTPUT main(VS_INPUT input) {
            VS_OUTPUT output;
            output.Pos = float4(input.Pos, 1.0f);
            output.Tex = input.Tex;
            return output;
        }
    )";

    // Einfacher Pixel Shader
    const char* pixelShaderSource = R"(
        struct PS_INPUT {
            float4 Pos : SV_POSITION;
            float2 Tex : TEXCOORD0;
        };
        
        float4 main(PS_INPUT input) : SV_TARGET {
            return float4(1.0f, 1.0f, 1.0f, 1.0f);
        }
    )";

    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;

    HRESULT hr = D3DCompile(
        vertexShaderSource,
        strlen(vertexShaderSource),
        nullptr,
        nullptr,
        nullptr,
        "main",
        "vs_5_0",
        0,
        0,
        &vsBlob,
        &errorBlob
    );

    if (FAILED(hr)) {
        if (errorBlob) {
            spdlog::error("Vertex shader compilation failed: {}", (char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }
        return false;
    }

    hr = D3DCompile(
        pixelShaderSource,
        strlen(pixelShaderSource),
        nullptr,
        nullptr,
        nullptr,
        "main",
        "ps_5_0",
        0,
        0,
        &psBlob,
        &errorBlob
    );

    if (FAILED(hr)) {
        if (errorBlob) {
            spdlog::error("Pixel shader compilation failed: {}", (char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }
        return false;
    }

    hr = m_device->CreateVertexShader(
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        nullptr,
        &m_vertexShader
    );

    if (FAILED(hr)) {
        vsBlob->Release();
        psBlob->Release();
        return false;
    }

    hr = m_device->CreatePixelShader(
        psBlob->GetBufferPointer(),
        psBlob->GetBufferSize(),
        nullptr,
        &m_pixelShader
    );

    if (FAILED(hr)) {
        vsBlob->Release();
        psBlob->Release();
        return false;
    }

    // Input Layout erstellen
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    hr = m_device->CreateInputLayout(
        layout,
        2,
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        &m_inputLayout
    );

    vsBlob->Release();
    psBlob->Release();

    return SUCCEEDED(hr);
}

bool WindowsUIComponent::createBuffers() {
    // Einfache Vertex-Daten für ein Rechteck
    struct Vertex {
        float x, y, z;
        float u, v;
    };

    Vertex vertices[] = {
        { -1.0f, -1.0f, 0.0f, 0.0f, 1.0f },
        { -1.0f,  1.0f, 0.0f, 0.0f, 0.0f },
        {  1.0f, -1.0f, 0.0f, 1.0f, 1.0f },
        {  1.0f,  1.0f, 0.0f, 1.0f, 0.0f }
    };

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = sizeof(vertices);
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices;

    HRESULT hr = m_device->CreateBuffer(
        &bufferDesc,
        &initData,
        &m_vertexBuffer
    );

    return SUCCEEDED(hr);
}

void WindowsUIComponent::render() {
    if (!m_deviceContext) return;

    // Viewport setzen
    D3D11_VIEWPORT viewport = {};
    viewport.Width = 800;
    viewport.Height = 600;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    m_deviceContext->RSSetViewports(1, &viewport);

    // Render-Target löschen
    float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    m_deviceContext->ClearRenderTargetView(m_renderTargetView, clearColor);

    // Shader und Input Layout setzen
    m_deviceContext->IASetInputLayout(m_inputLayout);
    m_deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
    m_deviceContext->PSSetShader(m_pixelShader, nullptr, 0);

    // Vertex Buffer setzen
    UINT stride = sizeof(float) * 5;
    UINT offset = 0;
    m_deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
    m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // Zeichnen
    m_deviceContext->Draw(4, 0);

    // Swap Chain präsentieren
    m_swapChain->Present(1, 0);
}

void WindowsUIComponent::resize(uint32_t width, uint32_t height) {
    if (!m_device || !m_swapChain) return;

    // Render Target View freigeben
    if (m_renderTargetView) {
        m_renderTargetView->Release();
        m_renderTargetView = nullptr;
    }

    // Swap Chain neu erstellen
    HRESULT hr = m_swapChain->ResizeBuffers(
        1,
        width,
        height,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        0
    );

    if (SUCCEEDED(hr)) {
        createRenderTarget();
    }
}

void WindowsUIComponent::cleanup() {
    if (m_constantBuffer) m_constantBuffer->Release();
    if (m_vertexBuffer) m_vertexBuffer->Release();
    if (m_inputLayout) m_inputLayout->Release();
    if (m_pixelShader) m_pixelShader->Release();
    if (m_vertexShader) m_vertexShader->Release();
    if (m_renderTargetView) m_renderTargetView->Release();
    if (m_swapChain) m_swapChain->Release();
    if (m_deviceContext) m_deviceContext->Release();
    if (m_device) m_device->Release();
}

UIComponents::UIComponents()
    : m_window(nullptr)
    , m_isInitialized(false)
    , m_showDemoWindow(false)
    , m_showAudioWindow(true)
    , m_showMIDIWindow(true)
    , m_showVRWindow(true)
{
    initialize();
}

UIComponents::~UIComponents()
{
    if (m_isInitialized) {
        shutdown();
    }
}

void UIComponents::initialize()
{
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    m_window = glfwCreateWindow(1280, 720, "VRMusic Studio", nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 150");

    m_isInitialized = true;
    spdlog::info("UI system initialized successfully");
}

void UIComponents::shutdown()
{
    if (m_isInitialized) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(m_window);
        glfwTerminate();

        m_isInitialized = false;
        spdlog::info("UI system shut down");
    }
}

void UIComponents::update()
{
    if (!m_isInitialized) return;

    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    renderMainMenuBar();
    renderAudioWindow();
    renderMIDIWindow();
    renderVRWindow();

    if (m_showDemoWindow) {
        ImGui::ShowDemoWindow(&m_showDemoWindow);
    }

    ImGui::Render();

    int display_w, display_h;
    glfwGetFramebufferSize(m_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }

    glfwSwapBuffers(m_window);
}

void UIComponents::renderMainMenuBar()
{
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Project")) {
                // Implement new project
            }
            if (ImGui::MenuItem("Open Project")) {
                // Implement open project
            }
            if (ImGui::MenuItem("Save Project")) {
                // Implement save project
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {
                glfwSetWindowShouldClose(m_window, true);
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Audio Window", nullptr, &m_showAudioWindow);
            ImGui::MenuItem("MIDI Window", nullptr, &m_showMIDIWindow);
            ImGui::MenuItem("VR Window", nullptr, &m_showVRWindow);
            ImGui::Separator();
            ImGui::MenuItem("Demo Window", nullptr, &m_showDemoWindow);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About")) {
                // Implement about dialog
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void UIComponents::renderAudioWindow()
{
    if (!m_showAudioWindow) return;

    ImGui::Begin("Audio", &m_showAudioWindow);
    
    if (ImGui::CollapsingHeader("Input Devices")) {
        // Implement input device selection
    }

    if (ImGui::CollapsingHeader("Output Devices")) {
        // Implement output device selection
    }

    if (ImGui::CollapsingHeader("Effects")) {
        // Implement effects chain
    }

    ImGui::End();
}

void UIComponents::renderMIDIWindow()
{
    if (!m_showMIDIWindow) return;

    ImGui::Begin("MIDI", &m_showMIDIWindow);
    
    if (ImGui::CollapsingHeader("Input Devices")) {
        // Implement MIDI input device selection
    }

    if (ImGui::CollapsingHeader("Output Devices")) {
        // Implement MIDI output device selection
    }

    if (ImGui::CollapsingHeader("Mapping")) {
        // Implement MIDI mapping
    }

    ImGui::End();
}

void UIComponents::renderVRWindow()
{
    if (!m_showVRWindow) return;

    ImGui::Begin("VR", &m_showVRWindow);
    
    if (ImGui::CollapsingHeader("Devices")) {
        // Implement VR device status
    }

    if (ImGui::CollapsingHeader("Settings")) {
        // Implement VR settings
    }

    if (ImGui::CollapsingHeader("Calibration")) {
        // Implement VR calibration
    }

    ImGui::End();
}

bool UIComponents::shouldClose() const
{
    return glfwWindowShouldClose(m_window);
}

} // namespace UI
} // namespace VRMusicStudio 