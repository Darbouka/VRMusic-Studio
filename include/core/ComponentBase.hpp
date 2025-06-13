#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>

namespace VRMusicStudio {
namespace Core {

class ComponentBase {
public:
    virtual ~ComponentBase() = default;

    // Component identification
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getCategory() const = 0;

    // Lifecycle management
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual void update() = 0;

    // State management
    virtual bool isEnabled() const = 0;
    virtual void setEnabled(bool enabled) = 0;
    virtual bool isInitialized() const = 0;

    // Error handling
    virtual std::string getLastError() const = 0;
    virtual void clearError() = 0;

protected:
    bool enabled = false;
    bool initialized = false;
    std::string lastError;
};

} // namespace Core
} // namespace VRMusicStudio 