import Foundation
import SwiftData

class SocialManager {
    static let shared = SocialManager()
    
    private init() {}
    
    func sendFriendRequest(from user: User, to targetUser: User) async throws {
        let request = FriendRequest(
            fromUserId: user.id,
            toUserId: targetUser.id,
            status: .pending,
            date: Date()
        )
        
        // Hier würde die Freundschaftsanfrage in der Datenbank gespeichert
    }
    
    func acceptFriendRequest(_ request: FriendRequest) async throws {
        // Hier würde die Freundschaftsanfrage akzeptiert und die Freundschaft erstellt
    }
    
    func declineFriendRequest(_ request: FriendRequest) async throws {
        // Hier würde die Freundschaftsanfrage abgelehnt
    }
    
    func createGroup(name: String, creator: User, members: [User]) async throws -> Group {
        let group = Group(
            name: name,
            creatorId: creator.id,
            memberIds: members.map { $0.id },
            createdAt: Date()
        )
        
        // Hier würde die Gruppe in der Datenbank erstellt
        return group
    }
    
    func joinGroup(group: Group, user: User) async throws {
        // Hier würde der Benutzer der Gruppe beitreten
    }
    
    func leaveGroup(group: Group, user: User) async throws {
        // Hier würde der Benutzer die Gruppe verlassen
    }
    
    func createEvent(
        title: String,
        description: String,
        date: Date,
        creator: User,
        group: Group?
    ) async throws -> Event {
        let event = Event(
            title: title,
            description: description,
            date: date,
            creatorId: creator.id,
            groupId: group?.id,
            createdAt: Date()
        )
        
        // Hier würde das Event in der Datenbank erstellt
        return event
    }
    
    func joinEvent(event: Event, user: User) async throws {
        // Hier würde der Benutzer dem Event beitreten
    }
    
    func shareActivity(user: User, activity: Activity) async throws {
        // Hier würde die Aktivität geteilt
    }
    
    func getFriends(for user: User) async throws -> [User] {
        // Hier würde die Freundesliste aus der Datenbank geladen
        return []
    }
    
    func getGroups(for user: User) async throws -> [Group] {
        // Hier würden die Gruppen aus der Datenbank geladen
        return []
    }
    
    func getEvents(for user: User) async throws -> [Event] {
        // Hier würden die Events aus der Datenbank geladen
        return []
    }
}

struct FriendRequest: Codable {
    let id: UUID
    let fromUserId: UUID
    let toUserId: UUID
    var status: RequestStatus
    let date: Date
}

struct Group: Codable {
    let id: UUID
    let name: String
    let creatorId: UUID
    var memberIds: [UUID]
    let createdAt: Date
}

struct Event: Codable {
    let id: UUID
    let title: String
    let description: String
    let date: Date
    let creatorId: UUID
    let groupId: UUID?
    let createdAt: Date
    var participants: [UUID]
}

struct Activity: Codable {
    let id: UUID
    let userId: UUID
    let type: ActivityType
    let data: [String: Any]
    let date: Date
}

enum RequestStatus: String, Codable {
    case pending
    case accepted
    case declined
}

enum ActivityType: String, Codable {
    case steps
    case achievement
    case event
    case group
} 