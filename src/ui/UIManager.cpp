#include "UIManager.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace VRMusicStudio {

struct UIManager::Impl {
    // ImGui
    ImGuiContext* imguiContext;
    
    // Fenster und Panels
    std::map<std::string, UIWindow> windows;
    std::map<std::string, UIPanel> panels;
    std::map<std::string, UIMenu> menus;
    
    // Callbacks
    std::map<std::string, std::function<void()>> windowCallbacks;
    std::map<std::string, std::function<void()>> panelCallbacks;
    std::map<std::string, std::map<std::string, std::function<void()>>> elementCallbacks;
    
    // Threading
    std::mutex mutex;
    std::condition_variable cv;
    std::thread processingThread;
    bool shouldStop;
    
    Impl() : imguiContext(nullptr), shouldStop(false) {}
};

UIManager::UIManager() : pImpl(std::make_unique<Impl>()) {}

UIManager::~UIManager() {
    shutdown();
}

bool UIManager::initialize() {
    IMGUI_CHECKVERSION();
    pImpl->imguiContext = ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    ImGui::StyleColorsDark();
    
    // Starte Processing-Thread
    pImpl->processingThread = std::thread([this]() {
        while (!pImpl->shouldStop) {
            std::unique_lock<std::mutex> lock(pImpl->mutex);
            pImpl->cv.wait(lock, [this]() { return pImpl->shouldStop; });
            
            // Verarbeite UI-Updates
            for (auto& [id, window] : pImpl->windows) {
                if (window.visible) {
                    renderWindow(id);
                }
            }
            
            for (auto& [id, panel] : pImpl->panels) {
                if (panel.visible) {
                    renderPanel(id);
                }
            }
            
            for (auto& [id, menu] : pImpl->menus) {
                renderMenu(id);
            }
        }
    });
    
    return true;
}

void UIManager::shutdown() {
    if (pImpl->processingThread.joinable()) {
        {
            std::lock_guard<std::mutex> lock(pImpl->mutex);
            pImpl->shouldStop = true;
        }
        pImpl->cv.notify_one();
        pImpl->processingThread.join();
    }
    
    if (pImpl->imguiContext) {
        ImGui::DestroyContext(pImpl->imguiContext);
        pImpl->imguiContext = nullptr;
    }
}

void UIManager::createWindow(const std::string& id, const std::string& title, const glm::vec2& position, const glm::vec2& size) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    UIWindow window;
    window.id = id;
    window.title = title;
    window.position = position;
    window.size = size;
    window.visible = true;
    window.resizable = true;
    window.movable = true;
    
    pImpl->windows[id] = window;
}

void UIManager::destroyWindow(const std::string& id) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->windows.erase(id);
}

void UIManager::showWindow(const std::string& id) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->windows.find(id);
    if (it != pImpl->windows.end()) {
        it->second.visible = true;
    }
}

void UIManager::hideWindow(const std::string& id) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->windows.find(id);
    if (it != pImpl->windows.end()) {
        it->second.visible = false;
    }
}

void UIManager::setWindowPosition(const std::string& id, const glm::vec2& position) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->windows.find(id);
    if (it != pImpl->windows.end()) {
        it->second.position = position;
    }
}

void UIManager::setWindowSize(const std::string& id, const glm::vec2& size) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->windows.find(id);
    if (it != pImpl->windows.end()) {
        it->second.size = size;
    }
}

void UIManager::createPanel(const std::string& id, const std::string& name, const glm::vec2& position, const glm::vec2& size) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    UIPanel panel;
    panel.id = id;
    panel.name = name;
    panel.position = position;
    panel.size = size;
    panel.visible = true;
    
    pImpl->panels[id] = panel;
}

void UIManager::destroyPanel(const std::string& id) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->panels.erase(id);
}

void UIManager::showPanel(const std::string& id) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->panels.find(id);
    if (it != pImpl->panels.end()) {
        it->second.visible = true;
    }
}

void UIManager::hidePanel(const std::string& id) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->panels.find(id);
    if (it != pImpl->panels.end()) {
        it->second.visible = false;
    }
}

void UIManager::setPanelPosition(const std::string& id, const glm::vec2& position) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->panels.find(id);
    if (it != pImpl->panels.end()) {
        it->second.position = position;
    }
}

void UIManager::setPanelSize(const std::string& id, const glm::vec2& size) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->panels.find(id);
    if (it != pImpl->panels.end()) {
        it->second.size = size;
    }
}

void UIManager::addElement(const std::string& windowId, const UIElement& element) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->windows.find(windowId);
    if (it != pImpl->windows.end()) {
        it->second.elements.push_back(element);
    }
}

void UIManager::removeElement(const std::string& windowId, const std::string& elementId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->windows.find(windowId);
    if (it != pImpl->windows.end()) {
        auto& elements = it->second.elements;
        elements.erase(
            std::remove_if(elements.begin(), elements.end(),
                [&elementId](const UIElement& element) {
                    return element.id == elementId;
                }
            ),
            elements.end()
        );
    }
}

void UIManager::showElement(const std::string& windowId, const std::string& elementId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->windows.find(windowId);
    if (it != pImpl->windows.end()) {
        for (auto& element : it->second.elements) {
            if (element.id == elementId) {
                element.visible = true;
                break;
            }
        }
    }
}

