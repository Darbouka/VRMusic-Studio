#pragma once

#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <torch/torch.h>

class VirtualRooms {
public:
    // Konzertsäle
    struct ConcertHall {
        float roomSize;
        float reverbTime;
        float earlyReflections;
        float diffusion;
        float damping;
        float stereoWidth;
        bool enabled;
    };

    struct OperaHouse {
        float roomSize;
        float reverbTime;
        float earlyReflections;
        float diffusion;
        float damping;
        float stereoWidth;
        bool enabled;
    };

    // Studios
    struct RecordingStudio {
        float roomSize;
        float reverbTime;
        float earlyReflections;
        float diffusion;
        float damping;
        float stereoWidth;
        bool enabled;
    };

    struct MixingStudio {
        float roomSize;
        float reverbTime;
        float earlyReflections;
        float diffusion;
        float damping;
        float stereoWidth;
        bool enabled;
    };

    // Kirchen
    struct Cathedral {
        float roomSize;
        float reverbTime;
        float earlyReflections;
        float diffusion;
        float damping;
        float stereoWidth;
        bool enabled;
    };

    struct Church {
        float roomSize;
        float reverbTime;
        float earlyReflections;
        float diffusion;
        float damping;
        float stereoWidth;
        bool enabled;
    };

    // Spezielle Räume
    struct ChamberRoom {
        float roomSize;
        float reverbTime;
        float earlyReflections;
        float diffusion;
        float damping;
        float stereoWidth;
        bool enabled;
    };

    struct JazzClub {
        float roomSize;
        float reverbTime;
        float earlyReflections;
        float diffusion;
        float damping;
        float stereoWidth;
        bool enabled;
    };

    // Virtuelle Räume
    struct VirtualSpace {
        float roomSize;
        float reverbTime;
        float earlyReflections;
        float diffusion;
        float damping;
        float stereoWidth;
        bool enabled;
    };

    struct QuantumSpace {
        float roomSize;
        float reverbTime;
        float earlyReflections;
        float diffusion;
        float damping;
        float stereoWidth;
        bool enabled;
    };

    VirtualRooms();
    ~VirtualRooms();

    // Parameter-Setter für alle Räume
    void setConcertHallParams(const ConcertHall& params);
    void setOperaHouseParams(const OperaHouse& params);
    void setRecordingStudioParams(const RecordingStudio& params);
    void setMixingStudioParams(const MixingStudio& params);
    void setCathedralParams(const Cathedral& params);
    void setChurchParams(const Church& params);
    void setChamberRoomParams(const ChamberRoom& params);
    void setJazzClubParams(const JazzClub& params);
    void setVirtualSpaceParams(const VirtualSpace& params);
    void setQuantumSpaceParams(const QuantumSpace& params);

    // Audio-Verarbeitung für alle Räume
    void processConcertHall(std::vector<float>& buffer);
    void processOperaHouse(std::vector<float>& buffer);
    void processRecordingStudio(std::vector<float>& buffer);
    void processMixingStudio(std::vector<float>& buffer);
    void processCathedral(std::vector<float>& buffer);
    void processChurch(std::vector<float>& buffer);
    void processChamberRoom(std::vector<float>& buffer);
    void processJazzClub(std::vector<float>& buffer);
    void processVirtualSpace(std::vector<float>& buffer);
    void processQuantumSpace(std::vector<float>& buffer);

    // Callback-Setter für alle Räume
    void setConcertHallCallback(std::function<void(const std::vector<float>&)> callback);
    void setOperaHouseCallback(std::function<void(const std::vector<float>&)> callback);
    void setRecordingStudioCallback(std::function<void(const std::vector<float>&)> callback);
    void setMixingStudioCallback(std::function<void(const std::vector<float>&)> callback);
    void setCathedralCallback(std::function<void(const std::vector<float>&)> callback);
    void setChurchCallback(std::function<void(const std::vector<float>&)> callback);
    void setChamberRoomCallback(std::function<void(const std::vector<float>&)> callback);
    void setJazzClubCallback(std::function<void(const std::vector<float>&)> callback);
    void setVirtualSpaceCallback(std::function<void(const std::vector<float>&)> callback);
    void setQuantumSpaceCallback(std::function<void(const std::vector<float>&)> callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
}; 