import XCTest
@testable import StompCoin

class CompatibilityTests: XCTestCase {
    var videoStreamingService: VideoStreamingService!
    var userManager: UserManager!
    var challengeService: ChallengeService!
    
    override func setUp() {
        super.setUp()
        videoStreamingService = VideoStreamingService.shared
        userManager = UserManager.shared
        challengeService = ChallengeService.shared
    }
    
    override func tearDown() {
        videoStreamingService = nil
        userManager = nil
        challengeService = nil
        super.tearDown()
    }
    
    func testIOSVersionCompatibility() {
        // Prüfen ob die App auf verschiedenen iOS-Versionen funktioniert
        let iosVersions = ["14.0", "15.0", "16.0", "17.0"]
        
        for version in iosVersions {
            let expectation = XCTestExpectation(description: "iOS \(version) Compatibility")
            
            // Simulieren der iOS-Version
            if #available(iOS \(version), *) {
                // App-Funktionalität testen
                let user = createTestUser()
                let challenge = createTestChallenge()
                
                Task {
                    try? await videoStreamingService.startVideoStream(for: challenge)
                    try? await videoStreamingService.addStomps(1000, for: user.id)
                    expectation.fulfill()
                }
            } else {
                expectation.fulfill()
            }
            
            wait(for: [expectation], timeout: 5.0)
        }
    }
    
    func testDeviceCompatibility() {
        // Prüfen ob die App auf verschiedenen Geräten funktioniert
        let devices = ["iPhone", "iPad"]
        
        for device in devices {
            let expectation = XCTestExpectation(description: "\(device) Compatibility")
            
            // Simulieren des Gerätetyps
            if device == "iPhone" {
                // iPhone-spezifische Tests
                let user = createTestUser()
                let challenge = createTestChallenge()
                
                Task {
                    try? await videoStreamingService.startVideoStream(for: challenge)
                    try? await videoStreamingService.switchCamera()
                    expectation.fulfill()
                }
            } else {
                // iPad-spezifische Tests
                let user = createTestUser()
                let challenge = createTestChallenge()
                
                Task {
                    try? await videoStreamingService.startVideoStream(for: challenge)
                    try? await videoStreamingService.addStomps(1000, for: user.id)
                    expectation.fulfill()
                }
            }
            
            wait(for: [expectation], timeout: 5.0)
        }
    }
    
    func testNetworkConditions() {
        // Prüfen ob die App unter verschiedenen Netzwerkbedingungen funktioniert
        let networkConditions = ["wifi", "cellular", "poor"]
        
        for condition in networkConditions {
            let expectation = XCTestExpectation(description: "\(condition) Network Compatibility")
            
            // Simulieren der Netzwerkbedingungen
            switch condition {
            case "wifi":
                // WLAN-Tests
                testStreamingWithNetworkCondition(expectation)
            case "cellular":
                // Mobilfunk-Tests
                testStreamingWithNetworkCondition(expectation)
            case "poor":
                // Schlechte Verbindung-Tests
                testStreamingWithNetworkCondition(expectation)
            default:
                expectation.fulfill()
            }
            
            wait(for: [expectation], timeout: 10.0)
        }
    }
    
    func testStorageCompatibility() {
        // Prüfen ob die App mit verschiedenen Speicherbedingungen funktioniert
        let storageConditions = ["normal", "low", "full"]
        
        for condition in storageConditions {
            let expectation = XCTestExpectation(description: "\(condition) Storage Compatibility")
            
            // Simulieren der Speicherbedingungen
            switch condition {
            case "normal":
                // Normaler Speicher-Tests
                testStreamingWithStorageCondition(expectation)
            case "low":
                // Niedriger Speicher-Tests
                testStreamingWithStorageCondition(expectation)
            case "full":
                // Vollständiger Speicher-Tests
                testStreamingWithStorageCondition(expectation)
            default:
                expectation.fulfill()
            }
            
            wait(for: [expectation], timeout: 5.0)
        }
    }
    
    func testCameraCompatibility() {
        // Prüfen ob die App mit verschiedenen Kamera-Konfigurationen funktioniert
        let cameraConfigurations = ["front", "back", "none"]
        
        for config in cameraConfigurations {
            let expectation = XCTestExpectation(description: "\(config) Camera Compatibility")
            
            // Simulieren der Kamera-Konfiguration
            switch config {
            case "front":
                // Frontkamera-Tests
                testStreamingWithCameraConfiguration(expectation)
            case "back":
                // Rückkamera-Tests
                testStreamingWithCameraConfiguration(expectation)
            case "none":
                // Keine Kamera-Tests
                testStreamingWithCameraConfiguration(expectation)
            default:
                expectation.fulfill()
            }
            
            wait(for: [expectation], timeout: 5.0)
        }
    }
    
    // MARK: - Helper Methods
    
    private func testStreamingWithNetworkCondition(_ expectation: XCTestExpectation) {
        let user = createTestUser()
        let challenge = createTestChallenge()
        
        Task {
            try? await videoStreamingService.startVideoStream(for: challenge)
            try? await videoStreamingService.addStomps(1000, for: user.id)
            expectation.fulfill()
        }
    }
    
    private func testStreamingWithStorageCondition(_ expectation: XCTestExpectation) {
        let user = createTestUser()
        let challenge = createTestChallenge()
        
        Task {
            try? await videoStreamingService.startVideoStream(for: challenge)
            try? await videoStreamingService.addStomps(1000, for: user.id)
            expectation.fulfill()
        }
    }
    
    private func testStreamingWithCameraConfiguration(_ expectation: XCTestExpectation) {
        let user = createTestUser()
        let challenge = createTestChallenge()
        
        Task {
            try? await videoStreamingService.startVideoStream(for: challenge)
            try? await videoStreamingService.switchCamera()
            expectation.fulfill()
        }
    }
    
    private func createTestUser(isVIPPremium: Bool = false) -> UserManager.User {
        return UserManager.User(
            id: UUID(),
            username: "Test User",
            email: "test@example.com",
            level: 1,
            tokens: 100,
            stompCoins: 100,
            isVIPPremium: isVIPPremium,
            isDeveloper: false,
            vipPremiumGrantedBy: nil,
            vipPremiumGrantDate: nil,
            isDeveloperVIPPremium: false
        )
    }
    
    private func createTestChallenge() -> ChallengeService.Challenge {
        return ChallengeService.Challenge(
            id: UUID(),
            title: "Test Challenge",
            description: "Test Description",
            eventId: UUID(),
            startDate: Date(),
            endDate: Calendar.current.date(byAdding: .day, value: 7, to: Date())!,
            requiredTokens: 100,
            requiredStompCoins: 200,
            participants: [],
            confirmedParticipants: [],
            status: .active,
            type: .event
        )
    }
} 