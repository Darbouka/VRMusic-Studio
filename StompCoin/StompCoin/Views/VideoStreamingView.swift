import SwiftUI
import AVFoundation

struct VideoStreamingView: View {
    @StateObject private var videoStreamingService = VideoStreamingService.shared
    @State private var isStreaming = false
    @State private var isPrivate = false
    @State private var selectedStreams: Set<UUID> = []
    @State private var showingViewerRequest = false
    @State private var pendingViewerId: UUID?
    @State private var showingRewards = false
    @State private var showingOMCoinProgress = false
    
    let challenge: ChallengeService.Challenge
    
    var body: some View {
        VStack {
            if isStreaming {
                // Eigenes Video anzeigen
                if let previewLayer = videoStreamingService.previewLayer {
                    VideoPreviewView(previewLayer: previewLayer)
                        .frame(height: 300)
                        .cornerRadius(12)
                }
                
                // Stream-Controls
                HStack {
                    Button(action: {
                        Task {
                            try? await videoStreamingService.switchCamera()
                        }
                    }) {
                        Image(systemName: "camera.rotate")
                            .font(.title2)
                            .foregroundColor(.blue)
                            .padding()
                            .background(Color.blue.opacity(0.1))
                            .cornerRadius(8)
                    }
                    
                    Button(action: {
                        Task {
                            try? await videoStreamingService.endVideoStream()
                            isStreaming = false
                        }
                    }) {
                        Text("Stream beenden")
                            .foregroundColor(.red)
                            .padding()
                            .background(Color.red.opacity(0.1))
                            .cornerRadius(8)
                    }
                    
                    if isPrivate {
                        Button(action: {
                            showingViewerRequest = true
                        }) {
                            Text("Zuschauer verwalten")
                                .foregroundColor(.blue)
                                .padding()
                                .background(Color.blue.opacity(0.1))
                                .cornerRadius(8)
                        }
                    }
                    
                    Button(action: {
                        showingRewards = true
                    }) {
                        Image(systemName: "star.fill")
                            .font(.title2)
                            .foregroundColor(.yellow)
                            .padding()
                            .background(Color.yellow.opacity(0.1))
                            .cornerRadius(8)
                    }
                }
                .padding()
                
                // Belohnungs-Statistiken
                if let stream = videoStreamingService.getUserStream(UserManager.shared.currentUser?.id ?? UUID()) {
                    VStack(spacing: 10) {
                        HStack {
                            StatisticView(title: "Stomps", value: "\(stream.totalStomps)")
                            StatisticView(title: "StompCoins", value: "\(stream.earnedStompCoins)")
                            StatisticView(title: "OM Coins", value: "\(stream.earnedOMCoins)")
                        }
                        
                        HStack {
                            StatisticView(title: "Zuschauer", value: "\(stream.viewerCount)")
                            StatisticView(title: "Streamzeit", value: stream.startTime.formatted())
                        }
                        
                        // OM Coin Fortschritt für Freemium-User
                        if let user = UserManager.shared.currentUser,
                           !user.isVIPPremium && !user.isDeveloperVIPPremium {
                            let progress = videoStreamingService.getOMCoinProgress(for: user.id)
                            
                            VStack(spacing: 5) {
                                ProgressView(value: progress.progress)
                                    .progressViewStyle(LinearProgressViewStyle())
                                    .tint(.blue)
                                
                                if let timeRemaining = progress.timeRemaining {
                                    Text("Zeit verbleibend: \(formatTimeRemaining(timeRemaining))")
                                        .font(.caption)
                                        .foregroundColor(.gray)
                                }
                                
                                Text("\(Int(progress.progress * 100))% zu nächstem OM Coin")
                                    .font(.caption)
                                    .foregroundColor(.gray)
                            }
                            .padding()
                            .background(Color.gray.opacity(0.1))
                            .cornerRadius(8)
                        }
                    }
                    .padding()
                    .background(Color.gray.opacity(0.1))
                    .cornerRadius(12)
                }
            } else {
                // Stream-Start-Optionen
                VStack(spacing: 20) {
                    Toggle("Privater Stream", isOn: $isPrivate)
                        .padding()
                    
                    Button(action: {
                        Task {
                            try? await videoStreamingService.startVideoStream(for: challenge, isPrivate: isPrivate)
                            isStreaming = true
                        }
                    }) {
                        Text("Stream starten")
                            .foregroundColor(.blue)
                            .padding()
                            .frame(maxWidth: .infinity)
                            .background(Color.blue.opacity(0.1))
                            .cornerRadius(8)
                    }
                }
                .padding()
            }
            
            // Aktive Streams
            if let user = UserManager.shared.currentUser,
               (user.isVIPPremium || user.isDeveloperVIPPremium) {
                ScrollView(.horizontal, showsIndicators: false) {
                    HStack(spacing: 15) {
                        ForEach(videoStreamingService.getViewableStreams()) { stream in
                            if stream.userId != user.id {
                                VideoStreamCard(stream: stream)
                                    .frame(width: 200)
                                    .onTapGesture {
                                        if selectedStreams.contains(stream.id) {
                                            selectedStreams.remove(stream.id)
                                        } else if selectedStreams.count < 3 {
                                            selectedStreams.insert(stream.id)
                                        }
                                    }
                                    .overlay(
                                        RoundedRectangle(cornerRadius: 12)
                                            .stroke(selectedStreams.contains(stream.id) ? Color.blue : Color.clear, lineWidth: 2)
                                    )
                            }
                        }
                    }
                    .padding()
                }
            } else {
                Text("Premium erforderlich für Live-Übertragungen")
                    .foregroundColor(.gray)
                    .padding()
            }
        }
        .navigationTitle("Live Streaming")
        .sheet(isPresented: $showingViewerRequest) {
            ViewerRequestView(streamId: videoStreamingService.getUserStream(UserManager.shared.currentUser?.id ?? UUID())?.id ?? UUID())
        }
        .sheet(isPresented: $showingRewards) {
            RewardsView(challenge: challenge)
        }
    }
    