void UIManager::hideElement(const std::string& windowId, const std::string& elementId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->windows.find(windowId);
    if (it != pImpl->windows.end()) {
        for (auto& element : it->second.elements) {
            if (element.id == elementId) {
                element.visible = false;
                break;
            }
        }
    }
}

void UIManager::setElementPosition(const std::string& windowId, const std::string& elementId, const glm::vec2& position) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->windows.find(windowId);
    if (it != pImpl->windows.end()) {
        for (auto& element : it->second.elements) {
            if (element.id == elementId) {
                element.position = position;
                break;
            }
        }
    }
}

void UIManager::setElementSize(const std::string& windowId, const std::string& elementId, const glm::vec2& size) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->windows.find(windowId);
    if (it != pImpl->windows.end()) {
        for (auto& element : it->second.elements) {
            if (element.id == elementId) {
                element.size = size;
                break;
            }
        }
    }
}

void UIManager::createMenu(const std::string& id, const std::string& name, const std::vector<std::string>& items) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    UIMenu menu;
    menu.id = id;
    menu.name = name;
    menu.items = items;
    
    pImpl->menus[id] = menu;
}

void UIManager::destroyMenu(const std::string& id) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->menus.erase(id);
}

void UIManager::setMenuCallback(const std::string& id, std::function<void(const std::string&)> callback) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->menus.find(id);
    if (it != pImpl->menus.end()) {
        it->second.onSelect = callback;
    }
}

void UIManager::beginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UIManager::endFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UIManager::render() {
    for (const auto& [id, window] : pImpl->windows) {
        if (window.visible) {
            renderWindow(id);
        }
    }
    
    for (const auto& [id, panel] : pImpl->panels) {
        if (panel.visible) {
            renderPanel(id);
        }
    }
    
    for (const auto& [id, menu] : pImpl->menus) {
        renderMenu(id);
    }
}

void UIManager::renderWindow(const std::string& id) {
    auto it = pImpl->windows.find(id);
    if (it == pImpl->windows.end()) {
        return;
    }
    
    const UIWindow& window = it->second;
    
    ImGui::SetNextWindowPos(ImVec2(window.position.x, window.position.y));
    ImGui::SetNextWindowSize(ImVec2(window.size.x, window.size.y));
    
    if (ImGui::Begin(window.title.c_str(), &window.visible, 
        (window.resizable ? ImGuiWindowFlags_None : ImGuiWindowFlags_NoResize) |
        (window.movable ? ImGuiWindowFlags_None : ImGuiWindowFlags_NoMove))) {
        
        for (const auto& element : window.elements) {
            if (element.visible) {
                ImGui::SetCursorPos(ImVec2(element.position.x, element.position.y));
                
                if (ImGui::Button(element.name.c_str(), ImVec2(element.size.x, element.size.y))) {
                    if (element.onClick) {
                        element.onClick();
                    }
                }
                
                if (ImGui::IsItemHovered() && element.onHover) {
                    element.onHover();
                }
                
                if (ImGui::IsItemActive() && element.onDrag) {
                    element.onDrag();
                }
            }
        }
        
        if (pImpl->windowCallbacks.count(id) > 0) {
            pImpl->windowCallbacks[id]();
        }
    }
    ImGui::End();
}

void UIManager::renderPanel(const std::string& id) {
    auto it = pImpl->panels.find(id);
    if (it == pImpl->panels.end()) {
        return;
    }
    
    const UIPanel& panel = it->second;
    
    ImGui::SetNextWindowPos(ImVec2(panel.position.x, panel.position.y));
    ImGui::SetNextWindowSize(ImVec2(panel.size.x, panel.size.y));
    
    if (ImGui::Begin(panel.name.c_str(), &panel.visible, 
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse)) {
        
        for (const auto& element : panel.elements) {
            if (element.visible) {
                ImGui::SetCursorPos(ImVec2(element.position.x, element.position.y));
                
                if (ImGui::Button(element.name.c_str(), ImVec2(element.size.x, element.size.y))) {
                    if (element.onClick) {
                        element.onClick();
                    }
                }
                
                if (ImGui::IsItemHovered() && element.onHover) {
                    element.onHover();
                }
                
                if (ImGui::IsItemActive() && element.onDrag) {
                    element.onDrag();
                }
            }
        }
        
        if (pImpl->panelCallbacks.count(id) > 0) {
            pImpl->panelCallbacks[id]();
        }
    }
    ImGui::End();
}

void UIManager::renderMenu(const std::string& id) {
    auto it = pImpl->menus.find(id);
    if (it == pImpl->menus.end()) {
        return;
    }
    
    const UIMenu& menu = it->second;
    
    if (ImGui::BeginMenu(menu.name.c_str())) {
        for (const auto& item : menu.items) {
            if (ImGui::MenuItem(item.c_str())) {
                if (menu.onSelect) {
                    menu.onSelect(item);
                }
            }
        }
        ImGui::EndMenu();
    }
}

void UIManager::setWindowCallback(const std::string& id, std::function<void()> callback) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->windowCallbacks[id] = callback;
}

void UIManager::setPanelCallback(const std::string& id, std::function<void()> callback) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->panelCallbacks[id] = callback;
}

void UIManager::setElementCallback(const std::string& windowId, const std::string& elementId, std::function<void()> callback) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->elementCallbacks[windowId][elementId] = callback;
}

} // namespace VRMusicStudio 