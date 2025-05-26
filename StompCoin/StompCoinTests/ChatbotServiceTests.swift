import XCTest
@testable import StompCoin

class ChatbotServiceTests: XCTestCase {
    var chatbotService: ChatbotService!
    var userManager: UserManager!
    
    override func setUp() {
        super.setUp()
        chatbotService = ChatbotService.shared
        userManager = UserManager.shared
    }
    
    override func tearDown() {
        chatbotService = nil
        userManager = nil
        super.tearDown()
    }
    
    func testSendMessage() async throws {
        // Benutzer erstellen
        let user = UserManager.User(
            id: UUID(),
            username: "Test User",
            email: "test@example.com",
            level: 1,
            tokens: 100,
            stompCoins: 100,
            isVIPPremium: false,
            isDeveloper: false,
            vipPremiumGrantedBy: nil,
            vipPremiumGrantDate: nil
        )
        
        // Benutzer als aktuellen Benutzer setzen
        await MainActor.run {
            userManager.currentUser = user
        }
        
        // Nachricht senden
        try await chatbotService.sendMessage("Hallo")
        
        // Prüfen ob Nachrichten gespeichert wurden
        XCTAssertEqual(chatbotService.messages.count, 2) // Benutzer-Nachricht + Bot-Antwort
        XCTAssertTrue(chatbotService.messages[0].isFromUser)
        XCTAssertFalse(chatbotService.messages[1].isFromUser)
    }
    
    func testFAQManagement() {
        // FAQ hinzufügen
        chatbotService.addFAQ(
            question: "Test Frage",
            answer: "Test Antwort",
            category: .general
        )
        
        // Prüfen ob FAQ gespeichert wurde
        let faqs = chatbotService.getFAQs()
        XCTAssertEqual(faqs.count, 4) // 3 Standard-FAQs + 1 neue FAQ
        
        // FAQ nach Kategorie filtern
        let generalFAQs = chatbotService.getFAQs(for: .general)
        XCTAssertEqual(generalFAQs.count, 2) // 1 Standard-FAQ + 1 neue FAQ
        
        // FAQ suchen
        let searchResults = chatbotService.searchFAQs(query: "Test")
        XCTAssertEqual(searchResults.count, 1)
    }
    
    func testChatbotResponses() async throws {
        // Benutzer erstellen
        let user = UserManager.User(
            id: UUID(),
            username: "Test User",
            email: "test@example.com",
            level: 1,
            tokens: 100,
            stompCoins: 100,
            isVIPPremium: false,
            isDeveloper: false,
            vipPremiumGrantedBy: nil,
            vipPremiumGrantDate: nil
        )
        
        // Benutzer als aktuellen Benutzer setzen
        await MainActor.run {
            userManager.currentUser = user
        }
        
        // Verschiedene Nachrichten testen
        try await chatbotService.sendMessage("Hallo")
        XCTAssertTrue(chatbotService.messages.last?.content.contains("Hallo") ?? false)
        
        try await chatbotService.sendMessage("Wie funktioniert VIP Premium?")
        XCTAssertTrue(chatbotService.messages.last?.content.contains("VIP Premium") ?? false)
        
        try await chatbotService.sendMessage("Was sind Challenges?")
        XCTAssertTrue(chatbotService.messages.last?.content.contains("Challenges") ?? false)
    }
    
    func testMessagePersistence() async throws {
        // Benutzer erstellen
        let user = UserManager.User(
            id: UUID(),
            username: "Test User",
            email: "test@example.com",
            level: 1,
            tokens: 100,
            stompCoins: 100,
            isVIPPremium: false,
            isDeveloper: false,
            vipPremiumGrantedBy: nil,
            vipPremiumGrantDate: nil
        )
        
        // Benutzer als aktuellen Benutzer setzen
        await MainActor.run {
            userManager.currentUser = user
        }
        
        // Mehrere Nachrichten senden
        try await chatbotService.sendMessage("Hallo")
        try await chatbotService.sendMessage("Wie funktioniert VIP Premium?")
        try await chatbotService.sendMessage("Was sind Challenges?")
        
        // Neuen ChatbotService erstellen
        let newChatbotService = ChatbotService.shared
        
        // Prüfen ob Nachrichten persistent sind
        XCTAssertEqual(newChatbotService.messages.count, 6) // 3 Benutzer-Nachrichten + 3 Bot-Antworten
    }
    
