import SwiftUI
import SwiftData

struct BoostShopView: View {
    @Environment(\.modelContext) private var modelContext
    @Query private var boostPacks: [BoostPack]
    @Query private var tokenPacks: [TokenPack]
    @Query private var userBoosts: [UserBoost]
    @State private var selectedTab = 0
    @State private var showingPurchaseConfirmation = false
    @State private var selectedPack: Any?
    
    var body: some View {
        NavigationView {
            VStack(spacing: 0) {
                // Tab-Auswahl
                Picker("Shop", selection: $selectedTab) {
                    Text("Boosts").tag(0)
                    Text("Token Packs").tag(1)
                }
                .pickerStyle(SegmentedPickerStyle())
                .padding()
                
                // Hauptinhalt
                TabView(selection: $selectedTab) {
                    // Boost-Packs
                    ScrollView {
                        VStack(spacing: 20) {
                            // Aktive Boosts
                            if !activeBoosts.isEmpty {
                                ActiveBoostsView(boosts: activeBoosts)
                            }
                            
                            // Verfügbare Boost-Packs
                            LazyVGrid(columns: [
                                GridItem(.flexible()),
                                GridItem(.flexible())
                            ], spacing: 20) {
                                ForEach(boostPacks) { pack in
                                    BoostPackCard(pack: pack) {
                                        selectedPack = pack
                                        showingPurchaseConfirmation = true
                                    }
                                }
                            }
                            .padding()
                        }
                    }
                    .tag(0)
                    
                    // Token-Packs
                    ScrollView {
                        LazyVGrid(columns: [
                            GridItem(.flexible()),
                            GridItem(.flexible())
                        ], spacing: 20) {
                            ForEach(tokenPacks) { pack in
                                TokenPackCard(pack: pack) {
                                    selectedPack = pack
                                    showingPurchaseConfirmation = true
                                }
                            }
                        }
                        .padding()
                    }
                    .tag(1)
                }
                .tabViewStyle(PageTabViewStyle(indexDisplayMode: .never))
            }
            .navigationTitle("Boost Shop")
            .sheet(isPresented: $showingPurchaseConfirmation) {
                if let boostPack = selectedPack as? BoostPack {
                    PurchaseConfirmationView(
                        title: boostPack.type.rawValue,
                        description: boostPack.type.description,
                        price: boostPack.price
                    ) {
                        purchaseBoostPack(boostPack)
                    }
                } else if let tokenPack = selectedPack as? TokenPack {
                    PurchaseConfirmationView(
                        title: tokenPack.name,
                        description: tokenPack.description,
                        price: tokenPack.price
                    ) {
                        purchaseTokenPack(tokenPack)
                    }
                }
            }
        }
    }
    
    private var activeBoosts: [UserBoost] {
        userBoosts.filter { $0.isActive && !$0.isExpired }
    }
    
    private func purchaseBoostPack(_ pack: BoostPack) {
        // Hier kommt die Logik für den Kauf eines Boost-Packs
        let userBoost = UserBoost(user: User.current, type: pack.type)
        modelContext.insert(userBoost)
    }
    
    private func purchaseTokenPack(_ pack: TokenPack) {
        // Hier kommt die Logik für den Kauf eines Token-Packs
    }
}

struct ActiveBoostsView: View {
    let boosts: [UserBoost]
    
    var body: some View {
        VStack(alignment: .leading, spacing: 10) {
            Text("Aktive Boosts")
                .font(.headline)
                .padding(.horizontal)
            
            ScrollView(.horizontal, showsIndicators: false) {
                HStack(spacing: 15) {
                    ForEach(boosts) { boost in
                        ActiveBoostCard(boost: boost)
                    }
                }
                .padding(.horizontal)
            }
        }
    }
}

struct ActiveBoostCard: View {
    let boost: UserBoost
    
    var body: some View {
        VStack {
            Text(boost.type.rawValue)
                .font(.headline)
            
            Text(timeRemaining)
                .font(.caption)
                .foregroundColor(.secondary)
            
            ProgressView(value: progress)
                .tint(.blue)
        }
        .padding()
        .background(Color.secondary.opacity(0.1))
        .cornerRadius(10)
    }
    
    private var timeRemaining: String {
        let minutes = Int(boost.remainingTime / 60)
        let seconds = Int(boost.remainingTime.truncatingRemainder(dividingBy: 60))
        return String(format: "%02d:%02d", minutes, seconds)
    }
    
    private var progress: Double {
        1 - (boost.remainingTime / boost.type.duration)
    }
}

struct BoostPackCard: View {
    let pack: BoostPack
    let action: () -> Void
    
    var body: some View {
        Button(action: action) {
            VStack {
                Text(pack.type.rawValue)
                    .font(.headline)
                
                Text(pack.type.description)
                    .font(.caption)
                    .foregroundColor(.secondary)
                    .multilineTextAlignment(.center)
                
                Text("\(Int(pack.price)) Coins")
                    .font(.subheadline)
                    .foregroundColor(.green)
                    .padding(.top, 5)
            }
            .padding()
            .frame(maxWidth: .infinity)
            .background(Color.secondary.opacity(0.1))
            .cornerRadius(15)
        }
    }
}

struct TokenPackCard: View {
    let pack: TokenPack
    let action: () -> Void
    
    var body: some View {
        Button(action: action) {
            VStack {
                Text(pack.name)
                    .font(.headline)
                
                Text(pack.description)
                    .font(.caption)
                    .foregroundColor(.secondary)
                    .multilineTextAlignment(.center)
                
                HStack {
                    Text("\(pack.tokens) Tokens")
                        .font(.subheadline)
                    
                    if pack.bonus > 0 {
                        Text("+\(pack.bonus) Bonus")
                            .font(.caption)
                            .foregroundColor(.green)
                    }
                }
                
                Text("\(Int(pack.price)) Coins")
                    .font(.subheadline)
                    .foregroundColor(.green)
                    .padding(.top, 5)
            }
            .padding()
            .frame(maxWidth: .infinity)
            .background(Color.secondary.opacity(0.1))
            .cornerRadius(15)
        }
    }
}

struct PurchaseConfirmationView: View {
    let title: String
    let description: String
    let price: Double
    let action: () -> Void
    @Environment(\.dismiss) private var dismiss
    
    var body: some View {
        NavigationView {
            VStack(spacing: 20) {
                Text(title)
                    .font(.title)
                    .fontWeight(.bold)
                
                Text(description)
                    .foregroundColor(.secondary)
                    .multilineTextAlignment(.center)
                    .padding()
                
                Text("\(Int(price)) Coins")
                    .font(.title2)
                    .foregroundColor(.green)
                
                Button(action: {
                    action()
                    dismiss()
                }) {
                    Text("Kaufen")
                        .font(.headline)
                        .foregroundColor(.white)
                        .frame(maxWidth: .infinity)
                        .padding()
                        .background(Color.blue)
                        .cornerRadius(10)
                }
                .padding()
            }
            .padding()
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
    BoostShopView()
} 