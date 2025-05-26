import SwiftUI

struct LanguageSelectionView: View {
    @StateObject private var localizationService = LocalizationService.shared
    @Environment(\.presentationMode) var presentationMode
    
    var body: some View {
        NavigationView {
            List {
                ForEach(localizationService.availableLanguages(), id: \.code) { language in
                    Button(action: {
                        localizationService.setLanguage(language.code)
                        presentationMode.wrappedValue.dismiss()
                    }) {
                        HStack {
                            Text(language.name)
                                .foregroundColor(.primary)
                            Spacer()
                            if language.code == localizationService.currentLanguage {
                                Image(systemName: "checkmark")
                                    .foregroundColor(.blue)
                            }
                        }
                    }
                }
            }
            .navigationTitle("Sprache auswählen".localized)
            .navigationBarItems(trailing: Button("Fertig".localized) {
                presentationMode.wrappedValue.dismiss()
            })
        }
    }
}

struct LanguageSelectionView_Previews: PreviewProvider {
    static var previews: some View {
        LanguageSelectionView()
    }
} 