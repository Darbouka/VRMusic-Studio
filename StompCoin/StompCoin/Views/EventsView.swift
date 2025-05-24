import SwiftUI
import SwiftData

struct EventsView: View {
    @Environment(\.modelContext) private var modelContext
    @Query private var events: [Event]
    @State private var showingEventDetails = false
    @State private var selectedEvent: Event?
    @State private var searchText = ""
    @State private var showingSuccess = false
    @State private var showingError = false
    @State private var errorMessage = ""
    
    var filteredEvents: [Event] {
        if searchText.isEmpty {
            return events
        }
        return events.filter { event in
            event.name.localizedCaseInsensitiveContains(searchText) ||
            event.location.localizedCaseInsensitiveContains(searchText) ||
            event.organizer.localizedCaseInsensitiveContains(searchText)
        }
    }
    
    var body: some View {
        NavigationView {
            List {
                ForEach(filteredEvents) { event in
                    EventListItem(event: event)
                        .onTapGesture {
                            selectedEvent = event
                            showingEventDetails = true
                        }
                }
            }
            .searchable(text: $searchText, prompt: "Search events")
            .navigationTitle("Events")
            .sheet(isPresented: $showingEventDetails) {
                if let event = selectedEvent {
                    EventDetailView(event: event)
                }
            }
        }
    }
    
    private func activateBoost() {
        guard let user = currentUser else { return }
        
        let success = EventDetectionManager.shared.activateBoost(.double)
        
        if success {
            showingSuccess = true
            // Event-Boost-Effekte anwenden
            if let event = currentEvent {
                event.rewardMultiplier = 2.0
                event.boostEndTime = Date().addingTimeInterval(3600) // 1 Stunde
            }
        } else {
            showingError = true
            errorMessage = "Boost konnte nicht aktiviert werden"
        }
    }
}

struct EventListItem: View {
    let event: Event
    
