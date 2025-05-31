import SwiftUI

struct SettingsView: View {
    @EnvironmentObject var appState: AppState
    @State private var showLogoutAlert = false
    @State private var username = ""
    @State private var email = ""
    @State private var pushNotifications = false
    @State private var emailNotifications = false
    
    var body: some View {
        NavigationView {
            Form {
                Section(header: Text("Profil")) {
                    TextField("Benutzername", text: $username)
                    TextField("E-Mail", text: $email)
                }
                
                Section(header: Text("Benachrichtigungen")) {
                    Toggle("Push-Benachrichtigungen", isOn: $pushNotifications)
                    Toggle("E-Mail-Benachrichtigungen", isOn: $emailNotifications)
                }
                
                Section {
                    Button("Speichern") {
                        appState.settings.pushNotifications = pushNotifications
                        appState.settings.emailNotifications = emailNotifications
                    }
                }
                
                Section {
                    Button("Abmelden") {
                        showLogoutAlert = true
                    }
                    .foregroundColor(.red)
                }
            }
            .navigationTitle("Einstellungen")
            .alert("Abmelden", isPresented: $showLogoutAlert) {
                Button("Abbrechen", role: .cancel) { }
                Button("Abmelden", role: .destructive) {
                    appState.logout()
                }
            } message: {
                Text("Möchten Sie sich wirklich abmelden?")
            }
            .onAppear {
                username = appState.currentUser?.username ?? ""
                email = appState.currentUser?.email ?? ""
                pushNotifications = appState.settings.pushNotifications
                emailNotifications = appState.settings.emailNotifications
            }
        }
    }
}

struct SettingsView_Previews: PreviewProvider {
    static var previews: some View {
        SettingsView()
            .environmentObject(AppState())
    }
} 