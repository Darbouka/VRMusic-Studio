import SwiftUI

struct SocialView: View {
    @StateObject private var viewModel = SocialViewModel()
    @State private var showingAddFriend = false
    @State private var showingCreateGroup = false
    
    var body: some View {
        NavigationView {
            ScrollView {
                VStack(spacing: 20) {
                    // Freundesliste
                    FriendsListCard(friends: viewModel.friends)
                    
                    // Gruppen
                    GroupsCard(groups: viewModel.groups)
                    
                    // Aktivitäten-Feed
                    SocialFeedCard(activities: viewModel.socialActivities)
                }
                .padding()
            }
            .navigationTitle("Social")
            .toolbar {
                ToolbarItem(placement: .navigationBarTrailing) {
                    HStack {
                        Button(action: { showingAddFriend = true }) {
                            Image(systemName: "person.badge.plus")
                        }
                        Button(action: { showingCreateGroup = true }) {
                            Image(systemName: "person.3.fill")
                        }
                    }
                }
            }
            .sheet(isPresented: $showingAddFriend) {
                AddFriendView(viewModel: viewModel)
            }
            .sheet(isPresented: $showingCreateGroup) {
                CreateGroupView(viewModel: viewModel)
            }
            .refreshable {
                await viewModel.refresh()
            }
        }
    }
}

struct FriendsListCard: View {
    let friends: [Friend]
    
    var body: some View {
        VStack(alignment: .leading, spacing: 15) {
            Text("Freunde")
                .font(.headline)
            
            ForEach(friends) { friend in
                FriendRow(friend: friend)
            }
        }
        .padding()
        .background(Color(.systemBackground))
        .cornerRadius(15)
        .shadow(radius: 5)
    }
}

struct FriendRow: View {
    let friend: Friend
    
    var body: some View {
        HStack {
            AsyncImage(url: URL(string: friend.avatarURL)) { image in
                image.resizable()
            } placeholder: {
                Color.gray
            }
            .frame(width: 40, height: 40)
            .clipShape(Circle())
            
            VStack(alignment: .leading) {
                Text(friend.name)
                    .font(.headline)
                Text(friend.status)
                    .font(.caption)
                    .foregroundColor(.gray)
            }
            Spacer()
            if friend.isOnline {
                Circle()
                    .fill(Color.green)
                    .frame(width: 10, height: 10)
            }
        }
        .padding(.vertical, 5)
    }
}

struct GroupsCard: View {
    let groups: [Group]
    
    var body: some View {
        VStack(alignment: .leading, spacing: 15) {
            Text("Gruppen")
                .font(.headline)
            
            ForEach(groups) { group in
                GroupRow(group: group)
            }
        }
        .padding()
        .background(Color(.systemBackground))
        .cornerRadius(15)
        .shadow(radius: 5)
    }
}

struct GroupRow: View {
    let group: Group
    
    var body: some View {
        HStack {
            Image(systemName: "person.3.fill")
                .font(.title2)
                .foregroundColor(.blue)
            
            VStack(alignment: .leading) {
                Text(group.name)
                    .font(.headline)
                Text("Mitglieder: \(group.members.count)")
                    .font(.caption)
                    .foregroundColor(.gray)
            }
            Spacer()
        }
        .padding(.vertical, 5)
    }
}

struct SocialFeedCard: View {
    let activities: [SocialActivity]
    
    var body: some View {
        VStack(alignment: .leading, spacing: 15) {
            Text("Aktivitäten-Feed")
                .font(.headline)
            
            ForEach(activities) { activity in
                SocialActivityRow(activity: activity)
            }
        }
        .padding()
        .background(Color(.systemBackground))
        .cornerRadius(15)
        .shadow(radius: 5)
    }
}

struct SocialActivityRow: View {
    let activity: SocialActivity
    
    var body: some View {
        HStack {
            AsyncImage(url: URL(string: activity.userAvatar)) { image in
                image.resizable()
            } placeholder: {
                Color.gray
            }
            .frame(width: 40, height: 40)
            .clipShape(Circle())
            
            VStack(alignment: .leading) {
                Text(activity.userName)
                    .font(.headline)
                Text(activity.description)
                    .font(.caption)
                    .foregroundColor(.gray)
            }
            Spacer()
            Text(activity.timeAgo)
                .font(.caption)
                .foregroundColor(.gray)
        }
        .padding(.vertical, 5)
    }
}

struct AddFriendView: View {
    @ObservedObject var viewModel: SocialViewModel
    @Environment(\.dismiss) var dismiss
    @State private var friendCode = ""
    
    var body: some View {
        NavigationView {
            Form {
                Section(header: Text("Freund hinzufügen")) {
                    TextField("Freundescode", text: $friendCode)
                }
                Section {
                    Button("Hinzufügen") {
                        // Implementierung Freund hinzufügen
                        dismiss()
                    }
                }
            }
            .navigationTitle("Freund hinzufügen")
            .navigationBarItems(trailing: Button("Abbrechen") {
                dismiss()
            })
        }
    }
}

struct CreateGroupView: View {
    @ObservedObject var viewModel: SocialViewModel
    @Environment(\.dismiss) var dismiss
    @State private var groupName = ""
    
    var body: some View {
        NavigationView {
            Form {
                Section(header: Text("Gruppenname")) {
                    TextField("Name", text: $groupName)
                }
                Section {
                    Button("Erstellen") {
                        // Implementierung Gruppe erstellen
                        dismiss()
                    }
                }
            }
            .navigationTitle("Gruppe erstellen")
            .navigationBarItems(trailing: Button("Abbrechen") {
                dismiss()
            })
        }
    }
}

// MARK: - ViewModel

class SocialViewModel: ObservableObject {
    @Published var friends: [Friend] = []
    @Published var groups: [Group] = []
    @Published var socialActivities: [SocialActivity] = []
    
    func refresh() async {
        // Implementierung der Datenaktualisierung
    }
}

// MARK: - Modelle

struct Friend: Identifiable {
    let id = UUID()
    let name: String
    let avatarURL: String
    let status: String
    let isOnline: Bool
}

struct Group: Identifiable {
    let id = UUID()
    let name: String
    let members: [Friend]
}

struct SocialActivity: Identifiable {
    let id = UUID()
    let userName: String
    let userAvatar: String
    let description: String
    let timeAgo: String
} 