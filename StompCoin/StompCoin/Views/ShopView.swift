import SwiftUI

struct ShopView: View {
    @StateObject private var shopManager = ShopManager.shared
    @State private var selectedCategory: ItemType?
    @State private var showingPurchaseConfirmation = false
    @State private var selectedItem: ShopItem?
    @State private var showingError = false
    @State private var errorMessage = ""
    @State private var searchText = ""
    
    var filteredItems: [ShopItem] {
        var items = shopManager.availableItems
        
        if let category = selectedCategory {
            items = items.filter { $0.type == category }
        }
        
        if !searchText.isEmpty {
            items = items.filter {
                $0.name.localizedCaseInsensitiveContains(searchText) ||
                $0.description.localizedCaseInsensitiveContains(searchText)
            }
        }
        
        return items.sorted { $0.rarity.rawValue > $1.rarity.rawValue }
    }
    
    var body: some View {
        List {
            Section(header: Text("Featured")) {
                ScrollView(.horizontal, showsIndicators: false) {
                    HStack(spacing: 15) {
                        ForEach(shopManager.featuredItems) { item in
                            FeaturedItemView(item: item)
                                .onTapGesture {
                                    selectedItem = item
                                    showingPurchaseConfirmation = true
                                }
                        }
                    }
                    .padding(.horizontal)
                }
                .frame(height: 200)
            }
            
            Section {
                Picker("Kategorie", selection: $selectedCategory) {
                    Text("Alle").tag(Optional<ItemType>.none)
                    ForEach([ItemType.token, .avatar, .theme, .effect, .boost], id: \.self) { type in
                        Text(type.rawValue).tag(Optional(type))
                    }
                }
                .pickerStyle(.segmented)
            }
            
            Section {
                ForEach(filteredItems) { item in
                    ShopItemRow(item: item)
                        .onTapGesture {
                            selectedItem = item
                            showingPurchaseConfirmation = true
                        }
                }
            }
        }
        .searchable(text: $searchText, prompt: "Suche nach Items")
        .navigationTitle("Shop")
        .alert("Item kaufen", isPresented: $showingPurchaseConfirmation) {
            Button("Abbrechen", role: .cancel) { }
            Button("Kaufen") {
                if let item = selectedItem {
                    Task {
                        do {
                            try await shopManager.purchaseItem(item)
                        } catch {
                            errorMessage = error.localizedDescription
                            showingError = true
                        }
                    }
                }
            }
        } message: {
            if let item = selectedItem {
                VStack {
                    Text(item.name)
                        .font(.headline)
                    Text(item.description)
                        .font(.subheadline)
                    Text("Preis: \(item.price, format: .currency(code: "EUR"))")
                        .foregroundColor(.green)
                }
            }
        }
        .alert("Fehler", isPresented: $showingError) {
            Button("OK", role: .cancel) { }
        } message: {
            Text(errorMessage)
        }
    }
}

struct FeaturedItemView: View {
    let item: ShopItem
    
    var body: some View {
        VStack {
            if let imageURL = item.imageURL {
                AsyncImage(url: imageURL) { image in
                    image
                        .resizable()
                        .aspectRatio(contentMode: .fill)
                } placeholder: {
                    ProgressView()
                }
                .frame(width: 150, height: 150)
                .clipShape(RoundedRectangle(cornerRadius: 10))
            }
            
            Text(item.name)
                .font(.headline)
            
            Text(item.rarity.rawValue)
                .font(.caption)
                .foregroundColor(rarityColor)
            
            Text(item.price, format: .currency(code: "EUR"))
                .font(.subheadline)
                .foregroundColor(.green)
        }
        .frame(width: 150)
    }
    
    var rarityColor: Color {
        switch item.rarity {
        case .common:
            return .gray
        case .rare:
            return .blue
        case .epic:
            return .purple
        case .legendary:
            return .orange
        case .exclusive:
            return .red
        }
    }
}

struct ShopItemRow: View {
    let item: ShopItem
    
    var body: some View {
        HStack {
            if let imageURL = item.imageURL {
                AsyncImage(url: imageURL) { image in
                    image
                        .resizable()
                        .aspectRatio(contentMode: .fill)
                } placeholder: {
                    ProgressView()
                }
                .frame(width: 50, height: 50)
                .clipShape(RoundedRectangle(cornerRadius: 5))
            }
            
            VStack(alignment: .leading) {
                Text(item.name)
                    .font(.headline)
                Text(item.description)
                    .font(.caption)
                    .foregroundColor(.secondary)
                Text(item.rarity.rawValue)
                    .font(.caption)
                    .foregroundColor(rarityColor)
            }
            
            Spacer()
            
            VStack(alignment: .trailing) {
                Text(item.price, format: .currency(code: "EUR"))
                    .font(.headline)
                    .foregroundColor(.green)
                
                if item.isPremium {
                    Image(systemName: "star.fill")
                        .foregroundColor(.yellow)
                }
            }
        }
    }
    
    var rarityColor: Color {
        switch item.rarity {
        case .common:
            return .gray
        case .rare:
            return .blue
        case .epic:
            return .purple
        case .legendary:
            return .orange
        case .exclusive:
            return .red
        }
    }
}

#Preview {
    NavigationView {
        ShopView()
    }
} 