    func testFAQCategories() {
        // FAQs für verschiedene Kategorien hinzufügen
        chatbotService.addFAQ(
            question: "Allgemeine Frage",
            answer: "Allgemeine Antwort",
            category: .general
        )
        
        chatbotService.addFAQ(
            question: "Konto Frage",
            answer: "Konto Antwort",
            category: .account
        )
        
        chatbotService.addFAQ(
            question: "Challenge Frage",
            answer: "Challenge Antwort",
            category: .challenges
        )
        
        chatbotService.addFAQ(
            question: "Zahlung Frage",
            answer: "Zahlung Antwort",
            category: .payments
        )
        
        chatbotService.addFAQ(
            question: "VIP Frage",
            answer: "VIP Antwort",
            category: .vip
        )
        
        // Prüfen ob FAQs korrekt kategorisiert sind
        XCTAssertEqual(chatbotService.getFAQs(for: .general).count, 2)
        XCTAssertEqual(chatbotService.getFAQs(for: .account).count, 1)
        XCTAssertEqual(chatbotService.getFAQs(for: .challenges).count, 2)
        XCTAssertEqual(chatbotService.getFAQs(for: .payments).count, 1)
        XCTAssertEqual(chatbotService.getFAQs(for: .vip).count, 2)
    }
    
    func testComplexChatbotResponses() async throws {
        // Benutzer erstellen
        let user = UserManager.User(
            id: UUID(),
            username: "Test User",
            email: "test@example.com",
            level: 1,
            tokens: 100,
            stompCoins: 100,
            isVIPPremium: false,
            isDeveloper: false,
            vipPremiumGrantedBy: nil,
            vipPremiumGrantDate: nil
        )
        
        // Benutzer als aktuellen Benutzer setzen
        await MainActor.run {
            userManager.currentUser = user
        }
        
        // Komplexe Nachrichten testen
        try await chatbotService.sendMessage("Wie kann ich VIP Premium werden?")
        XCTAssertTrue(chatbotService.messages.last?.content.contains("VIP Premium") ?? false)
        
        try await chatbotService.sendMessage("Was kostet die Teilnahme an einer Challenge?")
        XCTAssertTrue(chatbotService.messages.last?.content.contains("200 StompCoins") ?? false)
        
        try await chatbotService.sendMessage("Wie funktioniert das mit den Entwicklern?")
        XCTAssertTrue(chatbotService.messages.last?.content.contains("Entwickler") ?? false)
        
        try await chatbotService.sendMessage("Unbekannte Frage")
        XCTAssertTrue(chatbotService.messages.last?.content.contains("FAQ-Bereich") ?? false)
    }
    
    func testFAQSearch() {
        // Verschiedene FAQs hinzufügen
        chatbotService.addFAQ(
            question: "Wie funktioniert StompCoin?",
            answer: "StompCoin ist eine innovative App für Events und Challenges.",
            category: .general
        )
        
        chatbotService.addFAQ(
            question: "Was sind die Vorteile von VIP Premium?",
            answer: "VIP Premium bietet kostenlose Teilnahme an allen Events.",
            category: .vip
        )
        
        // Suchtests durchführen
        let stompCoinResults = chatbotService.searchFAQs(query: "StompCoin")
        XCTAssertEqual(stompCoinResults.count, 1)
        
        let vipResults = chatbotService.searchFAQs(query: "VIP")
        XCTAssertEqual(vipResults.count, 2)
        
        let eventResults = chatbotService.searchFAQs(query: "Event")
        XCTAssertEqual(eventResults.count, 1)
        
        let noResults = chatbotService.searchFAQs(query: "NichtVorhanden")
        XCTAssertEqual(noResults.count, 0)
    }
} 