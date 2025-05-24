import SwiftUI
import SwiftData

struct ProfileView: View {
    @Environment(\.modelContext) private var modelContext
    @Query private var users: [User]
    @State private var showingSubscriptionSheet = false
    @State private var showingStepSettings = false
    @State private var showingEditProfile = false
    @State private var showingSuccess = false
    @State private var showingError = false
    @State private var errorMessage = ""
    
    var currentUser: User? {
        users.first
    }
    
    var body: some View {
        NavigationView {
            List {
                // Profile Header
                Section {
                    HStack {
                        Image(systemName: "person.circle.fill")
                            .font(.system(size: 60))
                            .foregroundColor(.blue)
                        
                        VStack(alignment: .leading) {
                            Text(currentUser?.username ?? "Username")
                                .font(.title2)
                                .bold()
                            
                            Text(currentUser?.email ?? "email@example.com")
                                .font(.subheadline)
                                .foregroundColor(.secondary)
                            
                            if let subscription = currentUser?.subscriptionType {
                                Text(subscription.rawValue.capitalized)
                                    .font(.caption)
                                    .padding(.horizontal, 8)
                                    .padding(.vertical, 4)
                                    .background(Color.blue.opacity(0.1))
                                    .cornerRadius(4)
                            }
                        }
                    }
                    .padding(.vertical, 8)
                }
                
                // Account Settings
                Section("Account Settings") {
                    Button(action: { showingEditProfile = true }) {
                        Label("Edit Profile", systemImage: "person.fill")
                    }
                    
                    Button(action: { showingStepSettings = true }) {
                        Label("Step Settings", systemImage: "figure.walk")
                    }
                    
                    if currentUser?.isPremium == false {
                        Button(action: { showingSubscriptionSheet = true }) {
                            Label("Upgrade to Premium", systemImage: "star.fill")
                        }
                    }
                }
                
                // Statistics
                Section("Statistics") {
                    StatisticRow(title: "Total Steps", value: "\(currentUser?.currentSteps ?? 0)")
                    StatisticRow(title: "StompCoins Earned", value: "\(Int(currentUser?.stompCoins ?? 0))")
                    StatisticRow(title: "OM Coins Earned", value: "\(Int(currentUser?.omCoins ?? 0))")
                    StatisticRow(title: "Events Attended", value: "\(currentUser?.eventHistory.count ?? 0)")
                }
                
                // Favorite Genres
                if let genres = currentUser?.favoriteMusicGenres, !genres.isEmpty {
                    Section("Favorite Genres") {
                        ScrollView(.horizontal, showsIndicators: false) {
                            HStack {
                                ForEach(genres, id: \.self) { genre in
                                    Text(genre.rawValue)
                                        .font(.caption)
                                        .padding(.horizontal, 8)
                                        .padding(.vertical, 4)
                                        .background(Color.blue.opacity(0.1))
                                        .cornerRadius(4)
                                }
                            }
                        }
                    }
                }
                
                // Subscription Information
                if let subscription = currentUser?.subscriptionType {
                    Section("Subscription") {
                        VStack(alignment: .leading, spacing: 8) {
                            Text("Current Plan: \(subscription.rawValue.capitalized)")
                                .font(.headline)
                            
                            if let endDate = currentUser?.subscriptionEndDate {
                                Text("Valid until: \(endDate, style: .date)")
                                    .font(.subheadline)
                                    .foregroundColor(.secondary)
                            }
                            
                            Button("Manage Subscription") {
                                showingSubscriptionSheet = true
                            }
                            .font(.subheadline)
                            .foregroundColor(.blue)
                        }
                    }
                }
            }
            .navigationTitle("Profile")
            .sheet(isPresented: $showingSubscriptionSheet) {
                SubscriptionView(user: currentUser)
            }
            .sheet(isPresented: $showingStepSettings) {
                StepSettingsView(user: currentUser)
            }
            .sheet(isPresented: $showingEditProfile) {
                EditProfileView(user: currentUser)
            }
        }
    }
    
    private func updateProfile() {
        guard let user = currentUser else { return }
        
        let success = AppManager.shared.updateProfile(
            user: user,
            username: username,
            email: email,
            favoriteGenres: selectedGenres,
            modelContext: modelContext
        )
        
        if success {
            showingSuccess = true
        } else {
            showingError = true
            errorMessage = "Profil konnte nicht aktualisiert werden"
        }
    }
    
    private func updateStepLimits() {
        guard let user = currentUser else { return }
        
        let success = AppManager.shared.updateStepLimits(
            user: user,
            dailyLimit: dailyStepLimit,
            weeklyLimit: weeklyStepLimit,
            weekendLimit: weekendStepLimit,
            modelContext: modelContext
        )
        
        if success {
            showingSuccess = true
        } else {
            showingError = true
            errorMessage = "Schrittlimits konnten nicht aktualisiert werden"
        }
    }
    
    private func handleSubscription() {
        guard let user = currentUser else { return }
        
        let success = PaymentManager.shared.purchaseSubscription(
            productId: selectedSubscription,
            user: user,
            modelContext: modelContext
        )
        
        if success {
            showingSuccess = true
        } else {
            showingError = true
            errorMessage = "Abonnement konnte nicht aktiviert werden"
        }
    }
}

struct StatisticRow: View {
    let title: String
    let value: String
    
    var body: some View {
        HStack {
            Text(title)
            Spacer()
            Text(value)
                .bold()
        }
    }
}

