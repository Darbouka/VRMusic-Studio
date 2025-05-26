import XCTest

class UITests: XCTestCase {
    let app = XCUIApplication()
    
    override func setUp() {
        super.setUp()
        continueAfterFailure = false
        app.launch()
    }
    
    func testStreamingInterface() {
        // Stream starten
        app.buttons["streaming.start"].tap()
        
        // Kamera wechseln
        app.buttons["streaming.camera.switch"].tap()
        
        // Stream beenden
        app.buttons["streaming.end"].tap()
        
        // Überprüfen
        XCTAssertTrue(app.buttons["streaming.start"].exists)
    }
    
    func testRewardsView() {
        // Stream starten
        app.buttons["streaming.start"].tap()
        
        // Belohnungen öffnen
        app.buttons["rewards.title"].tap()
        
        // Überprüfen
        XCTAssertTrue(app.staticTexts["rewards.top.streamer"].exists)
        XCTAssertTrue(app.staticTexts["rewards.most.viewers"].exists)
    }
    
    func testLanguageSelection() {
        // Einstellungen öffnen
        app.buttons["settings"].tap()
        
        // Sprachauswahl öffnen
        app.buttons["language"].tap()
        
        // Sprache ändern
        app.buttons["English"].tap()
        
        // Überprüfen
        XCTAssertTrue(app.staticTexts["Live Streaming"].exists)
    }
    
    func testPremiumFeatures() {
        // Premium-Features öffnen
        app.buttons["premium.features"].tap()
        
        // Überprüfen
        XCTAssertTrue(app.staticTexts["premium.vip"].exists)
        XCTAssertTrue(app.staticTexts["premium.standard"].exists)
    }
    
    func testChallengeParticipation() {
        // Challenge öffnen
        app.buttons["challenge.title"].tap()
        
        // Challenge beitreten
        app.buttons["join"].tap()
        
        // Überprüfen
        XCTAssertTrue(app.staticTexts["challenge.status"].exists)
    }
    
    func testStreamStatistics() {
        // Stream starten
        app.buttons["streaming.start"].tap()
        
        // Überprüfen
        XCTAssertTrue(app.staticTexts["stats.stomps"].exists)
        XCTAssertTrue(app.staticTexts["stats.stompcoins"].exists)
        XCTAssertTrue(app.staticTexts["stats.omcoins"].exists)
    }
    
    func testPrivateStream() {
        // Stream starten
        app.buttons["streaming.start"].tap()
        
        // Privaten Stream aktivieren
        app.switches["streaming.private"].tap()
        
        // Zuschauer verwalten
        app.buttons["streaming.viewers"].tap()
        
        // Überprüfen
        XCTAssertTrue(app.staticTexts["streaming.viewers"].exists)
    }
    
    func testOMCoinProgress() {
        // Stream starten
        app.buttons["streaming.start"].tap()
        
        // Überprüfen
        XCTAssertTrue(app.progressIndicators["stats.progress"].exists)
        XCTAssertTrue(app.staticTexts["stats.time.remaining"].exists)
    }
    
    func testNavigation() {
        // Hauptnavigation testen
        app.tabBars.buttons["Home"].tap()
        app.tabBars.buttons["Challenges"].tap()
        app.tabBars.buttons["Rewards"].tap()
        app.tabBars.buttons["Settings"].tap()
        
        // Überprüfen
        XCTAssertTrue(app.navigationBars["StompCoin"].exists)
    }
    
    func testErrorHandling() {
        // Stream ohne Challenge starten
        app.buttons["streaming.start"].tap()
        
        // Überprüfen
        XCTAssertTrue(app.alerts["error"].exists)
        app.alerts["error"].buttons["ok"].tap()
    }
} 