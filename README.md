# Test Task

## Protocol Definition

```
Type Enum {
   GREETINGS = 1,
   READY = 2,
   TOKEN = 3
}
```

```
PROTOCOL Record {
  PayloadLength (2),
  Type (1)
  
  Record Payload (PayloadLength),
}
```

```
GREETINGS Record {
  Version (1),
  ClientIdLength (1),
  ClientId (ClientIdLength),
  TokenCount(4),
}
```

```
READY Record {
}
```

```
TOKEN Record {
  Length (8)
  Token (Length)
}
```