struct EditProfileView: View {
    @Environment(\.dismiss) private var dismiss
    @Environment(\.modelContext) private var modelContext
    let user: User?
    @State private var username: String = ""
    @State private var email: String = ""
    @State private var selectedGenres: Set<MusicGenre> = []
    
    var body: some View {
        NavigationView {
            Form {
                Section("Personal Information") {
                    TextField("Username", text: $username)
                    TextField("Email", text: $email)
                        .keyboardType(.emailAddress)
                        .autocapitalization(.none)
                }
                
                Section("Favorite Music Genres") {
                    ForEach(MusicGenre.allCases, id: \.self) { genre in
                        Toggle(genre.rawValue, isOn: Binding(
                            get: { selectedGenres.contains(genre) },
                            set: { isSelected in
                                if isSelected {
                                    selectedGenres.insert(genre)
                                } else {
                                    selectedGenres.remove(genre)
                                }
                            }
                        ))
                    }
                }
            }
            .navigationTitle("Edit Profile")
            .navigationBarTitleDisplayMode(.inline)
            .toolbar {
                ToolbarItem(placement: .navigationBarLeading) {
                    Button("Cancel") {
                        dismiss()
                    }
                }
                
                ToolbarItem(placement: .navigationBarTrailing) {
                    Button("Save") {
                        // TODO: Implement profile update
                        dismiss()
                    }
                }
            }
            .onAppear {
                if let user = user {
                    username = user.username
                    email = user.email
                    selectedGenres = Set(user.favoriteMusicGenres)
                }
            }
        }
    }
}

struct StepSettingsView: View {
    @Environment(\.dismiss) private var dismiss
    @Environment(\.modelContext) private var modelContext
    let user: User?
    @State private var dailyLimit: Double = 1000
    @State private var weeklyLimit: Double = 5000
    @State private var weekendLimit: Double = 8000
    
    var body: some View {
        NavigationView {
            Form {
                Section("Daily Step Limit") {
                    Slider(value: $dailyLimit, in: 100...10000, step: 100)
                    Text("\(Int(dailyLimit)) steps")
                        .font(.caption)
                        .foregroundColor(.secondary)
                }
                
                Section("Weekly Step Limit") {
                    Slider(value: $weeklyLimit, in: 1000...50000, step: 1000)
                    Text("\(Int(weeklyLimit)) steps")
                        .font(.caption)
                        .foregroundColor(.secondary)
                }
                
                Section("Weekend Step Limit") {
                    Slider(value: $weekendLimit, in: 1000...80000, step: 1000)
                    Text("\(Int(weekendLimit)) steps")
                        .font(.caption)
                        .foregroundColor(.secondary)
                }
                
                Section {
                    Text("Changes will take effect tomorrow")
                        .font(.caption)
                        .foregroundColor(.secondary)
                }
            }
            .navigationTitle("Step Settings")
            .navigationBarTitleDisplayMode(.inline)
            .toolbar {
                ToolbarItem(placement: .navigationBarTrailing) {
                    Button("Save") {
                        // TODO: Implement step limit update
                        dismiss()
                    }
                }
                
                ToolbarItem(placement: .navigationBarLeading) {
                    Button("Cancel") {
                        dismiss()
                    }
                }
            }
            .onAppear {
                if let user = user {
                    dailyLimit = Double(user.dailyStepLimit)
                    weeklyLimit = Double(user.weeklyStepLimit)
                    weekendLimit = Double(user.weekendStepLimit)
                }
            }
        }
    }
}

struct SubscriptionView: View {
    @Environment(\.dismiss) private var dismiss
    @Environment(\.modelContext) private var modelContext
    let user: User?
    @State private var selectedPlan: SubscriptionType = .monthly
    @State private var selectedPaymentMethod = PaymentMethod.creditCard
    
    var body: some View {
        NavigationView {
            Form {
                Section("Select Plan") {
                    ForEach(SubscriptionType.allCases, id: \.self) { plan in
                        if plan != .freemium {
                            Button(action: { selectedPlan = plan }) {
                                HStack {
                                    VStack(alignment: .leading) {
                                        Text(plan.rawValue.capitalized)
                                            .font(.headline)
                                        
                                        if let price = planPrice(for: plan) {
                                            Text("\(price, specifier: "%.2f") €/month")
                                                .font(.subheadline)
                                                .foregroundColor(.secondary)
                                        }
                                    }
                                    
                                    Spacer()
                                    
                                    if selectedPlan == plan {
                                        Image(systemName: "checkmark.circle.fill")
                                            .foregroundColor(.blue)
                                    }
                                }
                            }
                        }
                    }
                }
                
                Section("Payment Method") {
                    Picker("Select Payment Method", selection: $selectedPaymentMethod) {
                        ForEach(PaymentMethod.allCases, id: \.self) { method in
                            Text(method.rawValue.capitalized)
                        }
                    }
                }
                
                Section {
                    Button("Subscribe") {
                        // TODO: Implement subscription
                        dismiss()
                    }
                }
            }
            .navigationTitle("Subscription")
            .navigationBarTitleDisplayMode(.inline)
            .toolbar {
                ToolbarItem(placement: .navigationBarTrailing) {
                    Button("Cancel") {
                        dismiss()
                    }
                }
            }
        }
    }
    
    func planPrice(for plan: SubscriptionType) -> Double? {
        switch plan {
        case .monthly:
            return 9.99
        case .quarterly:
            return 24.99
        case .halfYearly:
            return 44.99
        case .yearly:
            return 79.99
        case .freemium:
            return nil
        }
    }
}

#Preview {
    ProfileView()
        .modelContainer(for: [User.self], inMemory: true)
} 