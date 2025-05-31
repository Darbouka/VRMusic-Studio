import SwiftUI

struct LoginView: View {
    @EnvironmentObject var appState: AppState
    @State private var email = ""
    @State private var password = ""
    @State private var isRegistering = false
    @State private var showError = false
    @State private var errorMessage = ""
    
    var body: some View {
        VStack(spacing: 30) {
            // Logo und Titel
            VStack(spacing: 20) {
                Image("app_logo")
                    .resizable()
                    .aspectRatio(contentMode: .fit)
                    .frame(width: 120, height: 120)
                
                Text("StompCoin")
                    .font(.largeTitle)
                    .bold()
                
                Text("Dein Fitness-Tanz-Erlebnis")
                    .font(.title3)
                    .foregroundColor(.secondary)
            }
            
            // Login-Formular
            VStack(spacing: 20) {
                TextField("E-Mail", text: $email)
                    .textFieldStyle(RoundedBorderTextFieldStyle())
                    .frame(width: 300)
                
                SecureField("Passwort", text: $password)
                    .textFieldStyle(RoundedBorderTextFieldStyle())
                    .frame(width: 300)
                
                Button(action: {
                    Task {
                        do {
                            try await appState.login(email: email, password: password)
                        } catch {
                            errorMessage = error.localizedDescription
                            showError = true
                        }
                    }
                }) {
                    if appState.isLoading {
                        ProgressView()
                            .progressViewStyle(CircularProgressViewStyle())
                    } else {
                        Text("Anmelden")
                            .frame(width: 300)
                    }
                }
                .buttonStyle(.borderedProminent)
                .disabled(appState.isLoading)
                
                Button("Registrieren") {
                    isRegistering = true
                }
                .sheet(isPresented: $isRegistering) {
                    RegisterView()
                }
            }
        }
        .frame(minWidth: 400, minHeight: 500)
        .alert("Fehler", isPresented: $showError) {
            Button("OK", role: .cancel) { }
        } message: {
            Text(errorMessage)
        }
    }
}

struct RegisterView: View {
    @EnvironmentObject var appState: AppState
    @Environment(\.dismiss) var dismiss
    @State private var username = ""
    @State private var email = ""
    @State private var password = ""
    @State private var confirmPassword = ""
    @State private var showError = false
    @State private var errorMessage = ""
    
    var body: some View {
        VStack(spacing: 20) {
            Text("Registrierung")
                .font(.title)
                .bold()
            
            TextField("Benutzername", text: $username)
                .textFieldStyle(RoundedBorderTextFieldStyle())
                .frame(width: 300)
            
            TextField("E-Mail", text: $email)
                .textFieldStyle(RoundedBorderTextFieldStyle())
                .frame(width: 300)
            
            SecureField("Passwort", text: $password)
                .textFieldStyle(RoundedBorderTextFieldStyle())
                .frame(width: 300)
            
            SecureField("Passwort bestätigen", text: $confirmPassword)
                .textFieldStyle(RoundedBorderTextFieldStyle())
                .frame(width: 300)
            
            Button(action: {
                Task {
                    do {
                        if password != confirmPassword {
                            errorMessage = "Die Passwörter stimmen nicht überein"
                            showError = true
                            return
                        }
                        
                        try await appState.register(username: username, email: email, password: password)
                        dismiss()
                    } catch {
                        errorMessage = error.localizedDescription
                        showError = true
                    }
                }
            }) {
                if appState.isLoading {
                    ProgressView()
                        .progressViewStyle(CircularProgressViewStyle())
                } else {
                    Text("Registrieren")
                        .frame(width: 300)
                }
            }
            .buttonStyle(.borderedProminent)
            .disabled(appState.isLoading)
            
            Button("Abbrechen") {
                dismiss()
            }
        }
        .padding()
        .frame(width: 400, height: 500)
        .alert("Fehler", isPresented: $showError) {
            Button("OK", role: .cancel) { }
        } message: {
            Text(errorMessage)
        }
    }
}

struct LoginView_Previews: PreviewProvider {
    static var previews: some View {
        LoginView()
            .environmentObject(AppState())
    }
} 