import XCTest
@testable import StompCoin

// MARK: - Test Errors

enum TestErrors: Error {
    // MARK: - General Errors
    
    case generalError
    case timeoutError
    case validationError
    case networkError
    case storageError
    
    // MARK: - User Errors
    
    case userNotFound
    case userAlreadyExists
    case invalidUserData
    case userNotAuthorized
    case userNotPremium
    case userNotDeveloper
    
    // MARK: - Challenge Errors
    
    case challengeNotFound
    case challengeAlreadyExists
    case invalidChallengeData
    case challengeNotActive
    case challengeAlreadyCompleted
    case challengeAlreadyCancelled
    case challengeNotStarted
    case challengeAlreadyStarted
    case challengeNotEnded
    case challengeAlreadyEnded
    
    // MARK: - Stream Errors
    
    case streamNotFound
    case streamAlreadyExists
    case invalidStreamData
    case streamNotActive
    case streamAlreadyActive
    case streamNotEnded
    case streamAlreadyEnded
    case streamNotPrivate
    case streamAlreadyPrivate
    case streamNotPublic
    case streamAlreadyPublic
    
    // MARK: - Rewards Errors
    
    case rewardsNotFound
    case rewardsAlreadyExists
    case invalidRewardsData
    case rewardsNotCalculated
    case rewardsAlreadyCalculated
    case rewardsNotDistributed
    case rewardsAlreadyDistributed
    
    // MARK: - Localization Errors
    
    case localizationNotFound
    case localizationAlreadyExists
    case invalidLocalizationData
    case languageNotSupported
    case languageAlreadySet
    
    // MARK: - Camera Errors
    
    case cameraNotFound
    case cameraNotAvailable
    case cameraNotAuthorized
    case cameraAlreadyInUse
    case cameraNotInitialized
    
    // MARK: - Network Errors
    
    case networkNotAvailable
    case networkNotConnected
    case networkNotAuthorized
    case networkNotInitialized
    case networkNotConfigured
    
    // MARK: - Storage Errors
    
    case storageNotAvailable
    case storageNotConnected
    case storageNotAuthorized
    case storageNotInitialized
    case storageNotConfigured
    
    // MARK: - UI Errors
    
    case uiNotAvailable
    case uiNotConnected
    case uiNotAuthorized
    case uiNotInitialized
    case uiNotConfigured
    
    // MARK: - Error Descriptions
    
    var localizedDescription: String {
        switch self {
        case .generalError:
            return "Ein allgemeiner Fehler ist aufgetreten"
        case .timeoutError:
            return "Ein Timeout-Fehler ist aufgetreten"
        case .validationError:
            return "Ein Validierungsfehler ist aufgetreten"
        case .networkError:
            return "Ein Netzwerkfehler ist aufgetreten"
        case .storageError:
            return "Ein Speicherfehler ist aufgetreten"
            
        case .userNotFound:
            return "Benutzer nicht gefunden"
        case .userAlreadyExists:
            return "Benutzer existiert bereits"
        case .invalidUserData:
            return "Ungültige Benutzerdaten"
        case .userNotAuthorized:
            return "Benutzer nicht autorisiert"
        case .userNotPremium:
            return "Benutzer ist kein Premium-Benutzer"
        case .userNotDeveloper:
            return "Benutzer ist kein Entwickler"
            
        case .challengeNotFound:
            return "Challenge nicht gefunden"
        case .challengeAlreadyExists:
            return "Challenge existiert bereits"
        case .invalidChallengeData:
            return "Ungültige Challenge-Daten"
        case .challengeNotActive:
            return "Challenge ist nicht aktiv"
        case .challengeAlreadyCompleted:
            return "Challenge ist bereits abgeschlossen"
        case .challengeAlreadyCancelled:
            return "Challenge ist bereits abgebrochen"
        case .challengeNotStarted:
            return "Challenge wurde nicht gestartet"
        case .challengeAlreadyStarted:
            return "Challenge wurde bereits gestartet"
        case .challengeNotEnded:
            return "Challenge wurde nicht beendet"
        case .challengeAlreadyEnded:
            return "Challenge wurde bereits beendet"
            
        case .streamNotFound:
            return "Stream nicht gefunden"
        case .streamAlreadyExists:
            return "Stream existiert bereits"
        case .invalidStreamData:
            return "Ungültige Stream-Daten"
        case .streamNotActive:
            return "Stream ist nicht aktiv"
        case .streamAlreadyActive:
            return "Stream ist bereits aktiv"
        case .streamNotEnded:
            return "Stream wurde nicht beendet"
        case .streamAlreadyEnded:
            return "Stream wurde bereits beendet"
        case .streamNotPrivate:
            return "Stream ist nicht privat"
        case .streamAlreadyPrivate:
            return "Stream ist bereits privat"
        case .streamNotPublic:
            return "Stream ist nicht öffentlich"
        case .streamAlreadyPublic:
            return "Stream ist bereits öffentlich"
            
        case .rewardsNotFound:
            return "Belohnungen nicht gefunden"
        case .rewardsAlreadyExists:
            return "Belohnungen existieren bereits"
        case .invalidRewardsData:
            return "Ungültige Belohnungsdaten"
        case .rewardsNotCalculated:
            return "Belohnungen wurden nicht berechnet"
        case .rewardsAlreadyCalculated:
            return "Belohnungen wurden bereits berechnet"
        case .rewardsNotDistributed:
            return "Belohnungen wurden nicht verteilt"
        case .rewardsAlreadyDistributed:
            return "Belohnungen wurden bereits verteilt"
            
        case .localizationNotFound:
            return "Lokalisierung nicht gefunden"
        case .localizationAlreadyExists:
            return "Lokalisierung existiert bereits"
        case .invalidLocalizationData:
            return "Ungültige Lokalisierungsdaten"
        case .languageNotSupported:
            return "Sprache wird nicht unterstützt"
        case .languageAlreadySet:
            return "Sprache wurde bereits gesetzt"
            
        case .cameraNotFound:
            return "Kamera nicht gefunden"
        case .cameraNotAvailable:
            return "Kamera nicht verfügbar"
        case .cameraNotAuthorized:
            return "Kamera nicht autorisiert"
        case .cameraAlreadyInUse:
            return "Kamera wird bereits verwendet"
        case .cameraNotInitialized:
            return "Kamera nicht initialisiert"
            
        case .networkNotAvailable:
            return "Netzwerk nicht verfügbar"
        case .networkNotConnected:
            return "Netzwerk nicht verbunden"
        case .networkNotAuthorized:
            return "Netzwerk nicht autorisiert"
        case .networkNotInitialized:
            return "Netzwerk nicht initialisiert"
        case .networkNotConfigured:
            return "Netzwerk nicht konfiguriert"
            
        case .storageNotAvailable:
            return "Speicher nicht verfügbar"
        case .storageNotConnected:
            return "Speicher nicht verbunden"
        case .storageNotAuthorized:
            return "Speicher nicht autorisiert"
        case .storageNotInitialized:
            return "Speicher nicht initialisiert"
        case .storageNotConfigured:
            return "Speicher nicht konfiguriert"
            
        case .uiNotAvailable:
            return "UI nicht verfügbar"
        case .uiNotConnected:
            return "UI nicht verbunden"
        case .uiNotAuthorized:
            return "UI nicht autorisiert"
        case .uiNotInitialized:
            return "UI nicht initialisiert"
        case .uiNotConfigured:
            return "UI nicht konfiguriert"
        }
    }
} 