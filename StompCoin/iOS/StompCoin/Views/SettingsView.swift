import SwiftUI

struct SettingsView: View {
    @EnvironmentObject var appState: AppState
    @State private var showingLogoutAlert = false
    
    var body: some View {
        NavigationView {
            // Sidebar mit Kategorien
            List {
                NavigationLink(destination: AudioSettingsView()) {
                    Label("Audio", systemImage: "speaker.wave.2.fill")
                }
                NavigationLink(destination: NotificationSettingsView()) {
                    Label("Benachrichtigungen", systemImage: "bell.fill")
                }
                NavigationLink(destination: AppearanceSettingsView()) {
                    Label("Erscheinungsbild", systemImage: "paintbrush.fill")
                }
                NavigationLink(destination: LanguageSettingsView()) {
                    Label("Sprache", systemImage: "globe")
                }
                NavigationLink(destination: SyncSettingsView()) {
                    Label("Synchronisierung", systemImage: "arrow.triangle.2.circlepath")
                }
                NavigationLink(destination: DisplaySettingsView()) {
                    Label("Anzeige", systemImage: "display")
                }
            }
            .listStyle(SidebarListStyle())
            .frame(minWidth: 250)
            
            // Standard-Startansicht
            AudioSettingsView()
        }
        .navigationViewStyle(DoubleColumnNavigationViewStyle())
    }
}

struct AudioSettingsView: View {
    @EnvironmentObject var appState: AppState
    
    var body: some View {
        Form {
            Section(header: Text("Musik")) {
                Slider(value: $appState.settings.musicVolume, in: 0...1) {
                    Text("Musiklautstärke")
                }
            }
            
            Section(header: Text("Soundeffekte")) {
                Slider(value: $appState.settings.soundEffectsVolume, in: 0...1) {
                    Text("Effektlautstärke")
                }
            }
        }
        .navigationTitle("Audio")
    }
}

struct NotificationSettingsView: View {
    @EnvironmentObject var appState: AppState
    
    var body: some View {
        Form {
            Section {
                Toggle("Benachrichtigungen aktivieren", isOn: $appState.settings.notificationsEnabled)
            }
            
            if appState.settings.notificationsEnabled {
                Section(header: Text("Benachrichtigungstypen")) {
                    Toggle("Neue Floors", isOn: $appState.settings.floorNotifications)
                    Toggle("Ranglisten-Updates", isOn: $appState.settings.leaderboardNotifications)
                    Toggle("Achievements", isOn: $appState.settings.achievementNotifications)
                }
            }
        }
        .navigationTitle("Benachrichtigungen")
    }
}

struct AppearanceSettingsView: View {
    @EnvironmentObject var appState: AppState
    
    var body: some View {
        Form {
            Section {
                Toggle("Dark Mode", isOn: $appState.settings.darkMode)
            }
            
            Section(header: Text("Akzentfarbe")) {
                ColorPicker("Wähle eine Farbe", selection: $appState.settings.accentColor)
            }
        }
        .navigationTitle("Erscheinungsbild")
    }
}

struct LanguageSettingsView: View {
    @EnvironmentObject var appState: AppState
    
    var body: some View {
        Form {
            Section {
                Picker("Sprache", selection: $appState.settings.language) {
                    Text("Deutsch").tag("de")
                    Text("English").tag("en")
                }
            }
        }
        .navigationTitle("Sprache")
    }
}

struct SyncSettingsView: View {
    @EnvironmentObject var appState: AppState
    
    var body: some View {
        Form {
            Section {
                Toggle("Automatische Synchronisierung", isOn: $appState.settings.autoSync)
            }
            
            Section {
                Button("Jetzt synchronisieren") {
                    // Sync-Logik hier
                }
            }
        }
        .navigationTitle("Synchronisierung")
    }
}

struct DisplaySettingsView: View {
    @EnvironmentObject var appState: AppState
    
    var body: some View {
        Form {
            Section {
                Toggle("Rangliste anzeigen", isOn: $appState.settings.showLeaderboard)
                Toggle("Tutorial anzeigen", isOn: $appState.settings.showTutorial)
            }
            
            Section(header: Text("Daten")) {
                Button("Cache leeren") {
                    // Cache-Lösch-Logik hier
                }
                .foregroundColor(.red)
            }
        }
        .navigationTitle("Anzeige")
    }
} 