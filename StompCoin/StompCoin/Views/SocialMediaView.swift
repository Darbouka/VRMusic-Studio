import SwiftUI

struct SocialMediaView: View {
    @StateObject private var socialMediaService = SocialMediaService.shared
    @State private var selectedPlatform: SocialMediaService.Platform = .instagram
    @State private var isStreaming = false
    @State private var showingShareSheet = false
    @State private var selectedChallenge: ChallengeService.Challenge?
    
    var body: some View {
        NavigationView {
            List {
                Section(header: Text("Live Streaming")) {
                    if isStreaming {
                        VStack(alignment: .leading, spacing: 10) {
                            Text("Aktiver Stream")
                                .font(.headline)
                            
                            if let stream = socialMediaService.getUserStream(UserManager.shared.currentUser?.id ?? UUID()) {
                                Text("Plattform: \(stream.platform.rawValue)")
                                Text("Startzeit: \(stream.startTime.formatted())")
                                Text("Zuschauer: \(stream.viewerCount)")
                                
                                Button(action: {
                                    Task {
                                        try? await socialMediaService.endLiveStream()
                                        isStreaming = false
                                    }
                                }) {
                                    Text("Stream beenden")
                                        .foregroundColor(.red)
                                }
                            }
                        }
                        .padding()
                    } else {
                        VStack(alignment: .leading, spacing: 10) {
                            Text("Neuer Stream")
                                .font(.headline)
                            
                            Picker("Plattform", selection: $selectedPlatform) {
                                ForEach(SocialMediaService.Platform.allCases, id: \.self) { platform in
                                    Text(platform.rawValue).tag(platform)
                                }
                            }
                            
                            Button(action: {
                                if let challenge = selectedChallenge {
                                    Task {
                                        try? await socialMediaService.startLiveStream(for: challenge, platform: selectedPlatform)
                                        isStreaming = true
                                    }
                                }
                            }) {
                                Text("Stream starten")
                                    .foregroundColor(.blue)
                            }
                            .disabled(selectedChallenge == nil)
                        }
                        .padding()
                    }
                }
                
                Section(header: Text("Aktive Streams")) {
                    ForEach(socialMediaService.getActiveStreams()) { stream in
                        VStack(alignment: .leading, spacing: 5) {
                            Text("Plattform: \(stream.platform.rawValue)")
                            Text("Startzeit: \(stream.startTime.formatted())")
                            Text("Zuschauer: \(stream.viewerCount)")
                        }
                        .padding(.vertical, 5)
                    }
                }
                
                Section(header: Text("Challenge teilen")) {
                    Button(action: {
                        showingShareSheet = true
                    }) {
                        Text("Challenge auf Social Media teilen")
                    }
                }
            }
            .navigationTitle("Social Media")
            .sheet(isPresented: $showingShareSheet) {
                if let challenge = selectedChallenge {
                    ShareSheet(activityItems: [
                        "🎉 Neue Challenge: \(challenge.title)\n\n\(challenge.description)\n\n#StompCoin #Challenge #\(challenge.type.rawValue)"
                    ])
                }
            }
        }
    }
}

struct ShareSheet: UIViewControllerRepresentable {
    let activityItems: [Any]
    
    func makeUIViewController(context: Context) -> UIActivityViewController {
        let controller = UIActivityViewController(
            activityItems: activityItems,
            applicationActivities: nil
        )
        return controller
    }
    
    func updateUIViewController(_ uiViewController: UIActivityViewController, context: Context) {}
}

struct SocialMediaView_Previews: PreviewProvider {
    static var previews: some View {
        SocialMediaView()
    }
} 