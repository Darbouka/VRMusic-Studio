//
//  StompCoinApp.swift
//  StompCoin
//
//  Created by fatih on 22.05.25.
//

import SwiftUI
import SwiftData

@main
struct StompCoinApp: App {
    @StateObject private var appManager = AppManager.shared
    
    var sharedModelContainer: ModelContainer = {
        let schema = Schema([
            User.self,
            Event.self,
            EventParticipation.self,
            Challenge.self,
            Transaction.self,
            Subscription.self
        ])
        let modelConfiguration = ModelConfiguration(schema: schema, isStoredInMemoryOnly: false)

        do {
            return try ModelContainer(for: schema, configurations: [modelConfiguration])
        } catch {
            fatalError("Could not create ModelContainer: \(error)")
        }
    }()

    var body: some Scene {
        WindowGroup {
            ContentView()
                .environmentObject(appManager)
                .onAppear {
                    appManager.initialize(modelContext: sharedModelContainer.mainContext)
                }
        }
        .modelContainer(sharedModelContainer)
    }
}
