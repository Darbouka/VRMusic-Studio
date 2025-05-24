# StompCoin API Dokumentation

## Überblick
Die StompCoin API ermöglicht die Interaktion mit der StompCoin-Plattform. Sie bietet Endpunkte für Benutzerverwaltung, Wallet-Operationen, Fitness-Tracking, soziale Funktionen und Trading.

## Basis-URL
```
https://api.stompcoin.com/v1
```

## Authentifizierung
Alle API-Anfragen müssen mit einem gültigen API-Token authentifiziert werden. Das Token muss im `Authorization`-Header mitgeführt werden:

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
    "username": "string",
    "email": "string",
    "password": "string"
}
```

**Response:**
```json
{
    "id": "uuid",
    "username": "string",
    "email": "string",
    "createdAt": "datetime"
}
```

#### Anmeldung
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

**Response:**
```json
{
    "token": "string",
    "user": {
        "id": "uuid",
        "username": "string",
        "email": "string"
    }
}
```

### Wallet

#### Wallet-Informationen abrufen
```http
GET /wallets/{id}
```

**Response:**
```json
{
    "id": "uuid",
    "balance": "number",
    "transactions": [
        {
            "id": "uuid",
            "amount": "number",
            "type": "string",
            "date": "datetime"
        }
    ]
}
```

#### Transaktion erstellen
```http
POST /wallets/{id}/transactions
```

**Request Body:**
```json
{
    "amount": "number",
    "type": "string",
    "description": "string"
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
    "date": "datetime"
}
```

#### Statistiken abrufen
```http
GET /fitness/stats
```

**Response:**
```json
{
    "steps": "number",
    "caloriesBurned": "number",
    "distance": "number",
    "activeMinutes": "number"
}
```

### Social

#### Freund hinzufügen
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

#### Freunde abrufen
```http
GET /social/friends
```

**Response:**
```json
{
    "friends": [
        {
            "id": "uuid",
            "username": "string",
            "status": "string"
        }
    ]
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

#### Mining starten
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

## Support
Bei Fragen oder Problemen wenden Sie sich bitte an:
- E-Mail: api-support@stompcoin.com
- Dokumentation: https://docs.stompcoin.com 