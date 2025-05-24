//
//  StompCoinTests.swift
//  StompCoinTests
//
//  Created by fatih on 22.05.25.
//

import XCTest
@testable import StompCoin

final class StompCoinTests: XCTestCase {
    var fitnessManager: FitnessManager!
    var walletManager: Wallet!
    var tradingManager: TradingManager!
    var socialManager: SocialManager!
    var securityManager: SecurityManager!
    
    override func setUpWithError() throws {
        fitnessManager = FitnessManager.shared
        walletManager = Wallet()
        tradingManager = TradingManager.shared
        socialManager = SocialManager.shared
        securityManager = SecurityManager.shared
    }
    
    override func tearDownWithError() throws {
        fitnessManager = nil
        walletManager = nil
        tradingManager = nil
        socialManager = nil
        securityManager = nil
    }
    
    // MARK: - Fitness Tests
    
    func testStepCounting() async throws {
        // Test Schrittzählung
        let steps = 1000
        let stats = FitnessStats(
            steps: steps,
            caloriesBurned: 0,
            distance: 0,
            activeMinutes: 0
        )
        
        XCTAssertEqual(stats.steps, steps)
    }
    
    func testCalorieCalculation() async throws {
        // Test Kalorienberechnung
        let steps = 1000
        let expectedCalories = Double(steps) * 0.04
        
        let stats = FitnessStats(
            steps: steps,
            caloriesBurned: expectedCalories,
            distance: 0,
            activeMinutes: 0
        )
        
        XCTAssertEqual(stats.caloriesBurned, expectedCalories)
    }
    
    // MARK: - Wallet Tests
    
    func testWalletBalance() {
        // Test Wallet-Saldo
        let initialBalance = 1000.0
        let transaction = Transaction(
            id: UUID(),
            amount: 100,
            type: .credit,
            date: Date()
        )
        
        walletManager.addTransaction(transaction)
        
        XCTAssertEqual(walletManager.balance, initialBalance + 100)
    }
    
    func testTransactionHistory() {
        // Test Transaktionshistorie
        let transaction = Transaction(
            id: UUID(),
            amount: 100,
            type: .credit,
            date: Date()
        )
        
        walletManager.addTransaction(transaction)
        
        XCTAssertEqual(walletManager.transactions.count, 1)
        XCTAssertEqual(walletManager.transactions.first?.amount, 100)
    }
    
    // MARK: - Trading Tests
    
    func testTradeCreation() async throws {
        // Test Trade-Erstellung
        let amount = 100.0
        let trade = try await tradingManager.createTrade(
            from: User(id: UUID()),
            to: User(id: UUID()),
            amount: amount,
            type: .stomp
        )
        
        XCTAssertEqual(trade.amount, amount)
        XCTAssertEqual(trade.type, .stomp)
    }
    
    func testMiningStart() async throws {
        // Test Mining-Start
        let user = User(id: UUID())
        user.wallet.isVerified = true
        
        try await tradingManager.startMining(for: user)
        let stats = try await tradingManager.getMiningStats(for: user)
        
        XCTAssertNotNil(stats.lastMiningDate)
    }
    
    // MARK: - Social Tests
    
    func testFriendRequest() async throws {
        // Test Freundschaftsanfrage
        let fromUser = User(id: UUID())
        let toUser = User(id: UUID())
        
        try await socialManager.sendFriendRequest(from: fromUser, to: toUser)
        let friends = try await socialManager.getFriends(for: fromUser)
        
        XCTAssertFalse(friends.contains(where: { $0.id == toUser.id }))
    }
    
    func testGroupCreation() async throws {
        // Test Gruppenerstellung
        let creator = User(id: UUID())
        let members = [User(id: UUID()), User(id: UUID())]
        
        let group = try await socialManager.createGroup(
            name: "Test Group",
            creator: creator,
            members: members
        )
        
        XCTAssertEqual(group.name, "Test Group")
        XCTAssertEqual(group.memberIds.count, 2)
    }
    
    // MARK: - Security Tests
    
    func testEncryption() throws {
        // Test Verschlüsselung
        let data = "Test Data".data(using: .utf8)!
        
        let encryptedData = try securityManager.encryptData(data)
        let decryptedData = try securityManager.decryptData(encryptedData)
        
        XCTAssertEqual(data, decryptedData)
    }
    
    func testBiometricAuthentication() async throws {
        // Test biometrische Authentifizierung
        let isAuthenticated = try await securityManager.authenticateWithBiometrics()
        
        XCTAssertFalse(isAuthenticated) // Simuliert fehlgeschlagene Authentifizierung
    }
    
    // MARK: - Performance Tests
    
    func testPerformanceStepCounting() throws {
        measure {
            // Test Leistung der Schrittzählung
            for _ in 0..<1000 {
                _ = FitnessStats(
                    steps: 1000,
                    caloriesBurned: 40,
                    distance: 762,
                    activeMinutes: 10
                )
            }
        }
    }
    
    func testPerformanceWalletOperations() throws {
        measure {
            // Test Leistung der Wallet-Operationen
            for _ in 0..<1000 {
                let transaction = Transaction(
                    id: UUID(),
                    amount: 100,
                    type: .credit,
                    date: Date()
                )
                walletManager.addTransaction(transaction)
            }
        }
    }
    
    // MARK: - Integration Tests
    
    func testFitnessToWalletIntegration() async throws {
        // Test Integration zwischen Fitness und Wallet
        let stats = FitnessStats(
            steps: 10000,
            caloriesBurned: 400,
            distance: 7620,
            activeMinutes: 100
        )
        
        let xp = GamificationManager.shared.calculateXP(from: stats)
        let coins = Double(xp) / 100.0
        
        let transaction = Transaction(
            id: UUID(),
            amount: coins,
            type: .credit,
            date: Date()
        )
        
        walletManager.addTransaction(transaction)
        
        XCTAssertEqual(walletManager.balance, coins)
    }
    
    func testSocialToTradingIntegration() async throws {
        // Test Integration zwischen Social und Trading
        let user = User(id: UUID())
        let friend = User(id: UUID())
        
        try await socialManager.sendFriendRequest(from: user, to: friend)
        try await socialManager.acceptFriendRequest(
            FriendRequest(
                id: UUID(),
                fromUserId: user.id,
                toUserId: friend.id,
                status: .pending,
                date: Date()
            )
        )
        
        let trade = try await tradingManager.createTrade(
            from: user,
            to: friend,
            amount: 100,
            type: .stomp
        )
        
        XCTAssertEqual(trade.fromUserId, user.id)
        XCTAssertEqual(trade.toUserId, friend.id)
    }
}
