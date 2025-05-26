import SwiftUI

struct SubscriptionView: View {
    @StateObject private var subscriptionService = SubscriptionService.shared
    @State private var selectedSubscription: SubscriptionType?
    @State private var showingPurchaseConfirmation = false
    @State private var showingCancellationInfo = false
    @State private var showingError = false
    @State private var errorMessage = ""
    
    var body: some View {
        List {
            Section(header: Text("Aktuelles Abonnement")) {
                if let subscription = subscriptionService.currentSubscription {
                    VStack(alignment: .leading, spacing: 10) {
                        Text(subscription.type.rawValue)
                            .font(.headline)
                        
                        if let endDate = subscription.endDate {
                            Text("Läuft bis: \(endDate.formatted(date: .long, time: .omitted))")
                                .foregroundColor(.secondary)
                        }
                        
                        Text("Monatliche Einnahmen: \(subscription.monthlyEarnings, format: .currency(code: "EUR"))")
                        
                        Text("Entwickler-Abgabe: \(subscription.developerPayout, format: .currency(code: "EUR"))")
                            .foregroundColor(.secondary)
                        
                        Text("Deine Auszahlung: \(subscription.userPayout, format: .currency(code: "EUR"))")
                            .foregroundColor(.green)
                        
                        Text("Nächste Auszahlung: \(subscription.nextPayoutDate.formatted(date: .long, time: .omitted))")
                            .font(.caption)
                        
                        if subscription.type == .vipPremium24Months {
                            Divider()
                            
                            Text("VIP Premium Vorteile:")
                                .font(.headline)
                            
                            Text("• 2 kostenlose Tokens/Items monatlich")
                            Text("• Verfügbar ab: \(subscription.nextBonusClaimDate?.formatted(date: .long, time: .omitted) ?? "")")
                            
                            if subscriptionService.availableBonusTokens > 0 {
                                Text("Verfügbare Bonus-Tokens: \(subscriptionService.availableBonusTokens)")
                                    .foregroundColor(.green)
                            }
                            
                            Button("Bonus-Tokens abholen") {
                                Task {
                                    do {
                                        try await subscriptionService.claimMonthlyBonus()
                                    } catch {
                                        errorMessage = error.localizedDescription
                                        showingError = true
                                    }
                                }
                            }
                            .disabled(subscription.nextBonusClaimDate == nil || Date() < subscription.nextBonusClaimDate!)
                        }
                        
                        if subscription.autoRenew {
                            Text("Automatische Verlängerung aktiv")
                                .foregroundColor(.green)
                        }
                        
                        Button("Abonnement kündigen") {
                            showingCancellationInfo = true
                        }
                        .foregroundColor(.red)
                    }
                } else {
                    Text("Freemium")
                        .font(.headline)
                    Text("40% Abgabe an Entwickler")
                        .foregroundColor(.secondary)
                }
            }
            
            Section(header: Text("Verfügbare Abonnements")) {
                ForEach([SubscriptionType.premium3Months,
                        .premium6Months,
                        .premium12Months,
                        .vipPremium24Months], id: \.self) { type in
                    Button(action: {
                        selectedSubscription = type
                        showingPurchaseConfirmation = true
                    }) {
                        HStack {
                            VStack(alignment: .leading) {
                                Text(type.rawValue)
                                    .font(.headline)
                                Text("\(Int(type.monthlyFeePercentage * 100))% Abgabe")
                                    .font(.caption)
                                    .foregroundColor(.secondary)
                                
                                if type == .vipPremium24Months {
                                    Text("2 kostenlose Tokens/Items monatlich")
                                        .font(.caption)
                                        .foregroundColor(.green)
                                }
                            }
                            
                            Spacer()
                            
                            Text(type.price, format: .currency(code: "EUR"))
                                .font(.headline)
                        }
                    }
                }
            }
            
            Section(header: Text("Vorteile")) {
                VStack(alignment: .leading, spacing: 10) {
                    Text("Premium-Vorteile:")
                        .font(.headline)
                    
                    Group {
                        Text("• Niedrigere Abgabe an Entwickler")
                        Text("• Exklusive Features")
                        Text("• Prioritäts-Support")
                        Text("• Früherer Zugang zu neuen Features")
                    }
                    .font(.subheadline)
                }
            }
            
            Section(header: Text("Wichtige Hinweise")) {
                VStack(alignment: .leading, spacing: 10) {
                    Text("• Alle Abonnements werden automatisch verlängert")
                    Text("• Kündigung nur per E-Mail an support@stompcoin.com")
                    Text("• Kündigung wird erst nach Ablauf des aktuellen Abonnements wirksam")
                    Text("• Zahlung erfolgt im Voraus")
                }
                .font(.caption)
                .foregroundColor(.secondary)
            }
        }
        .navigationTitle("Abonnement")
        .alert("Abonnement kaufen", isPresented: $showingPurchaseConfirmation) {
            Button("Abbrechen", role: .cancel) { }
            Button("Kaufen") {
                if let type = selectedSubscription {
                    Task {
                        do {
                            try await subscriptionService.purchaseSubscription(type)
                        } catch {
                            errorMessage = error.localizedDescription
                            showingError = true
                        }
                    }
                }
            }
        } message: {
            if let type = selectedSubscription {
                Text("Möchten Sie das \(type.rawValue) Abonnement für \(type.price, format: .currency(code: "EUR")) kaufen?")
            }
        }
        .alert("Abonnement kündigen", isPresented: $showingCancellationInfo) {
            Button("Abbrechen", role: .cancel) { }
            Button("Kündigen") {
                Task {
                    do {
                        try await subscriptionService.cancelSubscription()
                    } catch {
                        errorMessage = error.localizedDescription
                        showingError = true
                    }
                }
            }
        } message: {
            Text("Um Ihr Abonnement zu kündigen, senden Sie bitte eine E-Mail an support@stompcoin.com. Die Kündigung wird erst nach Ablauf des aktuellen Abonnements wirksam.")
        }
        .alert("Fehler", isPresented: $showingError) {
            Button("OK", role: .cancel) { }
        } message: {
            Text(errorMessage)
        }
    }
}

#Preview {
    NavigationView {
        SubscriptionView()
    }
} 