import Foundation
import SwiftData

enum MusicGenreCategory: String, Codable {
    case goaFloor = "Goa Floor"
    case technoFloor = "Techno Floor"
}

enum MusicGenre: String, Codable {
    // Goa Floor
    case psytrance
    case nightPsy
    case fullOn
    case forest
    case darkPsy
    case hightech
    case progressive
    case proggyPsy
    case progg
    case zenonesque
    
    // Techno Floor
    case minimalTechno
    case maximalTechno
    case acidTechno
    case berlinTechno
    case hardTechno
    case deepTechno
    case houseTechno
    case raveTechno
    case downbeatTechno
    
    var category: MusicGenreCategory {
        switch self {
        case .psytrance, .nightPsy, .fullOn, .forest, .darkPsy,
             .hightech, .progressive, .proggyPsy, .progg, .zenonesque:
            return .goaFloor
        case .minimalTechno, .maximalTechno, .acidTechno, .berlinTechno,
             .hardTechno, .deepTechno, .houseTechno, .raveTechno, .downbeatTechno:
            return .technoFloor
        }
    }
    
    var displayName: String {
        switch self {
        // Goa Floor
        case .psytrance: return "Psytrance"
        case .nightPsy: return "Night Psy"
        case .fullOn: return "Full On"
        case .forest: return "Forest"
        case .darkPsy: return "Dark Psy"
        case .hightech: return "Hightech"
        case .progressive: return "Progressive"
        case .proggyPsy: return "Proggy Psy"
        case .progg: return "Progg"
        case .zenonesque: return "Zenonesque"
        
        // Techno Floor
        case .minimalTechno: return "Minimal Techno"
        case .maximalTechno: return "Maximal Techno"
        case .acidTechno: return "Acid Techno"
        case .berlinTechno: return "Berlin Techno"
        case .hardTechno: return "Hard Techno"
        case .deepTechno: return "Deep Techno"
        case .houseTechno: return "House Techno"
        case .raveTechno: return "Rave Techno"
        case .downbeatTechno: return "Downbeat Techno"
        }
    }
    
    var icon: String {
        switch self {
        // Goa Floor
        case .psytrance: return "waveform.path"
        case .nightPsy: return "moon.stars"
        case .fullOn: return "flame"
        case .forest: return "leaf"
        case .darkPsy: return "cloud.bolt"
        case .hightech: return "cpu"
        case .progressive: return "chart.line.uptrend.xyaxis"
        case .proggyPsy: return "waveform.path.ecg"
        case .progg: return "waveform"
        case .zenonesque: return "sparkles"
        
        // Techno Floor
        case .minimalTechno: return "minus.circle"
        case .maximalTechno: return "plus.circle"
        case .acidTechno: return "drop"
        case .berlinTechno: return "building.2"
        case .hardTechno: return "bolt"
        case .deepTechno: return "arrow.down.circle"
        case .houseTechno: return "house"
        case .raveTechno: return "party.popper"
        case .downbeatTechno: return "arrow.down.to.line"
        }
    }
    
    var description: String {
        switch self {
        // Goa Floor
        case .psytrance:
            return "Psytrance ist ein elektronisches Musikgenre, das sich durch hypnotische Rhythmen und psychedelische Klänge auszeichnet."
        case .nightPsy:
            return "Night Psy ist eine dunklere, intensivere Variante des Psytrance mit mystischen Elementen."
        case .fullOn:
            return "Full On ist ein energiegeladener Psytrance-Stil mit schnellen Beats und komplexen Melodien."
        case .forest:
            return "Forest Psytrance verbindet organische Klänge mit psychedelischen Elementen."
        case .darkPsy:
            return "Dark Psy ist eine düstere, experimentelle Form des Psytrance."
        case .hightech:
            return "Hightech ist ein futuristischer Psytrance-Stil mit komplexen Rhythmen."
        case .progressive:
            return "Progressive Psytrance zeichnet sich durch sich entwickelnde Strukturen aus."
        case .proggyPsy:
            return "Proggy Psy kombiniert progressive Elemente mit Psytrance."
        case .progg:
            return "Progg ist eine melodischere Variante des Psytrance."
        case .zenonesque:
            return "Zenonesque ist ein meditativer, atmosphärischer Psytrance-Stil."
            
        // Techno Floor
        case .minimalTechno:
            return "Minimal Techno ist ein reduzierter, subtiler Techno-Stil."
        case .maximalTechno:
            return "Maximal Techno ist ein intensiver, kraftvoller Techno-Stil."
        case .acidTechno:
            return "Acid Techno ist bekannt für seine charakteristischen 303-Basslines."
        case .berlinTechno:
            return "Berlin Techno ist ein rauer, industrieller Techno-Stil."
        case .hardTechno:
            return "Hard Techno ist ein aggressiver, energischer Techno-Stil."
        case .deepTechno:
            return "Deep Techno zeichnet sich durch atmosphärische, tiefe Klänge aus."
        case .houseTechno:
            return "House Techno verbindet House- und Techno-Elemente."
        case .raveTechno:
            return "Rave Techno ist ein energiegeladener, partyorientierter Stil."
        case .downbeatTechno:
            return "Downbeat Techno ist ein entspannter, experimenteller Techno-Stil."
        }
    }
}

@Model
final class GenreRanking {
    var id: UUID
    var genre: MusicGenre
    var totalParticipants: Int
    var totalStompCoins: Double
    var internationalRankings: [UserRanking]
    var nationalRankings: [UserRanking]
    var localRankings: [UserRanking]
    var lastUpdated: Date
    
    init(genre: MusicGenre) {
        self.id = UUID()
        self.genre = genre
        self.totalParticipants = 0
        self.totalStompCoins = 0
        self.internationalRankings = []
        self.nationalRankings = []
        self.localRankings = []
        self.lastUpdated = Date()
    }
}

@Model
final class UserRanking {
    var id: UUID
    var user: User
    var genre: MusicGenre
    var stompCoins: Double
    var challengesWon: Int
    var rank: Int
    var lastUpdated: Date
    
    init(user: User, genre: MusicGenre) {
        self.id = UUID()
        self.user = user
        self.genre = genre
        self.stompCoins = 0
        self.challengesWon = 0
        self.rank = 0
        self.lastUpdated = Date()
    }
}

@Model
class MusicGenre {
    var name: String
    var icon: String // SF Symbol Name
    var description: String
    var popularity: Int
    var activeUsers: Int
    var totalSteps: Int
    var createdAt: Date
    
    init(name: String, icon: String, description: String) {
        self.name = name
        self.icon = icon
        self.description = description
        self.popularity = 0
        self.activeUsers = 0
        self.totalSteps = 0
        self.createdAt = Date()
    }
} 