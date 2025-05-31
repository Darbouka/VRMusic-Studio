// swift-tools-version: 5.7
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
        .package(url: "https://github.com/Alamofire/Alamofire.git", from: "5.8.1"),
        .package(url: "https://github.com/SDWebImage/SDWebImage.git", from: "5.18.10")
    ],
    targets: [
        .executableTarget(
            name: "StompCoin",
            dependencies: [
                "Alamofire",
                "SDWebImage"
            ]
        ),
        .testTarget(
            name: "StompCoinTests",
            dependencies: ["StompCoin"]
        )
    ]
) 