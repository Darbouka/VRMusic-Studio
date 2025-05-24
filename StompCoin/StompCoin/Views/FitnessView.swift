import SwiftUI
import HealthKit

struct FitnessView: View {
    @StateObject private var viewModel = FitnessViewModel()
    @State private var showingSettings = false
    
    var body: some View {
        NavigationView {
            ScrollView {
                VStack(spacing: 20) {
                    // Live Tracking
                    LiveTrackingCard(
                        steps: viewModel.currentSteps,
                        calories: viewModel.currentCalories,
                        distance: viewModel.currentDistance
                    )
                    
                    // Tagesziele
                    GoalsProgressCard(
                        stepsGoal: viewModel.stepsGoal,
                        currentSteps: viewModel.currentSteps,
                        caloriesGoal: viewModel.caloriesGoal,
                        currentCalories: viewModel.currentCalories
                    )
                    
                    // Wöchentliche Statistiken
                    WeeklyStatsCard(stats: viewModel.weeklyStats)
                    
                    // Aktivitätshistorie
                    ActivityHistoryCard(activities: viewModel.recentActivities)
                }
                .padding()
            }
            .navigationTitle("Fitness")
            .toolbar {
                ToolbarItem(placement: .navigationBarTrailing) {
                    Button(action: { showingSettings = true }) {
                        Image(systemName: "gear")
                    }
                }
            }
            .sheet(isPresented: $showingSettings) {
                FitnessSettingsView(viewModel: viewModel)
            }
            .refreshable {
                await viewModel.refresh()
            }
        }
    }
}

struct LiveTrackingCard: View {
    let steps: Int
    let calories: Double
    let distance: Double
    
    var body: some View {
        VStack(spacing: 15) {
            Text("Live Tracking")
                .font(.headline)
            
            HStack {
                TrackingItem(
                    title: "Schritte",
                    value: "\(steps)",
                    icon: "figure.walk",
                    color: .blue
                )
                
                TrackingItem(
                    title: "Kalorien",
                    value: "\(Int(calories))",
                    icon: "flame.fill",
                    color: .orange
                )
                
                TrackingItem(
                    title: "Distanz",
                    value: String(format: "%.1f km", distance),
                    icon: "map.fill",
                    color: .green
                )
            }
        }
        .padding()
        .background(Color(.systemBackground))
        .cornerRadius(15)
        .shadow(radius: 5)
    }
}

struct TrackingItem: View {
    let title: String
    let value: String
    let icon: String
    let color: Color
    
    var body: some View {
        VStack {
            Image(systemName: icon)
                .font(.title2)
                .foregroundColor(color)
            
            Text(value)
                .font(.headline)
            
            Text(title)
                .font(.caption)
                .foregroundColor(.gray)
        }
        .frame(maxWidth: .infinity)
    }
}

struct GoalsProgressCard: View {
    let stepsGoal: Int
    let currentSteps: Int
    let caloriesGoal: Double
    let currentCalories: Double
    
    var body: some View {
        VStack(spacing: 15) {
            Text("Tagesziele")
                .font(.headline)
            
            VStack(spacing: 10) {
                GoalProgressBar(
                    title: "Schritte",
                    current: currentSteps,
                    goal: stepsGoal,
                    color: .blue
                )
                
                GoalProgressBar(
                    title: "Kalorien",
                    current: Int(currentCalories),
                    goal: Int(caloriesGoal),
                    color: .orange
                )
            }
        }
        .padding()
        .background(Color(.systemBackground))
        .cornerRadius(15)
        .shadow(radius: 5)
    }
}

struct GoalProgressBar: View {
    let title: String
    let current: Int
    let goal: Int
    let color: Color
    
    private var progress: Double {
        Double(current) / Double(goal)
    }
    
    var body: some View {
        VStack(alignment: .leading) {
            HStack {
                Text(title)
                    .font(.subheadline)
                
                Spacer()
                
                Text("\(current)/\(goal)")
                    .font(.caption)
                    .foregroundColor(.gray)
            }
            
            GeometryReader { geometry in
                ZStack(alignment: .leading) {
                    Rectangle()
                        .fill(Color.gray.opacity(0.2))
                        .frame(height: 8)
                        .cornerRadius(4)
                    
                    Rectangle()
                        .fill(color)
                        .frame(width: geometry.size.width * progress, height: 8)
                        .cornerRadius(4)
                }
            }
            .frame(height: 8)
        }
    }
}

struct WeeklyStatsCard: View {
    let stats: [DailyStats]
    
