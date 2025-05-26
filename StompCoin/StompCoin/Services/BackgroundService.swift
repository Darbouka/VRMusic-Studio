import Foundation
import SwiftUI

class BackgroundService: ObservableObject {
    static let shared = BackgroundService()
    
    enum Season: String {
        case summer = "Sommer"
        case winter = "Winter"
    }
    
    enum MusicGenre: String {
        case techno = "Techno"
        case goa = "Goa"
        case psytrance = "Psytrance"
        case house = "House"
        case drumAndBass = "Drum & Bass"
    }
    
    enum TechnoStyle: String {
        case industrial = "Industrial"
        case acid = "Acid"
        case minimal = "Minimal"
        case hardcore = "Hardcore"
        case ambient = "Ambient"
    }
    
    @Published var currentSeason: Season
    @Published var currentBackground: BackgroundImage
    @Published var lastUpdateDate: Date
    
    private let userDefaults = UserDefaults.standard
    private let lastUpdateKey = "lastBackgroundUpdate"
    
    struct BackgroundImage: Codable {
        let id: UUID
        let imageURL: URL
        let title: String
        let description: String
        let season: Season?
        let genre: MusicGenre?
        let technoStyle: TechnoStyle?
        let isEvent: Bool
        let eventDate: Date?
    }
    
    init() {
        // Aktuelle Jahreszeit bestimmen
        let month = Calendar.current.component(.month, from: Date())
        currentSeason = (month >= 6 && month <= 8) ? .summer : .winter
        
        // Letztes Update laden
        if let dateData = userDefaults.data(forKey: lastUpdateKey),
           let date = try? JSONDecoder().decode(Date.self, from: dateData) {
            lastUpdateDate = date
        } else {
            lastUpdateDate = Date()
        }
        
        // Standard-Hintergrund setzen
        currentBackground = BackgroundImage(
            id: UUID(),
            imageURL: URL(string: "https://stompcoin.com/backgrounds/default.jpg")!,
            title: "Standard Hintergrund",
            description: "Standard Hintergrund",
            season: nil,
            genre: nil,
            technoStyle: nil,
            isEvent: false,
            eventDate: nil
        )
        
        // Monatliche Aktualisierung starten
        startMonthlyUpdate()
    }
    
    private func startMonthlyUpdate() {
        // Timer für monatliche Aktualisierung
        Timer.scheduledTimer(withTimeInterval: 30 * 24 * 60 * 60, repeats: true) { [weak self] _ in
            self?.updateBackgrounds()
        }
    }
    
    func updateBackgrounds() {
        // Hier die tatsächliche Aktualisierungslogik implementieren
        // Beispiel:
        Task {
            do {
                try await fetchNewBackgrounds()
                await MainActor.run {
                    lastUpdateDate = Date()
                    if let dateData = try? JSONEncoder().encode(lastUpdateDate) {
                        userDefaults.set(dateData, forKey: lastUpdateKey)
                    }
                }
            } catch {
                print("Fehler beim Aktualisieren der Hintergründe: \(error)")
            }
        }
    }
    
    private func fetchNewBackgrounds() async throws {
        // Hier die tatsächliche API-Integration implementieren
        // Beispiel-Implementierung:
        let backgrounds = [
            // Sommer Goa Festival
            BackgroundImage(
                id: UUID(),
                imageURL: URL(string: "https://stompcoin.com/backgrounds/summer_goa.jpg")!,
                title: "Sommer Goa Festival",
                description: "Outdoor Goa Festival mit tanzenden Menschen",
                season: .summer,
                genre: .goa,
                technoStyle: nil,
                isEvent: true,
                eventDate: Date()
            ),
            // Winter Goa Party
            BackgroundImage(
                id: UUID(),
                imageURL: URL(string: "https://stompcoin.com/backgrounds/winter_goa.jpg")!,
                title: "Winter Goa Party",
                description: "Indoor Goa Party mit tanzenden Menschen",
                season: .winter,
                genre: .goa,
                technoStyle: nil,
                isEvent: true,
                eventDate: Date()
            )
        ]
        
        // Hintergrund basierend auf aktueller Jahreszeit setzen
        if let background = backgrounds.first(where: { $0.season == currentSeason }) {
            await MainActor.run {
                currentBackground = background
            }
        }
    }
    
    func getBackgroundForGenre(_ genre: MusicGenre) -> BackgroundImage {
        // Hier die tatsächliche Logik implementieren
        // Beispiel:
        return BackgroundImage(
            id: UUID(),
            imageURL: URL(string: "https://stompcoin.com/backgrounds/\(genre.rawValue.lowercased()).jpg")!,
            title: "\(genre.rawValue) Event",
            description: "\(genre.rawValue) Festival/Party Hintergrund",
            season: nil,
            genre: genre,
            technoStyle: nil,
            isEvent: true,
            eventDate: Date()
        )
    }
    
    func getBackgroundForTechnoStyle(_ style: TechnoStyle) -> BackgroundImage {
        // Hier die tatsächliche Logik implementieren
        // Beispiel:
        return BackgroundImage(
            id: UUID(),
            imageURL: URL(string: "https://stompcoin.com/backgrounds/techno_\(style.rawValue.lowercased()).jpg")!,
            title: "\(style.rawValue) Techno Event",
            description: "\(style.rawValue) Techno Festival/Party Hintergrund",
            season: nil,
            genre: .techno,
            technoStyle: style,
            isEvent: true,
            eventDate: Date()
        )
    }
    
    func getTradeMarketingBackground() -> BackgroundImage {
        // Psychedelic Goa Style Trading Hintergrund
        return BackgroundImage(
            id: UUID(),
            imageURL: URL(string: "https://stompcoin.com/backgrounds/trade_marketing.jpg")!,
            title: "Trade Marketing Floor",
            description: "Psychedelic Goa Style Trading Interface",
            season: nil,
            genre: nil,
            technoStyle: nil,
            isEvent: false,
            eventDate: nil
        )
    }
    
    func getTokenItemBackground() -> BackgroundImage {
        // Psychedelic Goa Style Token Interface
        return BackgroundImage(
            id: UUID(),
            imageURL: URL(string: "https://stompcoin.com/backgrounds/token_items.jpg")!,
            title: "Token Items",
            description: "Psychedelic Goa Style Token Interface",
            season: nil,
            genre: nil,
            technoStyle: nil,
            isEvent: false,
            eventDate: nil
        )
    }
} 