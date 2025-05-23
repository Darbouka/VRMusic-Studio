import SwiftUI
import SwiftData
import PhotosUI
import CoreLocation
import LocalAuthentication

// Theme-Manager für konsistentes Erscheinungsbild
class ThemeManager: ObservableObject {
    @Published var isDarkMode: Bool = false
    @Published var accentColor: Color = .blue
    @Published var fontSize: FontSize = .medium
    
    enum FontSize: String, CaseIterable {
        case small = "Klein"
        case medium = "Mittel"
        case large = "Groß"
        
        var scale: CGFloat {
            switch self {
            case .small: return 0.9
            case .medium: return 1.0
            case .large: return 1.2
            }
        }
    }
}

// Lokalisierungs-Manager
class LocalizationManager: ObservableObject {
    @Published var currentLocale: Locale = .current
    @Published var currentLanguage: String = "Deutsch"
    
    let availableLanguages = [
        "Deutsch": "de",
        "English": "en",
        "Français": "fr",
        "Español": "es"
    ]
}

// Benachrichtigungs-Manager
class NotificationManager: ObservableObject {
    @Published var isPushEnabled = false
    @Published var eventReminders = true
    @Published var dailySummaries = true
    @Published var challengeNotifications = true
    @Published var tradeNotifications = true
}

struct SettingsView: View {
    @Environment(\.modelContext) private var modelContext
    @Environment(\.dismiss) private var dismiss
    @Environment(\.colorScheme) private var colorScheme
    @Query private var users: [User]
    @StateObject private var themeManager = ThemeManager()
    @State private var showingImagePicker = false
    @State private var selectedImage: PhotosPickerItem?
    @State private var profileImage: Image?
    @State private var showingLogoutAlert = false
    @State private var showingSaveAlert = false
    @State private var showingLocationPermissionAlert = false
    @State private var isUploadingImage = false
    
    // Benutzereinstellungen
    @State private var username: String = ""
    @State private var aboutMe: String = ""
    @State private var fullName: String = ""
    @State private var birthDate: Date = Date()
    @State private var birthPlace: String = ""
    @State private var email: String = ""
    
    // Floor-Einstellungen
    @State private var locationAutomation = false
    @State private var homeFloor = false
    @State private var walkFloor = false
    @State private var fitnessFloor = false
    @State private var eventFloor = false
    @State private var challengeFloor = false
    
    // Neue State-Variablen für Synchronisation
    @State private var isSyncing = false
    @State private var lastSyncDate: Date?
    @State private var showingSyncError = false
    @State private var syncError: String?
    
    @StateObject private var localizationManager = LocalizationManager()
    @StateObject private var notificationManager = NotificationManager()
    @State private var showingBiometricAlert = false
    @State private var isBiometricEnabled = false
    
    private let locationManager = CLLocationManager()
    
    var currentUser: User? {
        users.first
    }
    