    var body: some View {
        VStack(spacing: 15) {
            Text("Wöchentliche Statistiken")
                .font(.headline)
            
            HStack(alignment: .bottom, spacing: 8) {
                ForEach(stats) { stat in
                    VStack {
                        RoundedRectangle(cornerRadius: 5)
                            .fill(Color.blue)
                            .frame(height: CGFloat(stat.steps) / 100)
                        
                        Text(stat.day)
                            .font(.caption)
                    }
                }
            }
            .frame(height: 150)
        }
        .padding()
        .background(Color(.systemBackground))
        .cornerRadius(15)
        .shadow(radius: 5)
    }
}

struct ActivityHistoryCard: View {
    let activities: [Activity]
    
    var body: some View {
        VStack(alignment: .leading, spacing: 15) {
            Text("Aktivitätshistorie")
                .font(.headline)
            
            ForEach(activities) { activity in
                ActivityRow(activity: activity)
            }
        }
        .padding()
        .background(Color(.systemBackground))
        .cornerRadius(15)
        .shadow(radius: 5)
    }
}

struct ActivityRow: View {
    let activity: Activity
    
    var body: some View {
        HStack {
            Image(systemName: activity.icon)
                .font(.title2)
                .foregroundColor(.blue)
            
            VStack(alignment: .leading) {
                Text(activity.title)
                    .font(.headline)
                
                Text(activity.date, style: .time)
                    .font(.caption)
                    .foregroundColor(.gray)
            }
            
            Spacer()
            
            Text("\(activity.steps) Schritte")
                .font(.caption)
                .foregroundColor(.gray)
        }
        .padding(.vertical, 5)
    }
}

struct FitnessSettingsView: View {
    @ObservedObject var viewModel: FitnessViewModel
    @Environment(\.dismiss) var dismiss
    
    var body: some View {
        NavigationView {
            Form {
                Section(header: Text("Tagesziele")) {
                    Stepper("Schritte: \(viewModel.stepsGoal)", value: $viewModel.stepsGoal, in: 1000...20000, step: 1000)
                    Stepper("Kalorien: \(Int(viewModel.caloriesGoal))", value: $viewModel.caloriesGoal, in: 100...1000, step: 100)
                }
                
                Section(header: Text("Benachrichtigungen")) {
                    Toggle("Tägliche Erinnerungen", isOn: $viewModel.dailyReminders)
                    Toggle("Zielbenachrichtigungen", isOn: $viewModel.goalNotifications)
                }
                
                Section(header: Text("Datenschutz")) {
                    Toggle("Gesundheitsdaten teilen", isOn: $viewModel.shareHealthData)
                }
            }
            .navigationTitle("Einstellungen")
            .navigationBarItems(trailing: Button("Fertig") {
                dismiss()
            })
        }
    }
}

// MARK: - ViewModel

class FitnessViewModel: ObservableObject {
    @Published var currentSteps = 0
    @Published var currentCalories = 0.0
    @Published var currentDistance = 0.0
    
    @Published var stepsGoal = 10000
    @Published var caloriesGoal = 500.0
    
    @Published var weeklyStats: [DailyStats] = []
    @Published var recentActivities: [Activity] = []
    
    @Published var dailyReminders = true
    @Published var goalNotifications = true
    @Published var shareHealthData = false
    
    private let healthStore = HKHealthStore()
    
    init() {
        requestHealthKitAuthorization()
        startLiveTracking()
    }
    
    func refresh() async {
        // Implementierung der Datenaktualisierung
    }
    
    private func requestHealthKitAuthorization() {
        let types = Set([
            HKObjectType.quantityType(forIdentifier: .stepCount)!,
            HKObjectType.quantityType(forIdentifier: .activeEnergyBurned)!,
            HKObjectType.quantityType(forIdentifier: .distanceWalkingRunning)!
        ])
        
        healthStore.requestAuthorization(toShare: nil, read: types) { success, error in
            if let error = error {
                print("HealthKit Authorization Error: \(error.localizedDescription)")
            }
        }
    }
    
    private func startLiveTracking() {
        // Implementierung des Live-Trackings
    }
}

// MARK: - Modelle

struct DailyStats: Identifiable {
    let id = UUID()
    let day: String
    let steps: Int
    let calories: Double
    let distance: Double
}

struct Activity: Identifiable {
    let id = UUID()
    let title: String
    let icon: String
    let date: Date
    let steps: Int
    let calories: Double
    let distance: Double
} 