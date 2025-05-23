import SwiftUI

struct StepCounterView: View {
    @StateObject private var stepManager = StepCounterManager.shared
    @State private var showingDatePicker = false
    @State private var selectedDate = Date()
    
    var body: some View {
        VStack(spacing: 20) {
            // Schrittzähler-Anzeige
            VStack {
                Text(stepManager.formatSteps(stepManager.steps))
                    .font(.system(size: 60, weight: .bold))
                    .foregroundColor(.primary)
                
                Text("Schritte")
                    .font(.title2)
                    .foregroundColor(.secondary)
            }
            .padding()
            .frame(maxWidth: .infinity)
            .background(Color.secondary.opacity(0.1))
            .cornerRadius(15)
            
            // Fitness-Floor Statistiken
            if stepManager.isFitnessFloorActive {
                VStack(spacing: 15) {
                    if let distance = stepManager.calculateFitnessDistance() {
                        HStack {
                            Image(systemName: "figure.walk")
                            Text(String(format: "%.1f km", distance / 1000))
                                .font(.title3)
                        }
                    }
                    
                    if let calories = stepManager.calculateFitnessCalories() {
                        HStack {
                            Image(systemName: "flame.fill")
                            Text(String(format: "%.0f kcal", calories))
                                .font(.title3)
                        }
                    }
                    
                    if let duration = stepManager.getFitnessDuration() {
                        HStack {
                            Image(systemName: "clock")
                            Text(formatDuration(duration))
                                .font(.title3)
                        }
                    }
                }
                .padding()
                .background(Color.orange.opacity(0.1))
                .cornerRadius(15)
            }
            
            // Steuerung
            VStack(spacing: 15) {
                Button(action: {
                    if stepManager.isCounting {
                        stepManager.stopCounting()
                    } else {
                        stepManager.startCounting()
                    }
                }) {
                    Text(stepManager.isCounting ? "Stoppen" : "Starten")
                        .font(.headline)
                        .foregroundColor(.white)
                        .frame(maxWidth: .infinity)
                        .padding()
                        .background(stepManager.isCounting ? Color.red : Color.green)
                        .cornerRadius(10)
                }
                
                Button(action: {
                    if stepManager.isFitnessFloorActive {
                        stepManager.deactivateFitnessFloor()
                    } else {
                        stepManager.activateFitnessFloor()
                    }
                }) {
                    Text(stepManager.isFitnessFloorActive ? "Fitness Floor beenden" : "Fitness Floor starten")
                        .font(.headline)
                        .foregroundColor(.white)
                        .frame(maxWidth: .infinity)
                        .padding()
                        .background(stepManager.isFitnessFloorActive ? Color.red : Color.orange)
                        .cornerRadius(10)
                }
                
                Button(action: {
                    stepManager.getStepsForToday()
                }) {
                    Text("Heute")
                        .font(.headline)
                        .foregroundColor(.white)
                        .frame(maxWidth: .infinity)
                        .padding()
                        .background(Color.blue)
                        .cornerRadius(10)
                }
                
                Button(action: {
                    showingDatePicker = true
                }) {
                    Text("Datum auswählen")
                        .font(.headline)
                        .foregroundColor(.white)
                        .frame(maxWidth: .infinity)
                        .padding()
                        .background(Color.orange)
                        .cornerRadius(10)
                }
            }
            
            if let error = stepManager.error {
                Text(error)
                    .foregroundColor(.red)
                    .padding()
            }
            
            if let lastUpdate = stepManager.lastUpdate {
                Text("Letzte Aktualisierung: \(lastUpdate.formatted())")
                    .font(.caption)
                    .foregroundColor(.secondary)
            }
        }
        .padding()
        .sheet(isPresented: $showingDatePicker) {
            DatePickerView(selectedDate: $selectedDate) { startDate, endDate in
                stepManager.getStepsForDateRange(from: startDate, to: endDate)
            }
        }
    }
    
    private func formatDuration(_ duration: TimeInterval) -> String {
        let hours = Int(duration) / 3600
        let minutes = Int(duration) / 60 % 60
        let seconds = Int(duration) % 60
        
        if hours > 0 {
            return String(format: "%d:%02d:%02d", hours, minutes, seconds)
        } else {
            return String(format: "%02d:%02d", minutes, seconds)
        }
    }
}

struct DatePickerView: View {
    @Environment(\.dismiss) private var dismiss
    @Binding var selectedDate: Date
    let onDateRangeSelected: (Date, Date) -> Void
    
    var body: some View {
        NavigationView {
            VStack {
                DatePicker(
                    "Datum auswählen",
                    selection: $selectedDate,
                    displayedComponents: [.date]
                )
                .datePickerStyle(.graphical)
                .padding()
                
                Button("Schritte abrufen") {
                    let calendar = Calendar.current
                    let startOfDay = calendar.startOfDay(for: selectedDate)
                    let endOfDay = calendar.date(byAdding: .day, value: 1, to: startOfDay)!
                    
                    onDateRangeSelected(startOfDay, endOfDay)
                    dismiss()
                }
                .buttonStyle(.borderedProminent)
                .padding()
            }
            .navigationTitle("Datum auswählen")
            .navigationBarTitleDisplayMode(.inline)
            .toolbar {
                ToolbarItem(placement: .navigationBarTrailing) {
                    Button("Abbrechen") {
                        dismiss()
                    }
                }
            }
        }
    }
}

#Preview {
    StepCounterView()
} 