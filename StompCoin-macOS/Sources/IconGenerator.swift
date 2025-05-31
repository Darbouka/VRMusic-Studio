import SwiftUI

struct IconGenerator: View {
    var body: some View {
        ZStack {
            // Psychedelischer Hintergrund
            RadialGradient(
                gradient: Gradient(colors: [
                    Color(red: 0.8, green: 0.2, blue: 0.8),  // Pink
                    Color(red: 0.2, green: 0.8, blue: 0.8),  // Türkis
                    Color(red: 0.8, green: 0.8, blue: 0.2),  // Gelb
                    Color(red: 0.2, green: 0.2, blue: 0.8)   // Blau
                ]),
                center: .center,
                startRadius: 100,
                endRadius: 500
            )
            .hueRotation(.degrees(45))
            .saturation(1.5)
            
            // Psychedelische Wellen
            ForEach(0..<3) { i in
                Circle()
                    .stroke(
                        LinearGradient(
                            colors: [.white.opacity(0.3), .clear],
                            startPoint: .top,
                            endPoint: .bottom
                        ),
                        lineWidth: 20
                    )
                    .frame(width: 600 + CGFloat(i * 100))
                    .rotationEffect(.degrees(Double(i) * 30))
            }
            
            // Stilistischer Fußabdruck
            ZStack {
                // Fußballen
                Circle()
                    .fill(.white)
                    .frame(width: 180, height: 180)
                    .shadow(color: .black.opacity(0.3), radius: 10)
                    .offset(y: -60)
                
                // Fußabdruck-Kurve
                Path { path in
                    path.move(to: CGPoint(x: 0, y: 0))
                    path.addQuadCurve(
                        to: CGPoint(x: 0, y: 360),
                        control: CGPoint(x: 90, y: 180)
                    )
                }
                .stroke(
                    LinearGradient(
                        colors: [.white, .white.opacity(0.8)],
                        startPoint: .top,
                        endPoint: .bottom
                    ),
                    lineWidth: 70
                )
                .frame(width: 180, height: 360)
                .shadow(color: .black.opacity(0.2), radius: 5)
            }
            .rotationEffect(.degrees(-30))
            
            // Glanz-Effekt
            Circle()
                .fill(
                    RadialGradient(
                        gradient: Gradient(colors: [.white.opacity(0.3), .clear]),
                        center: .topLeading,
                        startRadius: 0,
                        endRadius: 400
                    )
                )
                .frame(width: 800, height: 800)
                .offset(x: -200, y: -200)
        }
        .frame(width: 1024, height: 1024)
        .background(Color.black)
    }
}

#Preview {
    IconGenerator()
} 