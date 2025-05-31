// swift-tools-version:6.1
import PackageDescription

let package = Package(
    name: "StompCoin",
    platforms: [
        .macOS(.v12)
    ],
    products: [
        .executable(
            name: "StompCoin",
            targets: ["StompCoin"]
        )
    ],
    dependencies: [
        .package(url: "https://github.com/Alamofire/Alamofire.git", from: "5.6.4"),
        .package(url: "https://github.com/SDWebImage/SDWebImage.git", from: "5.15.0")
    ],
    targets: [
        .executableTarget(
            name: "StompCoin",
            dependencies: [
                "Alamofire",
                "SDWebImage"
            ],
            path: "Sources/StompCoin",
            resources: [
                .process("Resources")
            ]
        ),
        .testTarget(
            name: "StompCoinTests",
            dependencies: ["StompCoin"],
            path: "Tests/StompCoinTests"
        )
    ]
) 