    var body: some View {
        NavigationView {
            List {
                // Profil Sektion
                Section("Profil") {
                    if let user = currentUser {
                        HStack {
                            if let profileImage = profileImage {
                                profileImage
                                    .resizable()
                                    .scaledToFill()
                                    .frame(width: 80, height: 80)
                                    .clipShape(Circle())
                            } else {
                                Image(systemName: "person.circle.fill")
                                    .resizable()
                                    .frame(width: 80, height: 80)
                                    .foregroundColor(.gray)
                            }
                            
                            VStack(alignment: .leading) {
                                PhotosPicker(selection: $selectedImage,
                                           matching: .images) {
                                    Text("Profilbild hinzufügen")
                                }
                                
                                if isUploadingImage {
                                    ProgressView()
                                        .progressViewStyle(CircularProgressViewStyle())
                                }
                            }
                        }
                        .onChange(of: selectedImage) { _, newValue in
                            Task {
                                isUploadingImage = true
                                if let data = try? await newValue?.loadTransferable(type: Data.self),
                                   let uiImage = UIImage(data: data) {
                                    profileImage = Image(uiImage: uiImage)
                                    // Hier Bild-Upload-Logik implementieren
                                }
                                isUploadingImage = false
                            }
                        }
                        
                        NavigationLink {
                            TextField("Username", text: $username)
                                .onSubmit {
                                    if isValidUsername(username) {
                                        saveChanges()
                                    }
                                }
                        } label: {
                            HStack {
                                Text("Username")
                                Spacer()
                                Text(username.isEmpty ? user.username : username)
                                    .foregroundColor(.secondary)
                            }
                        }
                        
                        NavigationLink {
                            TextEditor(text: $aboutMe)
                                .frame(height: 200)
                                .onChange(of: aboutMe) { _, _ in
                                    saveChanges()
                                }
                        } label: {
                            HStack {
                                Text("Über mich")
                                Spacer()
                                Text(aboutMe.isEmpty ? "Keine Beschreibung" : aboutMe)
                                    .foregroundColor(.secondary)
                                    .lineLimit(1)
                            }
                        }
                        
                        NavigationLink {
                            TextField("Vor- und Nachname", text: $fullName)
                                .onSubmit {
                                    saveChanges()
                                }
                        } label: {
                            HStack {
                                Text("Vor- und Nachname")
                                Spacer()
                                Text(fullName.isEmpty ? "Nicht angegeben" : fullName)
                                    .foregroundColor(.secondary)
                            }
                        }
                        
                        NavigationLink {
                            DatePicker("Geburtsdatum",
                                     selection: $birthDate,
                                     in: ...Date(),
                                     displayedComponents: .date)
                                .onChange(of: birthDate) { _, _ in
                                    if isAdult(birthDate) {
                                        saveChanges()
                                    }
                                }
                        } label: {
                            HStack {
                                Text("Geburtsdatum")
                                Spacer()
                                Text(birthDate.formatted(date: .long, time: .omitted))
                                    .foregroundColor(.secondary)
                            }
                        }
                        
                        NavigationLink {
                            TextField("Geburtsort", text: $birthPlace)
                                .onSubmit {
                                    saveChanges()
                                }
                        } label: {
                            HStack {
                                Text("Geburtsort")
                                Spacer()
                                Text(birthPlace.isEmpty ? "Nicht angegeben" : birthPlace)
                                    .foregroundColor(.secondary)
                            }
                        }
                        
                        NavigationLink {
                            TextField("Email", text: $email)
                                .keyboardType(.emailAddress)
                                .autocapitalization(.none)
                                .onSubmit {
                                    if isValidEmail(email) {
                                        saveChanges()
                                    }
                                }
                        } label: {
                            HStack {
                                Text("Email")
                                Spacer()
                                Text(email.isEmpty ? user.email : email)
                                    .foregroundColor(.secondary)
                            }
                        }
                    }
                }
                
                // Einstellungen Sektion
                Section("Einstellungen") {
                    NavigationLink("Display") {
                        DisplaySettingsView()
                    }
                    
                    NavigationLink("Daten und Sicherheit") {
                        SecuritySettingsView()
                    }
                    
                    NavigationLink("Profileinstellungen") {
                        ProfileSettingsView()
                    }
                    
                    NavigationLink("Schrittlimit ändern") {
                        StepLimitSettingsView()
                    }
                    
                    Toggle("Standortautomatik", isOn: $locationAutomation)
                        .onChange(of: locationAutomation) { _, newValue in
                            if newValue {
                                checkLocationPermission()
                            }
                        }
                    
                    // Floor-Einstellungen
                    Section("Floor-Einstellungen") {
                        Toggle("Home Floor", isOn: $homeFloor)
                            .disabled(locationAutomation)
                        Toggle("Walk Floor", isOn: $walkFloor)
                            .disabled(locationAutomation)
                        Toggle("Fitness Floor", isOn: $fitnessFloor)
                            .disabled(locationAutomation)
                        Toggle("Event Floor", isOn: $eventFloor)
                            .disabled(locationAutomation)
                        Toggle("Challenge Floor", isOn: $challengeFloor)
                            .disabled(locationAutomation)
                    }
                }
                
                // Trade Market Sektion
                Section {
                    NavigationLink("Trade Market") {
                        TradeMarketView()
                    }
                }
                
                // Rechtliches Sektion
                Section("Rechtliches") {
                    NavigationLink("Privacy Policy") {
                        PrivacyPolicyView()
                    }
                    
                    NavigationLink("AGB und Datenschutzerklärung") {
                        TermsAndConditionsView()
                    }
                    
                    NavigationLink("Über Uns") {
                        AboutUsView()
                    }
                }
                
                // Neue Theming-Sektion
                Section("Erscheinungsbild") {
                    Toggle("Dunkelmodus", isOn: $themeManager.isDarkMode)
                        .onChange(of: themeManager.isDarkMode) { _, newValue in
                            // Hier System-Darkmode anpassen
                        }
                    
                    Picker("Schriftgröße", selection: $themeManager.fontSize) {
                        ForEach(ThemeManager.FontSize.allCases, id: \.self) { size in
                            Text(size.rawValue).tag(size)
                        }
                    }
                    
                    ColorPicker("Akzentfarbe", selection: $themeManager.accentColor)
                }
                
                // Neue Synchronisation-Sektion
                Section("Synchronisation") {
                    HStack {
                        Text("Letzte Synchronisation")
                        Spacer()
                        if let date = lastSyncDate {
                            Text(date.formatted())
                                .foregroundColor(.secondary)
                        } else {
                            Text("Nie")
                                .foregroundColor(.secondary)
                        }
                    }
                    
                    Button(action: syncData) {
                        HStack {
                            if isSyncing {
                                ProgressView()
                                    .progressViewStyle(CircularProgressViewStyle())
                            } else {
                                Image(systemName: "arrow.triangle.2.circlepath")
                            }
                            Text("Jetzt synchronisieren")
                        }
                    }
                    .disabled(isSyncing)
                }
                
                // Neue Barrierefreiheit-Sektion
                Section("Barrierefreiheit") {
                    Toggle("VoiceOver-Unterstützung", isOn: .constant(true))
                    Toggle("Kontrast erhöhen", isOn: .constant(false))
                    Toggle("Bewegung reduzieren", isOn: .constant(false))
                }
                
                // Neue Lokalisierungs-Sektion
                Section("Sprache und Region") {
                    Picker("Sprache", selection: $localizationManager.currentLanguage) {
                        ForEach(Array(localizationManager.availableLanguages.keys), id: \.self) { language in
                            Text(language).tag(language)
                        }
                    }
                    
                    Picker("Region", selection: $localizationManager.currentLocale) {
                        Text("Deutschland").tag(Locale(identifier: "de_DE"))
                        Text("Österreich").tag(Locale(identifier: "de_AT"))
                        Text("Schweiz").tag(Locale(identifier: "de_CH"))
                    }
                }
                
                // Neue Benachrichtigungs-Sektion
                Section("Benachrichtigungen") {
                    Toggle("Push-Benachrichtigungen", isOn: $notificationManager.isPushEnabled)
                    
                    if notificationManager.isPushEnabled {
                        Toggle("Event-Erinnerungen", isOn: $notificationManager.eventReminders)
                        Toggle("Tägliche Zusammenfassungen", isOn: $notificationManager.dailySummaries)
                        Toggle("Challenge-Benachrichtigungen", isOn: $notificationManager.challengeNotifications)
                        Toggle("Trade-Benachrichtigungen", isOn: $notificationManager.tradeNotifications)
                    }
                }
                
                // Neue Sicherheits-Sektion
                Section("Sicherheit") {
                    Toggle("Biometrische Anmeldung", isOn: $isBiometricEnabled)
                        .onChange(of: isBiometricEnabled) { _, newValue in
                            if newValue {
                                authenticateWithBiometrics()
                            }
                        }
                    
                    NavigationLink("Zwei-Faktor-Authentifizierung") {
                        TwoFactorAuthView()
                    }
                    
                    NavigationLink("Datenschutz-Einstellungen") {
                        PrivacySettingsView()
                    }
                    
                    NavigationLink("Sicherheitsprotokoll") {
                        SecurityLogView()
                    }
                }
                
                // Logout Sektion
                Section {
                    Button("Logout") {
                        showingLogoutAlert = true
                    }
                    .foregroundColor(.red)
                }
            }
            .navigationTitle("Einstellungen")
            .alert("Logout", isPresented: $showingLogoutAlert) {
                Button("Abbrechen", role: .cancel) { }
                Button("Logout", role: .destructive) {
                    logout()
                }
            } message: {
                Text("Möchten Sie sich wirklich ausloggen?")
            }
            .alert("Standortberechtigung", isPresented: $showingLocationPermissionAlert) {
                Button("Einstellungen öffnen") {
                    if let url = URL(string: UIApplication.openSettingsURLString) {
                        UIApplication.shared.open(url)
                    }
                }
                Button("Abbrechen", role: .cancel) {
                    locationAutomation = false
                }
            } message: {
                Text("Bitte aktivieren Sie die Standortberechtigung in den Einstellungen, um die Standortautomatik zu nutzen.")
            }
            .alert("Synchronisationsfehler", isPresented: $showingSyncError) {
                Button("OK", role: .cancel) { }
            } message: {
                Text(syncError ?? "Ein unbekannter Fehler ist aufgetreten.")
            }
        }
        .environmentObject(themeManager)
        .preferredColorScheme(themeManager.isDarkMode ? .dark : .light)
        .accentColor(themeManager.accentColor)
        .font(.system(size: 16 * themeManager.fontSize.scale))
        .onAppear {
            loadUserData()
        }
    }
    
