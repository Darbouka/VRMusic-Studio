import XCTest
@testable import StompCoin

class PerformanceTests: XCTestCase {
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
    
    func testStreamPerformance() throws {
        let user = createTestUser()
        let challenge = createTestChallenge()
        
        measure {
            let expectation = XCTestExpectation(description: "Stream Performance")
            
            Task {
                try? await videoStreamingService.startVideoStream(for: challenge)
                try? await videoStreamingService.addStomps(100_000, for: user.id)
                try? await videoStreamingService.endVideoStream()
                expectation.fulfill()
            }
            
            wait(for: [expectation], timeout: 5.0)
        }
    }
    
    func testCameraSwitchPerformance() throws {
        let user = createTestUser()
        let challenge = createTestChallenge()
        
        measure {
            let expectation = XCTestExpectation(description: "Camera Switch Performance")
            
            Task {
                try? await videoStreamingService.startVideoStream(for: challenge)
                try? await videoStreamingService.switchCamera()
                expectation.fulfill()
            }
            
            wait(for: [expectation], timeout: 2.0)
        }
    }
    
    func testRewardsCalculationPerformance() throws {
        let user = createTestUser()
        let challenge = createTestChallenge()
        
        measure {
            let expectation = XCTestExpectation(description: "Rewards Calculation Performance")
            
            Task {
                try? await videoStreamingService.startVideoStream(for: challenge)
                for _ in 0..<1000 {
                    try? await videoStreamingService.addStomps(100, for: user.id)
                }
                expectation.fulfill()
            }
            
            wait(for: [expectation], timeout: 10.0)
        }
    }
    
    func testLanguageSwitchPerformance() throws {
        let localizationService = LocalizationService.shared
        
        measure {
            for _ in 0..<100 {
                localizationService.setLanguage("en")
                localizationService.setLanguage("de")
            }
        }
    }
    
    func testMemoryUsage() throws {
        let user = createTestUser()
        let challenge = createTestChallenge()
        
        measure {
            let expectation = XCTestExpectation(description: "Memory Usage")
            
            Task {
                try? await videoStreamingService.startVideoStream(for: challenge)
                for _ in 0..<100 {
                    try? await videoStreamingService.addStomps(1000, for: user.id)
                }
                expectation.fulfill()
            }
            
            wait(for: [expectation], timeout: 5.0)
        }
    }
    
    func testConcurrentStreams() throws {
        let users = (0..<5).map { _ in createTestUser() }
        let challenge = createTestChallenge()
        
        measure {
            let expectation = XCTestExpectation(description: "Concurrent Streams")
            expectation.expectedFulfillmentCount = users.count
            
            for user in users {
                Task {
                    try? await videoStreamingService.startVideoStream(for: challenge)
                    try? await videoStreamingService.addStomps(1000, for: user.id)
                    expectation.fulfill()
                }
            }
            
            wait(for: [expectation], timeout: 10.0)
        }
    }
    
    // MARK: - Helper Methods
    
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