    var body: some View {
        VStack(alignment: .leading, spacing: 8) {
            HStack {
                Text(event.name)
                    .font(.headline)
                Spacer()
                if event.isActive {
                    Text("Active")
                        .font(.caption)
                        .padding(.horizontal, 8)
                        .padding(.vertical, 4)
                        .background(Color.green)
                        .foregroundColor(.white)
                        .cornerRadius(4)
                }
            }
            
            Text(event.location)
                .font(.subheadline)
                .foregroundColor(.secondary)
            
            HStack {
                Text(event.startDate, style: .date)
                Text("-")
                Text(event.endDate, style: .date)
            }
            .font(.caption)
            .foregroundColor(.secondary)
            
            if !event.musicGenres.isEmpty {
                ScrollView(.horizontal, showsIndicators: false) {
                    HStack {
                        ForEach(event.musicGenres, id: \.self) { genre in
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
        .padding(.vertical, 8)
    }
}

struct EventDetailView: View {
    @Environment(\.modelContext) private var modelContext
    @Environment(\.dismiss) private var dismiss
    let event: Event
    @State private var showingBoostConfirmation = false
    
    var body: some View {
        NavigationView {
            ScrollView {
                VStack(alignment: .leading, spacing: 20) {
                    // Event Header
                    VStack(alignment: .leading, spacing: 8) {
                        Text(event.name)
                            .font(.title)
                            .bold()
                        
                        Text(event.location)
                            .font(.headline)
                        
                        HStack {
                            Text(event.startDate, style: .date)
                            Text("-")
                            Text(event.endDate, style: .date)
                        }
                        .font(.subheadline)
                        .foregroundColor(.secondary)
                    }
                    
                    // Description
                    Text(event.description)
                        .font(.body)
                    
                    // Music Genres
                    VStack(alignment: .leading, spacing: 8) {
                        Text("Music Genres")
                            .font(.headline)
                        
                        FlowLayout(spacing: 8) {
                            ForEach(event.musicGenres, id: \.self) { genre in
                                Text(genre.rawValue)
                                    .font(.caption)
                                    .padding(.horizontal, 8)
                                    .padding(.vertical, 4)
                                    .background(Color.blue.opacity(0.1))
                                    .cornerRadius(4)
                            }
                        }
                    }
                    
                    // Boost Information
                    VStack(alignment: .leading, spacing: 8) {
                        Text("Boost Information")
                            .font(.headline)
                        
                        HStack {
                            Text("Multiplier:")
                            Text("\(event.boostMultiplier, specifier: "%.1f")x")
                                .bold()
                        }
                        
                        if event.isActive {
                            Button(action: { showingBoostConfirmation = true }) {
                                Text("Activate Boost")
                                    .bold()
                                    .foregroundColor(.white)
                                    .frame(maxWidth: .infinity)
                                    .padding()
                                    .background(Color.blue)
                                    .cornerRadius(8)
                            }
                        }
                    }
                    
                    // Challenges
                    if !event.challenges.isEmpty {
                        VStack(alignment: .leading, spacing: 8) {
                            Text("Challenges")
                                .font(.headline)
                            
                            ForEach(event.challenges) { challenge in
                                ChallengeCard(challenge: challenge)
                            }
                        }
                    }
                    
                    // VIP Tickets
                    if event.vipTicketsAvailable > 0 {
                        VStack(alignment: .leading, spacing: 8) {
                            Text("VIP Tickets")
                                .font(.headline)
                            
                            Text("Available: \(event.vipTicketsAvailable)")
                            Text("Price: \(event.vipTicketPrice, specifier: "%.2f") StompCoins")
                        }
                    }
                }
                .padding()
            }
            .navigationBarTitleDisplayMode(.inline)
            .toolbar {
                ToolbarItem(placement: .navigationBarTrailing) {
                    Button("Close") {
                        dismiss()
                    }
                }
            }
            .alert("Activate Boost", isPresented: $showingBoostConfirmation) {
                Button("Cancel", role: .cancel) { }
                Button("Activate") {
                    activateBoost()
                }
            } message: {
                Text("This will activate the boost for this event. The boost will last until the event ends.")
            }
        }
    }
}

struct ChallengeCard: View {
    let challenge: Challenge
    
    var body: some View {
        VStack(alignment: .leading, spacing: 8) {
            Text(challenge.name)
                .font(.headline)
            
            Text(challenge.description)
                .font(.subheadline)
                .foregroundColor(.secondary)
            
            HStack {
                Text("Reward:")
                Text("\(Int(challenge.reward)) StompCoins")
                    .bold()
            }
            .font(.caption)
            
            if challenge.isLive {
                Text("LIVE")
                    .font(.caption)
                    .padding(.horizontal, 8)
                    .padding(.vertical, 4)
                    .background(Color.red)
                    .foregroundColor(.white)
                    .cornerRadius(4)
            }
        }
        .padding()
        .background(Color(.systemBackground))
        .cornerRadius(8)
        .shadow(radius: 2)
    }
}

struct FlowLayout: Layout {
    var spacing: CGFloat = 8
    
    func sizeThatFits(proposal: ProposedViewSize, subviews: Subviews, cache: inout ()) -> CGSize {
        let sizes = subviews.map { $0.sizeThatFits(.unspecified) }
        return layout(sizes: sizes, proposal: proposal).size
    }
    
    func placeSubviews(in bounds: CGRect, proposal: ProposedViewSize, subviews: Subviews, cache: inout ()) {
        let sizes = subviews.map { $0.sizeThatFits(.unspecified) }
        let positions = layout(sizes: sizes, proposal: proposal).positions
        
        for (index, subview) in subviews.enumerated() {
            subview.place(at: positions[index], proposal: .unspecified)
        }
    }
    
    private func layout(sizes: [CGSize], proposal: ProposedViewSize) -> (positions: [CGPoint], size: CGSize) {
        guard let width = proposal.width else { return ([], .zero) }
        
        var positions: [CGPoint] = []
        var currentX: CGFloat = 0
        var currentY: CGFloat = 0
        var maxY: CGFloat = 0
        var rowHeight: CGFloat = 0
        
        for size in sizes {
            if currentX + size.width > width {
                currentX = 0
                currentY += rowHeight + spacing
                rowHeight = 0
            }
            
            positions.append(CGPoint(x: currentX, y: currentY))
            currentX += size.width + spacing
            rowHeight = max(rowHeight, size.height)
            maxY = max(maxY, currentY + size.height)
        }
        
        return (positions, CGSize(width: width, height: maxY))
    }
}

#Preview {
    EventsView()
        .modelContainer(for: [Event.self, Challenge.self], inMemory: true)
} 