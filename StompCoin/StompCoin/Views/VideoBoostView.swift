import SwiftUI
import AVKit

struct VideoBoostView: View {
    @Environment(\.dismiss) private var dismiss
    @Environment(\.modelContext) private var modelContext
    let user: User?
    @State private var isVideoPlaying = false
    @State private var showingReward = false
    @State private var boostMultiplier = 2.0
    @State private var showingSuccess = false
    @State private var showingError = false
    @State private var errorMessage = ""
    @State private var videoPlayer: AVPlayer?
    
    var body: some View {
        NavigationView {
            VStack {
                if isVideoPlaying {
                    // Placeholder for video player
                    Color.black
                        .overlay(
                            Image(systemName: "play.fill")
                                .font(.largeTitle)
                                .foregroundColor(.white)
                        )
                        .frame(height: 200)
                        .onTapGesture {
                            // Simulate video completion
                            isVideoPlaying = false
                            showingReward = true
                        }
                } else {
                    VStack(spacing: 20) {
                        Image(systemName: "video.fill")
                            .font(.system(size: 60))
                            .foregroundColor(.blue)
                        
                        Text("Watch a video to boost your steps!")
                            .font(.title2)
                            .multilineTextAlignment(.center)
                        
                        Text("You have \(user?.videoBoostsRemaining ?? 0) boosts remaining today")
                            .font(.subheadline)
                            .foregroundColor(.secondary)
                        
                        Button(action: { isVideoPlaying = true }) {
                            Text("Watch Video")
                                .bold()
                                .foregroundColor(.white)
                                .frame(maxWidth: .infinity)
                                .padding()
                                .background(Color.blue)
                                .cornerRadius(8)
                        }
                        .disabled(user?.videoBoostsRemaining == 0)
                    }
                    .padding()
                }
            }
            .navigationTitle("Video Boost")
            .navigationBarTitleDisplayMode(.inline)
            .toolbar {
                ToolbarItem(placement: .navigationBarTrailing) {
                    Button("Close") {
                        dismiss()
                    }
                }
            }
            .alert("Boost Activated!", isPresented: $showingReward) {
                Button("OK") {
                    activateBoost()
                }
            } message: {
                Text("Your steps will be multiplied by \(boostMultiplier)x for the next hour!")
            }
            .alert("Boost erfolgreich aktiviert!", isPresented: $showingSuccess) {
                Button("OK") {
                    // Video abspielen
                    if let videoURL = URL(string: "https://example.com/ad.mp4") {
                        videoPlayer = AVPlayer(url: videoURL)
                        videoPlayer?.play()
                    }
                }
            } message: {
                Text("Dein Schrittzähler wird um \(boostMultiplier)x erhöht für die nächste Stunde!")
            }
            .alert("Boost konnte nicht aktiviert werden", isPresented: $showingError) {
                Button("OK") {
                    // TODO: Handle error
                }
            } message: {
                Text(errorMessage)
            }
        }
    }
    
    private func activateBoost() {
        guard let user = user else { return }
        
        let success = AppManager.shared.activateVideoBoost(
            user: user,
            modelContext: modelContext
        )
        
        if success {
            showingSuccess = true
            // Video abspielen
            if let videoURL = URL(string: "https://example.com/ad.mp4") {
                videoPlayer = AVPlayer(url: videoURL)
                videoPlayer?.play()
            }
        } else {
            showingError = true
            errorMessage = "Boost konnte nicht aktiviert werden"
        }
    }
}

#Preview {
    VideoBoostView(user: nil)
} 