    private func formatTimeRemaining(_ timeInterval: TimeInterval) -> String {
        let hours = Int(timeInterval) / 3600
        let minutes = (Int(timeInterval) % 3600) / 60
        return String(format: "%02d:%02d", hours, minutes)
    }
}

struct VideoPreviewView: UIViewRepresentable {
    let previewLayer: AVCaptureVideoPreviewLayer
    
    func makeUIView(context: Context) -> UIView {
        let view = UIView()
        previewLayer.frame = view.bounds
        view.layer.addSublayer(previewLayer)
        return view
    }
    
    func updateUIView(_ uiView: UIView, context: Context) {
        previewLayer.frame = uiView.bounds
    }
}

struct VideoStreamCard: View {
    let stream: VideoStreamingService.VideoStream
    
    var body: some View {
        VStack {
            // Hier Video-Player einbinden
            Rectangle()
                .fill(Color.gray.opacity(0.3))
                .frame(height: 150)
                .overlay(
                    Text("Live Stream")
                        .foregroundColor(.white)
                )
            
            VStack(alignment: .leading, spacing: 5) {
                Text("Streamer: \(stream.userId)")
                    .font(.caption)
                Text("Zuschauer: \(stream.viewerCount)")
                    .font(.caption)
                Text("Start: \(stream.startTime.formatted())")
                    .font(.caption)
            }
            .padding(.horizontal, 8)
            .padding(.vertical, 4)
        }
        .background(Color.white)
        .cornerRadius(12)
        .shadow(radius: 5)
    }
}

struct ViewerRequestView: View {
    @StateObject private var videoStreamingService = VideoStreamingService.shared
    @Environment(\.presentationMode) var presentationMode
    let streamId: UUID
    
    var body: some View {
        NavigationView {
            List {
                // Hier Viewer-Anfragen anzeigen
                Text("Keine ausstehenden Anfragen")
                    .foregroundColor(.gray)
            }
            .navigationTitle("Zuschauer verwalten")
            .navigationBarItems(trailing: Button("Fertig") {
                presentationMode.wrappedValue.dismiss()
            })
        }
    }
}

struct StatisticView: View {
    let title: String
    let value: String
    
    var body: some View {
        VStack(spacing: 5) {
            Text(title)
                .font(.caption)
                .foregroundColor(.gray)
            Text(value)
                .font(.headline)
        }
        .frame(maxWidth: .infinity)
    }
}

struct RewardsView: View {
    @StateObject private var videoStreamingService = VideoStreamingService.shared
    @Environment(\.presentationMode) var presentationMode
    let challenge: ChallengeService.Challenge
    
    var body: some View {
        NavigationView {
            List {
                Section(header: Text("Top Streamer")) {
                    if let topStreamer = videoStreamingService.getTopStreamer(for: challenge) {
                        VStack(alignment: .leading, spacing: 10) {
                            Text("Meiste Stomps: \(topStreamer.totalStomps)")
                            Text("Verdiente StompCoins: \(topStreamer.earnedStompCoins)")
                            Text("Verdiente OM Coins: \(topStreamer.earnedOMCoins)")
                        }
                    }
                }
                
                Section(header: Text("Meiste Zuschauer")) {
                    if let mostViewed = videoStreamingService.getMostViewedStreamer(for: challenge) {
                        VStack(alignment: .leading, spacing: 10) {
                            Text("Zuschauer: \(mostViewed.viewerCount)")
                            if mostViewed.userId == videoStreamingService.getTopStreamer(for: challenge)?.userId {
                                Text("+1 OM Coin für die meisten Zuschauer!")
                                    .foregroundColor(.green)
                            }
                        }
                    }
                }
            }
            .navigationTitle("Belohnungen")
            .navigationBarItems(trailing: Button("Fertig") {
                presentationMode.wrappedValue.dismiss()
            })
        }
    }
}

struct VideoStreamingView_Previews: PreviewProvider {
    static var previews: some View {
        VideoStreamingView(challenge: ChallengeService.Challenge(
            id: UUID(),
            title: "Test Challenge",
            description: "Test Description",
            eventId: UUID(),
            startDate: Date(),
            endDate: Date().addingTimeInterval(3600),
            requiredTokens: 100,
            requiredStompCoins: 200,
            participants: [],
            confirmedParticipants: [],
            status: .active,
            type: .event
        ))
    }
} 