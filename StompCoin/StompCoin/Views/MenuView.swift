NavigationLink(destination: SubscriptionView()) {
    Label("Abonnement", systemImage: "creditcard")
}

NavigationLink(destination: TransactionHistoryView()) {
    Label("Transaktionshistorie", systemImage: "list.bullet.clipboard")
}

NavigationLink(destination: ChallengeView()) {
    Label("Challenges", systemImage: "trophy")
} 