# Stomp Coin API Dokumentation

## Übersicht
Die Stomp Coin API ermöglicht die Integration von Fitness-Tracking, Kryptowährung und sozialen Funktionen in Ihre Anwendung.

## Basis-URL
```
https://api.stompcoin.com/v1
```

## Authentifizierung
Alle API-Anfragen müssen mit einem gültigen API-Token authentifiziert werden:
```
Authorization: Bearer <your-api-token>
```

## Endpunkte

### Benutzer

#### Registrierung
```http
POST /users/register
```

**Request Body:**
```json
{
    "email": "string",
    "password": "string",
    "username": "string"
}
```

#### Login
```http
POST /users/login
```

**Request Body:**
```json
{
    "email": "string",
    "password": "string"
}
```

### Fitness

#### Schritte aufzeichnen
```http
POST /fitness/steps
```

**Request Body:**
```json
{
    "steps": "number",
    "timestamp": "string",
    "deviceId": "string"
}
```

#### Fitness-Statistiken
```http
GET /fitness/stats
```

**Query Parameter:**
- `startDate`: Startdatum (ISO 8601)
- `endDate`: Enddatum (ISO 8601)
- `type`: Statistiktyp (daily, weekly, monthly)

### Wallet

#### Wallet-Informationen
```http
GET /wallets/:id
```

#### Transaktionen
```http
POST /wallets/:id/transactions
```

**Request Body:**
```json
{
    "amount": "number",
    "type": "string",
    "description": "string"
}
```

### Social

#### Freunde
```http
POST /social/friends
```

**Request Body:**
```json
{
    "userId": "uuid",
    "friendId": "uuid"
}
```

#### Gruppen
```http
POST /social/groups
```

**Request Body:**
```json
{
    "name": "string",
    "description": "string",
    "members": ["uuid"]
}
```

### Trading

#### Trade erstellen
```http
POST /trading/trades
```

**Request Body:**
```json
{
    "fromUserId": "uuid",
    "toUserId": "uuid",
    "amount": "number",
    "type": "string"
}
```

#### Mining
```http
POST /trading/mining
```

**Request Body:**
```json
{
    "userId": "uuid"
}
```

## Fehlercodes

| Code | Beschreibung |
|------|--------------|
| 400 | Ungültige Anfrage |
| 401 | Nicht authentifiziert |
| 403 | Keine Berechtigung |
| 404 | Ressource nicht gefunden |
| 429 | Zu viele Anfragen |
| 500 | Server-Fehler |

## Rate Limiting
- 100 Anfragen pro Minute pro API-Token
- 1000 Anfragen pro Stunde pro API-Token

## Versionierung
Die API verwendet Semantische Versionierung. Die aktuelle Version ist in der URL enthalten:
```
https://api.stompcoin.com/v1
```

## WebSocket API

### Verbindung
```
wss://api.stompcoin.com/v1/ws
```

### Nachrichten

#### Fitness-Updates
```json
{
    "type": "fitness_update",
    "data": {
        "steps": "number",
        "calories": "number",
        "distance": "number"
    }
}
```

#### Trading-Updates
```json
{
    "type": "trading_update",
    "data": {
        "price": "number",
        "volume": "number",
        "change": "number"
    }
}
```

## SDKs
- iOS SDK: [GitHub Repository](https://github.com/stompcoin/ios-sdk)
- Android SDK: [GitHub Repository](https://github.com/stompcoin/android-sdk)
- Web SDK: [GitHub Repository](https://github.com/stompcoin/web-sdk)

## Support
Bei Fragen oder Problemen wenden Sie sich bitte an:
- E-Mail: api-support@stompcoin.com
- Dokumentation: https://docs.stompcoin.com
- Stack Overflow: [stompcoin](https://stackoverflow.com/questions/tagged/stompcoin) 