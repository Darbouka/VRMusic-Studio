import XCTest
@testable import StompCoin

class PayoutServiceTests: XCTestCase {
    var payoutService: PayoutService!
    
    override func setUp() {
        super.setUp()
        payoutService = PayoutService.shared
    }
    
    override func tearDown() {
        payoutService = nil
        super.tearDown()
    }
    
    func testDeveloperPayout() async throws {
        // Initiale Balance prüfen
        let initialBalance = payoutService.getDeveloperBalance()
        
        // Auszahlung durchführen
        try await payoutService.processDeveloperPayout(amount: 200)
        
        // Neue Balance prüfen
        let newBalance = payoutService.getDeveloperBalance()
        XCTAssertEqual(newBalance, initialBalance + 200)
    }
    
    func testMultiplePayouts() async throws {
        // Initiale Balance prüfen
        let initialBalance = payoutService.getDeveloperBalance()
        
        // Mehrere Auszahlungen durchführen
        try await payoutService.processDeveloperPayout(amount: 100)
        try await payoutService.processDeveloperPayout(amount: 200)
        try await payoutService.processDeveloperPayout(amount: 300)
        
        // Finale Balance prüfen
        let finalBalance = payoutService.getDeveloperBalance()
        XCTAssertEqual(finalBalance, initialBalance + 600)
    }
    
    func testLargePayout() async throws {
        // Initiale Balance prüfen
        let initialBalance = payoutService.getDeveloperBalance()
        
        // Große Auszahlung durchführen
        try await payoutService.processDeveloperPayout(amount: 10000)
        
        // Neue Balance prüfen
        let newBalance = payoutService.getDeveloperBalance()
        XCTAssertEqual(newBalance, initialBalance + 10000)
    }
} 