    private func loadUserData() {
        guard let user = currentUser else { return }
        username = user.username
        email = user.email
        // Weitere Daten laden...
    }
    
    private func saveChanges() {
        guard let user = currentUser else { return }
        user.username = username
        user.email = email
        // Weitere Änderungen speichern...
        try? modelContext.save()
        showingSaveAlert = true
    }
    
    private func logout() {
        // Logout-Logik implementieren
    }
    
    private func checkLocationPermission() {
        switch locationManager.authorizationStatus {
        case .notDetermined:
            locationManager.requestWhenInUseAuthorization()
        case .restricted, .denied:
            showingLocationPermissionAlert = true
        case .authorizedWhenInUse, .authorizedAlways:
            // Standortautomatik aktivieren
            break
        @unknown default:
            break
        }
    }
    
    private func isValidEmail(_ email: String) -> Bool {
        let emailRegex = "[A-Z0-9a-z._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,64}"
        let emailPredicate = NSPredicate(format:"SELF MATCHES %@", emailRegex)
        return emailPredicate.evaluate(with: email)
    }
    
    private func isValidUsername(_ username: String) -> Bool {
        let usernameRegex = "^[a-zA-Z0-9_]{3,20}$"
        let usernamePredicate = NSPredicate(format:"SELF MATCHES %@", usernameRegex)
        return usernamePredicate.evaluate(with: username)
    }
    
