import SwiftUI
import Foundation

struct SettingsView: View {
    @EnvironmentObject var appState: AppState
    
    var body: some View {
        Form {
            Section(header: Text("Audio")) {
                HStack {
                    Text("Musik-Lautstärke")
                    Slider(value: $appState.settings.musicVolume, in: 0...1)
                }
                
                HStack {
                    Text("Soundeffekte")
                    Slider(value: $appState.settings.soundEffectsVolume, in: 0...1)
                }
            }
            
            Section(header: Text("Benachrichtigungen")) {
                Toggle("Benachrichtigungen aktivieren", isOn: $appState.settings.notificationsEnabled)
            }
            
            Section(header: Text("Erscheinungsbild")) {
                Toggle("Dunkles Design", isOn: $appState.settings.darkMode)
            }
            
            Section(header: Text("Sprache")) {
                Picker("Sprache", selection: $appState.settings.language) {
                    Text("Deutsch").tag("Deutsch")
                    Text("English").tag("English")
                }
            }
            
            Section(header: Text("Synchronisierung")) {
                Toggle("Automatische Synchronisierung", isOn: $appState.settings.autoSync)
            }
            
            Section(header: Text("Anzeige")) {
                Toggle("Rangliste anzeigen", isOn: $appState.settings.showLeaderboard)
                Toggle("Tutorial anzeigen", isOn: $appState.settings.showTutorial)
            }
        }
        .onChange(of: appState.settings) { _ in
            appState.saveSettings()
        }
    }
} 