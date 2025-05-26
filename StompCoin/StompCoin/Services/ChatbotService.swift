import Foundation

class ChatbotService: ObservableObject {
    static let shared = ChatbotService()
    
    struct Message: Identifiable, Codable {
        let id: UUID
        let userId: UUID
        let content: String
        let timestamp: Date
        let isFromUser: Bool
    }
    
    struct FAQ: Identifiable, Codable {
        let id: UUID
        let question: String
        let answer: String
        let category: FAQCategory
    }
    
    enum FAQCategory: String, Codable {
        case general = "Allgemein"
        case account = "Konto"
        case challenges = "Challenges"
        case payments = "Zahlungen"
        case vip = "VIP Premium"
    }
    
    @Published private(set) var messages: [Message] = []
    @Published private(set) var faqs: [FAQ] = []
    
    private let userDefaults = UserDefaults.standard
    private let messagesKey = "chatbotMessages"
    private let faqsKey = "faqs"
    
    init() {
        loadMessages()
        loadFAQs()
    }
    
    private func loadMessages() {
        if let data = userDefaults.data(forKey: messagesKey),
           let loadedMessages = try? JSONDecoder().decode([Message].self, from: data) {
            messages = loadedMessages
        }
    }
    
    private func loadFAQs() {
        if let data = userDefaults.data(forKey: faqsKey),
           let loadedFAQs = try? JSONDecoder().decode([FAQ].self, from: data) {
            faqs = loadedFAQs
        } else {
            // Standard-FAQs laden
            faqs = [
                FAQ(
                    id: UUID(),
                    question: "Was ist StompCoin?",
                    answer: "StompCoin ist eine innovative App für Event-Challenges und Social Media Aktivitäten.",
                    category: .general
                ),
                FAQ(
                    id: UUID(),
                    question: "Wie funktioniert VIP Premium?",
                    answer: "VIP Premium bietet exklusive Vorteile wie kostenlose Teilnahme an Challenges und Events.",
                    category: .vip
                ),
                FAQ(
                    id: UUID(),
                    question: "Wie kann ich an Challenges teilnehmen?",
                    answer: "Sie können an Challenges teilnehmen, indem Sie die erforderlichen Token und StompCoins haben.",
                    category: .challenges
                )
            ]
            saveFAQs()
        }
    }
    
    private func saveMessages() {
        if let data = try? JSONEncoder().encode(messages) {
            userDefaults.set(data, forKey: messagesKey)
        }
    }
    
    private func saveFAQs() {
        if let data = try? JSONEncoder().encode(faqs) {
            userDefaults.set(data, forKey: faqsKey)
        }
    }
    
    func sendMessage(_ content: String) async throws {
        guard let userId = UserManager.shared.currentUser?.id else {
            throw NSError(domain: "ChatbotService", code: 1, userInfo: [NSLocalizedDescriptionKey: "Benutzer nicht angemeldet"])
        }
        
        // Benutzer-Nachricht speichern
        let userMessage = Message(
            id: UUID(),
            userId: userId,
            content: content,
            timestamp: Date(),
            isFromUser: true
        )
        
        await MainActor.run {
            messages.append(userMessage)
            saveMessages()
        }
        
        // Chatbot-Antwort generieren
        let response = try await generateResponse(to: content)
        
        // Chatbot-Antwort speichern
        let botMessage = Message(
            id: UUID(),
            userId: userId,
            content: response,
            timestamp: Date(),
            isFromUser: false
        )
        
        await MainActor.run {
            messages.append(botMessage)
            saveMessages()
        }
    }
    
    private func generateResponse(to message: String) async throws -> String {
        // Hier die tatsächliche KI-Integration implementieren
        // Beispiel:
        let lowercasedMessage = message.lowercased()
        
        if lowercasedMessage.contains("hallo") || lowercasedMessage.contains("hi") {
            return "Hallo! Wie kann ich Ihnen helfen?"
        } else if lowercasedMessage.contains("vip") || lowercasedMessage.contains("premium") {
            return "VIP Premium bietet exklusive Vorteile wie kostenlose Teilnahme an Challenges und Events. Entwickler und von ihnen bestätigte Benutzer erhalten lebenslangen VIP Premium Status."
        } else if lowercasedMessage.contains("challenge") {
            return "Challenges sind Events, an denen Sie teilnehmen können. Die Teilnahmegebühr beträgt 200 StompCoins, die an die Entwickler gehen."
        } else {
            return "Entschuldigung, ich verstehe Ihre Frage nicht. Bitte versuchen Sie es anders zu formulieren oder schauen Sie in unseren FAQ-Bereich."
        }
    }
    
    func addFAQ(question: String, answer: String, category: FAQCategory) {
        let faq = FAQ(
            id: UUID(),
            question: question,
            answer: answer,
            category: category
        )
        
        faqs.append(faq)
        saveFAQs()
    }
    
    func getFAQs(for category: FAQCategory? = nil) -> [FAQ] {
        if let category = category {
            return faqs.filter { $0.category == category }
        }
        return faqs
    }
    
    func searchFAQs(query: String) -> [FAQ] {
        let lowercasedQuery = query.lowercased()
        return faqs.filter {
            $0.question.lowercased().contains(lowercasedQuery) ||
            $0.answer.lowercased().contains(lowercasedQuery)
        }
    }
} 