import Foundation

struct FitnessData: Codable {
    var steps: Int
    var calories: Double
    var distance: Double
    var activeMinutes: Int
    var lastUpdated: Date
} 