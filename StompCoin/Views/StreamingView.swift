import SwiftUI
import AVFoundation

struct StreamingView: View {
    @StateObject private var viewModel = StreamingViewModel()
    @EnvironmentObject private var appManager: AppManager
    
    var body: some View {
        NavigationView {
            VStack {
                if viewModel.isStreaming {
                    // Aktiver Stream
                    StreamView(viewModel: viewModel)
                } else {
                    // Stream-Start-Bildschirm
                    StreamStartView(viewModel: viewModel)
                }
            }
            .navigationTitle("Streaming")
            .alert("Fehler", isPresented: $viewModel.showError) {
                Button("OK", role: .cancel) {}
            } message: {
                Text(viewModel.errorMessage)
            }
        }
    }
}

struct StreamView: View {
    @ObservedObject var viewModel: StreamingViewModel
    
    var body: some View {
        VStack {
            // Video-Vorschau
            CameraPreviewView(session: viewModel.captureSession)
                .aspectRatio(9/16, contentMode: .fit)
                .cornerRadius(12)
            
            // Stream-Statistiken
            StreamStatsView(viewModel: viewModel)
            
            // Stream-Steuerung
            StreamControlsView(viewModel: viewModel)
        }
        .padding()
    }
}

struct CameraPreviewView: UIViewRepresentable {
    let session: AVCaptureSession
    
    func makeUIView(context: Context) -> UIView {
        let view = UIView(frame: UIScreen.main.bounds)
        let previewLayer = AVCaptureVideoPreviewLayer(session: session)
        previewLayer.frame = view.frame
        previewLayer.videoGravity = .resizeAspectFill
        view.layer.addSublayer(previewLayer)
        return view
    }
    
    func updateUIView(_ uiView: UIView, context: Context) {}
}

struct StreamStatsView: View {
    @ObservedObject var viewModel: StreamingViewModel
    
    var body: some View {
        HStack(spacing: 20) {
            StatView(title: "Stomps", value: "\(viewModel.totalStomps)")
            StatView(title: "Zuschauer", value: "\(viewModel.viewerCount)")
            StatView(title: "Dauer", value: viewModel.streamDuration)
        }
        .padding()
        .background(Color(.systemBackground))
        .cornerRadius(12)
        .shadow(radius: 2)
    }
}

struct StreamControlsView: View {
    @ObservedObject var viewModel: StreamingViewModel
    
    var body: some View {
        HStack(spacing: 30) {
            Button(action: viewModel.toggleCamera) {
                Image(systemName: "camera.rotate.fill")
                    .font(.title)
            }
            
            Button(action: viewModel.toggleMicrophone) {
                Image(systemName: viewModel.isMicrophoneEnabled ? "mic.fill" : "mic.slash.fill")
                    .font(.title)
            }
            
            Button(action: viewModel.togglePrivateMode) {
                Image(systemName: viewModel.isPrivateMode ? "lock.fill" : "lock.open.fill")
                    .font(.title)
            }
            
            Button(action: viewModel.endStream) {
                Image(systemName: "stop.circle.fill")
                    .font(.title)
                    .foregroundColor(.red)
            }
        }
        .padding()
    }
}

struct StreamStartView: View {
    @ObservedObject var viewModel: StreamingViewModel
    
    var body: some View {
        VStack(spacing: 20) {
            // Stream-Einstellungen
            Form {
                Section(header: Text("Stream-Einstellungen")) {
                    Toggle("Privater Stream", isOn: $viewModel.isPrivateMode)
                    Toggle("Mikrofon aktivieren", isOn: $viewModel.isMicrophoneEnabled)
                }
                
                Section(header: Text("Challenge auswählen")) {
                    Picker("Challenge", selection: $viewModel.selectedChallenge) {
                        ForEach(viewModel.availableChallenges) { challenge in
                            Text(challenge.title).tag(challenge as Challenge?)
                        }
                    }
                }
            }
            
            // Start-Button
            Button(action: viewModel.startStream) {
                HStack {
                    Image(systemName: "video.fill")
                    Text("Stream starten")
                }
                .font(.headline)
                .foregroundColor(.white)
                .padding()
                .frame(maxWidth: .infinity)
                .background(Color.blue)
                .cornerRadius(12)
            }
            .padding()
            .disabled(viewModel.selectedChallenge == nil)
        }
    }
}

#Preview {
    StreamingView()
        .environmentObject(AppManager.shared)
} 