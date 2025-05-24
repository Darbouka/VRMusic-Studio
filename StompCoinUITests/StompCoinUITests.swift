//
//  StompCoinUITests.swift
//  StompCoinUITests
//
//  Created by fatih on 22.05.25.
//

import XCTest

class StompCoinUITests: XCTestCase {
    var app: XCUIApplication!
    
    override func setUp() {
        super.setUp()
        app = XCUIApplication()
        app.launch()
    }
    
    override func tearDown() {
        app = nil
        super.tearDown()
    }
    
    func testDashboardNavigation() {
        // Test Dashboard Navigation
        XCTAssertTrue(app.navigationBars["Dashboard"].exists)
        
        // Test Tab Navigation
        app.tabBars.buttons["Events"].tap()
        XCTAssertTrue(app.navigationBars["Events"].exists)
        
        app.tabBars.buttons["Profile"].tap()
        XCTAssertTrue(app.navigationBars["Profile"].exists)
    }
    
    func testStepCounter() {
        // Test Step Counter UI
        let stepCounter = app.staticTexts["stepCount"]
        XCTAssertTrue(stepCounter.exists)
        
        // Test Start/Stop Button
        let startButton = app.buttons["startCounting"]
        XCTAssertTrue(startButton.exists)
        startButton.tap()
        
        // Verify counting started
        XCTAssertTrue(app.staticTexts["Counting..."].exists)
    }
    
    func testBoostShop() {
        // Navigate to Boost Shop
        app.tabBars.buttons["Shop"].tap()
        
        // Test Boost Cards
        let boostCards = app.scrollViews.otherElements.containing(.button, identifier: "boostCard")
        XCTAssertTrue(boostCards.count > 0)
        
        // Test Purchase Flow
        boostCards.firstMatch.tap()
        XCTAssertTrue(app.buttons["Purchase"].exists)
    }
    
    func testEventParticipation() {
        // Navigate to Events
        app.tabBars.buttons["Events"].tap()
        
        // Test Event List
        let eventList = app.tables["eventList"]
        XCTAssertTrue(eventList.exists)
        
        // Test Event Details
        eventList.cells.firstMatch.tap()
        XCTAssertTrue(app.buttons["Join Event"].exists)
    }
    
    func testLeaderboard() {
        // Navigate to Leaderboard
        app.tabBars.buttons["Leaderboard"].tap()
        
        // Test Time Frame Selection
        let timeFramePicker = app.pickers["timeFramePicker"]
        XCTAssertTrue(timeFramePicker.exists)
        
        // Test Leaderboard Entries
        let leaderboardEntries = app.tables["leaderboardTable"]
        XCTAssertTrue(leaderboardEntries.exists)
    }
    
    func testSettings() {
        // Navigate to Settings
        app.tabBars.buttons["Settings"].tap()
        
        // Test Settings Options
        let settingsTable = app.tables["settingsTable"]
        XCTAssertTrue(settingsTable.exists)
        
        // Test Theme Selection
        settingsTable.cells["Appearance"].tap()
        XCTAssertTrue(app.buttons["Dark Mode"].exists)
    }
    
    func testProfile() {
        // Navigate to Profile
        app.tabBars.buttons["Profile"].tap()
        
        // Test Profile Information
        XCTAssertTrue(app.staticTexts["username"].exists)
        XCTAssertTrue(app.staticTexts["coinBalance"].exists)
        
        // Test Edit Profile
        app.buttons["Edit Profile"].tap()
        XCTAssertTrue(app.textFields["usernameField"].exists)
    }
    
    func testMusicGenres() {
        // Navigate to Music Genres
        app.tabBars.buttons["Music"].tap()
        
        // Test Genre Selection
        let genreGrid = app.collectionViews["genreGrid"]
        XCTAssertTrue(genreGrid.exists)
        
        // Test Genre Details
        genreGrid.cells.firstMatch.tap()
        XCTAssertTrue(app.staticTexts["genreDescription"].exists)
    }
    
    func testFitnessMode() {
        // Navigate to Fitness Mode
        app.tabBars.buttons["Fitness"].tap()
        
        // Test Fitness Stats
        XCTAssertTrue(app.staticTexts["distance"].exists)
        XCTAssertTrue(app.staticTexts["calories"].exists)
        
        // Test Start Workout
        app.buttons["Start Workout"].tap()
        XCTAssertTrue(app.staticTexts["Workout in Progress"].exists)
    }
    
    func testOfflineMode() {
        // Simulate Offline Mode
        app.launchArguments = ["UI-Testing", "Offline-Mode"]
        app.launch()
        
        // Test Offline Indicator
        XCTAssertTrue(app.staticTexts["Offline Mode"].exists)
        
        // Test Cached Data
        let cachedData = app.tables["cachedDataTable"]
        XCTAssertTrue(cachedData.exists)
    }
    
    func testErrorHandling() {
        // Simulate Error State
        app.launchArguments = ["UI-Testing", "Error-State"]
        app.launch()
        
        // Test Error Alert
        XCTAssertTrue(app.alerts["Error"].exists)
        
        // Test Retry Button
        app.alerts["Error"].buttons["Retry"].tap()
        XCTAssertFalse(app.alerts["Error"].exists)
    }
}