    private func isAdult(_ birthDate: Date) -> Bool {
        let calendar = Calendar.current
        let ageComponents = calendar.dateComponents([.year], from: birthDate, to: Date())
        return (ageComponents.year ?? 0) >= 18
    }
    
    // Neue Funktionen für Synchronisation
    private func syncData() {
        isSyncing = true
        
        // Simuliere Synchronisation
        DispatchQueue.main.asyncAfter(deadline: .now() + 2) {
            do {
                // Hier Synchronisationslogik implementieren
                lastSyncDate = Date()
                isSyncing = false
            } catch {
                syncError = error.localizedDescription
                showingSyncError = true
                isSyncing = false
            }
        }
    }
    
    // Neue Funktionen für Biometrie
    private func authenticateWithBiometrics() {
        let context = LAContext()
        var error: NSError?
        
        if context.canEvaluatePolicy(.deviceOwnerAuthenticationWithBiometrics, error: &error) {
            context.evaluatePolicy(.deviceOwnerAuthenticationWithBiometrics,
                                 localizedReason: "Aktivieren Sie die biometrische Anmeldung") { success, error in
                DispatchQueue.main.async {
                    if success {
                        isBiometricEnabled = true
                    } else {
                        isBiometricEnabled = false
                        showingBiometricAlert = true
                    }
                }
            }
        } else {
            isBiometricEnabled = false
            showingBiometricAlert = true
        }
    }
}

