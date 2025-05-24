import XCTest
@testable import StompCoin

class StepCounterTests: XCTestCase {
    var stepCounter: StepCounterManager!
    
    override func setUp() {
        super.setUp()
        stepCounter = StepCounterManager()
    }
    
    override func tearDown() {
        stepCounter = nil
        super.tearDown()
    }
    
    func testStepCounting() {
        // Given
        let expectedSteps = 100
        
        // When
        stepCounter.startCounting()
        stepCounter.updateSteps(expectedSteps)
        
        // Then
        XCTAssertEqual(stepCounter.currentSteps, expectedSteps)
    }
    
    func testStepReset() {
        // Given
        stepCounter.startCounting()
        stepCounter.updateSteps(100)
        
        // When
        stepCounter.resetSteps()
        
        // Then
        XCTAssertEqual(stepCounter.currentSteps, 0)
    }
    
    func testStepConversion() {
        // Given
        let steps = 1000
        let expectedCoins = Double(steps) * 0.1 // 10% Konvertierungsrate
        
        // When
        let coins = stepCounter.convertStepsToCoins(steps)
        
        // Then
        XCTAssertEqual(coins, expectedCoins)
    }
    
    func testFitnessMode() {
        // Given
        stepCounter.startCounting()
        stepCounter.activateFitnessFloor()
        
        // When
        stepCounter.updateSteps(1000)
        
        // Then
        XCTAssertTrue(stepCounter.isFitnessFloorActive)
        XCTAssertNotNil(stepCounter.fitnessDistance)
        XCTAssertNotNil(stepCounter.fitnessCalories)
    }
    
    func testWalkMode() {
        // Given
        stepCounter.startCounting()
        
        // When
        stepCounter.updateSteps(1000)
        
        // Then
        XCTAssertFalse(stepCounter.isFitnessFloorActive)
        XCTAssertNil(stepCounter.fitnessDistance)
        XCTAssertNil(stepCounter.fitnessCalories)
    }
    
    func testStepHistory() {
        // Given
        let steps = [100, 200, 300]
        
        // When
        for step in steps {
            stepCounter.updateSteps(step)
        }
        
        // Then
        XCTAssertEqual(stepCounter.stepHistory.count, steps.count)
    }
    
    func testStepValidation() {
        // Given
        let invalidSteps = -100
        
        // When
        stepCounter.updateSteps(invalidSteps)
        
        // Then
        XCTAssertEqual(stepCounter.currentSteps, 0)
    }
    
    func testStepPersistence() {
        // Given
        stepCounter.startCounting()
        stepCounter.updateSteps(500)
        
        // When
        let savedSteps = stepCounter.currentSteps
        stepCounter.resetSteps()
        stepCounter.loadSavedSteps()
        
        // Then
        XCTAssertEqual(stepCounter.currentSteps, savedSteps)
    }
    
    func testStepThreshold() {
        // Given
        let threshold = 1000
        
        // When
        stepCounter.updateSteps(threshold)
        
        // Then
        XCTAssertTrue(stepCounter.hasReachedThreshold(threshold))
    }
    
    func testStepStatistics() {
        // Given
        let steps = [100, 200, 300, 400, 500]
        
        // When
        for step in steps {
            stepCounter.updateSteps(step)
        }
        
        // Then
        XCTAssertEqual(stepCounter.averageSteps, 300)
        XCTAssertEqual(stepCounter.maxSteps, 500)
        XCTAssertEqual(stepCounter.minSteps, 100)
    }
} 