// Erweiterte Platzhalter-Views mit Theming
struct DisplaySettingsView: View {
    @EnvironmentObject private var themeManager: ThemeManager
    
    var body: some View {
        List {
            Section("Anzeige") {
                Toggle("Dunkelmodus", isOn: $themeManager.isDarkMode)
                Picker("Schriftgröße", selection: $themeManager.fontSize) {
                    ForEach(ThemeManager.FontSize.allCases, id: \.self) { size in
                        Text(size.rawValue).tag(size)
                    }
                }
                ColorPicker("Akzentfarbe", selection: $themeManager.accentColor)
            }
        }
        .navigationTitle("Display-Einstellungen")
    }
}

struct SecuritySettingsView: View {
    var body: some View {
        Text("Daten und Sicherheit")
    }
}

struct ProfileSettingsView: View {
    var body: some View {
        Text("Profileinstellungen")
    }
}

struct StepLimitSettingsView: View {
    var body: some View {
        Text("Schrittlimit ändern")
    }
}

struct TradeMarketView: View {
    var body: some View {
        Text("Offizieller Echtzeit Trading Market")
    }
}

struct PrivacyPolicyView: View {
    var body: some View {
        Text("Privacy Policy")
    }
}

struct TermsAndConditionsView: View {
    var body: some View {
        Text("AGB und Datenschutzerklärung")
    }
}

struct AboutUsView: View {
    var body: some View {
        Text("Über Uns")
    }
}

// Neue Views für Sicherheit
struct TwoFactorAuthView: View {
    @State private var isEnabled = false
    @State private var showingQRCode = false
    
    var body: some View {
        List {
            Section {
                Toggle("Zwei-Faktor-Authentifizierung", isOn: $isEnabled)
                
                if isEnabled {
                    Button("QR-Code anzeigen") {
                        showingQRCode = true
                    }
                }
            }
            
            Section("Backup-Codes") {
                Text("Generieren Sie Backup-Codes für den Fall, dass Sie Ihr Gerät verlieren.")
                Button("Backup-Codes generieren") {
                    // Backup-Codes generieren
                }
            }
        }
        .navigationTitle("Zwei-Faktor-Authentifizierung")
        .sheet(isPresented: $showingQRCode) {
            QRCodeView()
        }
    }
}

struct PrivacySettingsView: View {
    @State private var shareAnalytics = true
    @State private var shareLocation = true
    @State private var shareActivity = true
    
    var body: some View {
        List {
            Section("Datenschutz") {
                Toggle("Analytics teilen", isOn: $shareAnalytics)
                Toggle("Standort teilen", isOn: $shareLocation)
                Toggle("Aktivitäten teilen", isOn: $shareActivity)
            }
            
            Section("Datenexport") {
                Button("Daten exportieren") {
                    // Datenexport-Logik
                }
                
                Button("Konto löschen") {
                    // Konto-Lösch-Logik
                }
                .foregroundColor(.red)
            }
        }
        .navigationTitle("Datenschutz")
    }
}

struct SecurityLogView: View {
    var body: some View {
        List {
            Section("Letzte Anmeldungen") {
                // Anmeldungs-Historie
            }
            
            Section("Geräte") {
                // Verbundene Geräte
            }
        }
        .navigationTitle("Sicherheitsprotokoll")
    }
}

struct QRCodeView: View {
    var body: some View {
        VStack {
            // QR-Code anzeigen
            Text("QR-Code für 2FA")
                .font(.title)
            
            Button("Schließen") {
                // Sheet schließen
            }
        }
    }
}

// Accessibility-Modifier für bessere Barrierefreiheit
extension View {
    func accessible() -> some View {
        self
            .accessibilityLabel("Einstellungen")
            .accessibilityHint("Tippen Sie, um die Einstellungen zu öffnen")
    }
}

#Preview {
    SettingsView()
        .modelContainer(for: User.self, inMemory: true)
} 