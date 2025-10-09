# THUG Networking System

## Overview

Tony Hawk's Underground features a robust multiplayer networking system supporting both LAN and Internet play. The system uses a client-server architecture with UDP sockets for low-latency gameplay, supporting up to 8 players simultaneously in various game modes.

## Table of Contents

- [Architecture](#architecture)
- [Network Protocol](#network-protocol)
- [Client-Server Model](#client-server-model)
- [Message System](#message-system)
- [Data Synchronization](#data-synchronization)
- [Game Modes](#game-modes)
- [Lobby System](#lobby-system)
- [Known Limitations](#known-limitations)

---

## Architecture

### System Structure

The networking system is organized into two main layers:

```
┌─────────────────────────────────────────────────────┐
│        GameNet::Manager (Game Network Layer)        │
│      Code/Sk/GameNet/GameNet.h/cpp                  │
│    - Game-specific network logic                    │
│    - Player management                              │
│    - Game mode coordination                         │
│    - Score synchronization                          │
└──────────────────┬──────────────────────────────────┘
                   ↓
┌─────────────────────────────────────────────────────┐
│       Net::Manager (Core Network Layer)             │
│      Code/Gel/Net/net.h/cpp                         │
│    - Socket management                              │
│    - Connection handling                            │
│    - Message routing                                │
│    - Low-level protocol                             │
└──────────────────┬──────────────────────────────────┘
                   ↓
┌─────────────────────────────────────────────────────┐
│           Platform Socket APIs                      │
│    - Windows: Winsock2                              │
│    - PlayStation 2: SN Systems socket API           │
│    - Xbox: Winsock X                                │
│    - GameCube: Dolphin socket API                   │
└─────────────────────────────────────────────────────┘
```

### Design Philosophy

1. **Client-Server**: Authoritative server, predictive clients
2. **UDP Protocol**: Low-latency, unreliable transport
3. **Message-Based**: Discrete messages with handlers
4. **State Synchronization**: Periodic full state updates
5. **Platform Abstraction**: Unified API across platforms

---

## Network Protocol

### Transport Protocol

**Protocol**: UDP (User Datagram Protocol)

**Reasons for UDP**:
- **Low Latency**: No connection overhead
- **Real-Time**: Better for fast-paced action
- **Multicast Support**: Efficient for multiple clients
- **Custom Reliability**: Implement only what's needed

**Trade-offs**:
- **No Guarantees**: Packets can be lost or reordered
- **Custom Reliability**: Must implement acknowledgments
- **NAT Traversal**: Requires special handling

### Connection Management

**File**: `Code/Gel/Net/net.h`

Connections are managed through the `Conn` class:

```cpp
class Conn
{
public:
    enum State
    {
        mREMOTE_PENDING,    // Connection being established
        mREMOTE,            // Connected
        mCLOSED,            // Disconnected
    };
    
    int GetHandle() const;
    State GetState() const;
    
    void Send(MsgDesc* pMsg);
    void Disconnect();
    
    struct sockaddr_in GetAddress() const;
    int GetLatency() const;          // Round-trip time (ms)
    int GetPacketLoss() const;       // Packet loss percentage
};
```

### Network Modes

**File**: `Code/Sk/GameNet/GameNet.h`

```cpp
enum NetworkMode
{
    vNO_NET_MODE,       // Single player / no network
    vLAN_MODE,          // Local area network
    vINTERNET_MODE,     // Internet play
};

enum JoinMode
{
    vJOIN_MODE_PLAY,    // Join as player
    vJOIN_MODE_OBSERVE, // Join as observer
};

enum
{
    mSERVER  = 0x0001,  // Server flag
    mCLIENT  = 0x0002,  // Client flag
    mINTERNET= 0x0004,  // Internet mode
    mLAN     = 0x0008,  // LAN mode
};
```

### Version Control

**File**: `Code/Sk/GameNet/GameNet.h`

```cpp
enum
{
    // Version number: high word = major, low word = minor
    vVERSION_NUMBER = 0x30036,  // Version 3.54
};
```

Clients and servers must have matching version numbers to connect.

---

## Client-Server Model

### Server Responsibilities

**File**: `Code/Gel/Net/Server/netserv.h`

```cpp
class Server : public App
{
public:
    Server(int flags = 0);
    
    void ReceiveData();
    void SendEnqueuedMessages(Conn* conn);
    
    // Alias management (object ID compression)
    void AllocateAliasTables();
    void ClearAliasTables();
    unsigned char GetObjectAlias(int handle, int obj_id, int cur_time);
    void SetObjectAlias(int handle, unsigned char alias, 
                        int obj_id, int expiration);
};
```

**Server Role**:
1. **Authority**: Final word on game state
2. **State Management**: Track all objects and players
3. **Event Processing**: Handle player actions
4. **Broadcast**: Send state updates to all clients
5. **Validation**: Check for cheating/illegal moves

### Client Responsibilities

**File**: `Code/Gel/Net/Client/netclnt.h`

```cpp
class Client : public App
{
public:
    Client(int flags = 0);
    
    bool ConnectToServer(int ip, unsigned short port);
    void ReceiveData();
    void SendEnqueuedMessages(Conn* conn);
    
    // Alias management
    void ClearAliasTable();
    unsigned char GetObjectAlias(int obj_id, int cur_time);
    unsigned char NewObjectAlias(int obj_id, int cur_time, 
                                 bool expires = true);
    int GetObjectId(unsigned char alias);
};
```

**Client Role**:
1. **Input**: Send player actions to server
2. **Prediction**: Estimate state between updates
3. **Interpolation**: Smooth movement between states
4. **Rendering**: Display game state
5. **Correction**: Apply server authoritative updates

### Connection Flow

```
Client                              Server
  │                                   │
  │  1. Connection Request            │
  ├─────────────────────────────────>│
  │                                   │
  │  2. Connection Accepted/Refused   │
  │<─────────────────────────────────┤
  │                                   │
  │  3. Game State Sync               │
  │<─────────────────────────────────┤
  │                                   │
  │  4. Player Input                  │
  ├─────────────────────────────────>│
  │                                   │
  │  5. State Updates (periodic)      │
  │<─────────────────────────────────┤
  │                                   │
  │  6. Disconnect Request            │
  ├─────────────────────────────────>│
  │                                   │
  │  7. Disconnect Confirmation       │
  │<─────────────────────────────────┤
  │                                   │
```

---

## Message System

### Message Structure

**File**: `Code/Sk/GameNet/GameMsg.h`

Messages are the fundamental unit of network communication:

```cpp
struct MsgDesc
{
    unsigned short m_Id;        // Message type ID
    unsigned short m_Length;    // Payload length
    unsigned char m_Data[];     // Payload data
};
```

### Message Types

**Message Categories**:

1. **Connection Messages**
   - `MSG_ID_CONNECT_REQUEST`
   - `MSG_ID_CONNECT_ACCEPT`
   - `MSG_ID_CONNECT_REFUSE`
   - `MSG_ID_DISCONNECT_REQUEST`

2. **Game State Messages**
   - `MSG_ID_TIMESTAMP` - Time synchronization
   - `MSG_ID_PLAYER_STATE` - Player position/orientation
   - `MSG_ID_OBJECT_UPDATE` - Object state changes
   - `MSG_ID_SCORE_UPDATE` - Score changes

3. **Game Event Messages**
   - `MSG_ID_TRICK_PERFORMED` - Player performed trick
   - `MSG_ID_BAIL` - Player bailed
   - `MSG_ID_GRIND_START` - Started grinding
   - `MSG_ID_GRIND_END` - Stopped grinding

4. **Game Control Messages**
   - `MSG_ID_START_GAME` - Begin game
   - `MSG_ID_END_GAME` - End game
   - `MSG_ID_PAUSE_GAME` - Pause game
   - `MSG_ID_CHANGE_LEVEL` - Load new level

### Message Handlers

**File**: `Code/Gel/Net/net.h`

Message handlers are registered callbacks:

```cpp
typedef int (*MsgHandlerFunction)(MsgDesc* pMsg, Conn* conn);

struct MsgHandlerCode
{
    unsigned short m_Id;            // Message ID
    MsgHandlerFunction m_Handler;   // Handler function
};

// Register handler
void RegisterHandler(unsigned short msgId, 
                     MsgHandlerFunction handler);

// Example handler
int HandlePlayerState(MsgDesc* pMsg, Conn* conn)
{
    // Extract player ID
    int playerId = *(int*)pMsg->m_Data;
    
    // Extract position
    float x = *(float*)(pMsg->m_Data + 4);
    float y = *(float*)(pMsg->m_Data + 8);
    float z = *(float*)(pMsg->m_Data + 12);
    
    // Update player
    UpdatePlayer(playerId, x, y, z);
    
    return 0;  // Success
}
```

### Message Queue

Messages are queued before sending:

```cpp
class App
{
public:
    void EnqueueMessage(MsgDesc* pMsg, Conn* conn);
    void SendEnqueuedMessages(Conn* conn);
    
private:
    Lst::Head<MsgDesc> m_msg_queue;
};
```

**Advantages**:
- **Batching**: Send multiple messages together
- **Priority**: Send critical messages first
- **Rate Limiting**: Prevent flooding

---

## Data Synchronization

### Object Aliasing

**Purpose**: Compress object IDs for bandwidth efficiency

**File**: `Code/Gel/Net/Client/netclnt.h`

```cpp
#ifdef USE_ALIASES
// Alias table entry
struct AliasEntry
{
    int obj_id;             // Full object ID (32-bit)
    unsigned char alias;    // Compressed alias (8-bit)
    int expiration;         // Time when alias expires
};

// Alias table size
enum { vNUM_ALIASES = 256 };

// Get alias for object
unsigned char GetObjectAlias(int obj_id, int cur_time);

// Create new alias
unsigned char NewObjectAlias(int obj_id, int cur_time, 
                             bool expires = true);

// Lookup object from alias
int GetObjectId(unsigned char alias);
#endif
```

**Benefit**: Reduces object ID from 32 bits to 8 bits (4x compression)

### State Updates

**Full State Update**:
- Sent periodically (e.g., every second)
- Contains complete game state
- Corrects accumulated errors

**Delta Update**:
- Sent frequently (e.g., every frame)
- Contains only changes since last update
- Efficient bandwidth usage

**Example**:

```cpp
// Full state (sent every 1 second)
struct FullPlayerState
{
    int playerId;
    float position[3];      // x, y, z
    float velocity[3];      // vx, vy, vz
    float orientation[4];   // quaternion
    int score;
    int health;
    int combo;
};

// Delta state (sent every frame)
struct DeltaPlayerState
{
    unsigned char alias;    // Player alias (8-bit)
    short deltaPos[3];      // Position delta (16-bit each)
    short deltaVel[3];      // Velocity delta
};
```

### Time Synchronization

**File**: `Code/Gel/Net/net.h`

Clients and server synchronize clocks:

```cpp
// Timestamp message
struct TimestampMsg
{
    unsigned short msgId;       // MSG_ID_TIMESTAMP
    int serverTime;             // Server's current time
    int clientTime;             // Client's time (echo)
};

// Calculate latency
int CalculateLatency(TimestampMsg* pMsg)
{
    int now = GetCurrentTime();
    int latency = (now - pMsg->clientTime) / 2;
    return latency;
}

// Adjust client time
void SynchronizeTime(int serverTime, int latency)
{
    int adjustedServerTime = serverTime + latency;
    SetClientTime(adjustedServerTime);
}
```

### Interpolation and Prediction

**Interpolation**: Smooth movement between received states

```cpp
void InterpolatePlayer(Player* pPlayer, float t)
{
    // t = interpolation factor (0.0 to 1.0)
    
    // Interpolate position
    pPlayer->position = Lerp(pPlayer->prevPos, 
                             pPlayer->nextPos, t);
    
    // Interpolate orientation
    pPlayer->orientation = Slerp(pPlayer->prevOrient, 
                                 pPlayer->nextOrient, t);
}
```

**Prediction**: Estimate future state based on current velocity

```cpp
void PredictPlayer(Player* pPlayer, float dt)
{
    // Predict position
    pPlayer->predictedPos = pPlayer->position + 
                            pPlayer->velocity * dt;
    
    // Apply physics (gravity, friction)
    pPlayer->velocity.y -= GRAVITY * dt;
    pPlayer->velocity *= (1.0f - FRICTION * dt);
}
```

**Correction**: Blend predicted state with authoritative update

```cpp
void CorrectPlayer(Player* pPlayer, Vec3 authPos)
{
    // Calculate error
    Vec3 error = authPos - pPlayer->predictedPos;
    
    // Apply correction over time (smooth)
    float correctionSpeed = 0.1f;  // 10% per frame
    pPlayer->position += error * correctionSpeed;
}
```

---

## Game Modes

### Supported Modes

**File**: `Code/Sk/GameNet/GameNet.h`

THUG supports various multiplayer game modes:

1. **Free Skate**: Casual skating, no objectives
2. **Trick Attack**: High score competition
3. **Graffiti**: Tag objects with your color
4. **HORSE**: Match tricks or get letters
5. **King of the Hill**: Control designated area
6. **Capture the Flag**: Classic CTF gameplay
7. **Slap!**: Tag other players
8. **Combo Mambo**: Longest combo wins

### Player Management

**Maximum Players**: 8 simultaneous players

```cpp
enum
{
    vMAX_PLAYERS = 8,
};

class PlayerInfo
{
public:
    int m_playerId;                 // Unique player ID
    char m_name[32];                // Player name
    int m_handle;                   // Network handle
    Obj::CSkaterProfile* m_pProfile; // Skater profile
    
    int m_score;                    // Current score
    int m_rank;                     // Current rank
    bool m_isObserver;              // Observer flag
};
```

### Ranking System

**File**: `Code/Sk/GameNet/GameNet.h`

```cpp
enum
{
    vMAX_RANK           = 10,
    vMAX_GAME_CREDIT    = 500,
    vNUM_RANKED_LEVELS  = 12,
    vMAX_SCORE_CREDIT   = 100,
    vMAX_COMBO_CREDIT   = 100,
    vMAX_RATING = (vMAX_GAME_CREDIT + 
                   vNUM_RANKED_LEVELS * 
                   (vMAX_SCORE_CREDIT + vMAX_COMBO_CREDIT)),
};
```

Players earn rating points through:
- Winning games
- High scores
- Long combos
- Completing ranked levels

---

## Lobby System

### Lobby Manager

**File**: `Code/Sk/GameNet/Lobby.h`

The lobby system manages pre-game matchmaking:

```cpp
class Lobby
{
public:
    void Create();                  // Create lobby
    void Join(int lobbyId);         // Join existing lobby
    void Leave();                   // Leave lobby
    
    void SetGameMode(int mode);     // Set game mode
    void SetLevel(int level);       // Set level/map
    void SetMaxPlayers(int max);    // Set player limit
    
    void StartGame();               // Begin game
    
    PlayerInfo* GetPlayers();       // Get player list
    int GetNumPlayers() const;      // Get player count
};
```

### Server Browser

Players can browse available servers:

```cpp
struct ServerInfo
{
    char m_name[64];            // Server name
    int m_ip;                   // IP address
    unsigned short m_port;      // Port number
    int m_numPlayers;           // Current players
    int m_maxPlayers;           // Maximum players
    int m_gameMode;             // Game mode
    int m_level;                // Current level
    int m_ping;                 // Latency (ms)
};

// Server list manager
class ServerList
{
public:
    void Refresh();             // Query servers
    int GetNumServers() const;
    ServerInfo* GetServer(int index);
    void SortByPing();          // Sort by latency
};
```

### Matchmaking

**LAN Discovery**: Broadcast UDP packets to find local games

```cpp
void DiscoverLANServers()
{
    // Broadcast discovery packet
    char packet[] = "THUG_DISCOVER";
    SendBroadcast(packet, sizeof(packet), 6500);
    
    // Listen for responses
    ReceiveResponses(5000);  // 5 second timeout
}
```

**Internet Matchmaking**: Use master server list

```cpp
void QueryMasterServer()
{
    // Connect to master server
    ConnectToServer("masterserver.example.com", 6500);
    
    // Request server list
    SendMessage("GET_SERVER_LIST");
    
    // Receive server list
    ServerInfo* servers = ReceiveServerList();
}
```

---

## Known Limitations

### Platform-Specific Issues

**PlayStation 2**:
- Uses SN Systems socket API (not BSD sockets)
- Limited to 100 Mbit network adapter
- NAT traversal challenges

**Xbox**:
- Uses Xbox Live API (separate from standard networking)
- Requires Xbox Live subscription for Internet play
- Built-in voice chat support

**GameCube**:
- Uses Dolphin socket API
- Requires GameCube broadband adapter
- Limited to 100 Mbit

**PC**:
- Uses Winsock2 (standard Windows sockets)
- Best cross-platform compatibility
- No built-in voice chat (requires 3rd party)

### Network Requirements

**Bandwidth**:
- Minimum: 56 Kbps (dial-up)
- Recommended: 256+ Kbps (broadband)
- Optimal: 1+ Mbps

**Latency**:
- Acceptable: < 150 ms
- Good: < 100 ms
- Excellent: < 50 ms

**Packet Loss**:
- Acceptable: < 5%
- Good: < 2%
- Excellent: < 1%

### Game Limitations

**Player Count**: Maximum 8 players
- More players would require significant protocol changes
- Bandwidth scales linearly with player count

**Level Size**: Large levels can cause synchronization issues
- More objects to track
- Increased bandwidth usage
- Higher memory requirements

**Physics Synchronization**: Imperfect
- Complex physics can diverge between clients
- Requires frequent corrections
- Can cause "rubber-banding" effect

### Security Considerations

**No Encryption**: All data sent in plaintext
- Vulnerable to eavesdropping
- Vulnerable to packet injection

**No Authentication**: Minimal player verification
- Vulnerable to impersonation
- Vulnerable to fake servers

**No Anti-Cheat**: Limited cheat detection
- Client can modify game state
- Score manipulation possible
- Speed hacking possible

**Recommendations**:
- Play on trusted servers
- Use private lobbies with friends
- Report suspicious players

---

## Performance Optimization

### Bandwidth Optimization

1. **Object Aliasing**: Compress IDs from 32 to 8 bits
2. **Delta Updates**: Send only changes
3. **Message Batching**: Combine multiple messages
4. **Compression**: Compress large payloads
5. **Priority**: Send critical data first

### Latency Mitigation

1. **Prediction**: Estimate future state
2. **Interpolation**: Smooth between states
3. **Time Synchronization**: Align client/server clocks
4. **Dead Reckoning**: Extrapolate movement
5. **Lag Compensation**: Account for latency in hit detection

### Code Example: Optimized Player Update

```cpp
// Efficient player state update
struct OptimizedPlayerState
{
    unsigned char alias;        // 1 byte (instead of 4)
    short pos[3];               // 6 bytes (quantized position)
    unsigned char orient;       // 1 byte (quantized orientation)
    short vel[3];               // 6 bytes (quantized velocity)
    // Total: 14 bytes (vs 40+ bytes unoptimized)
};

void SendPlayerUpdate(Player* pPlayer)
{
    OptimizedPlayerState state;
    
    // Get alias
    state.alias = GetObjectAlias(pPlayer->id);
    
    // Quantize position (world coords to grid coords)
    state.pos[0] = (short)(pPlayer->pos.x * 10.0f);
    state.pos[1] = (short)(pPlayer->pos.y * 10.0f);
    state.pos[2] = (short)(pPlayer->pos.z * 10.0f);
    
    // Quantize orientation (quaternion to 8-bit)
    state.orient = QuantizeOrientation(pPlayer->orient);
    
    // Quantize velocity
    state.vel[0] = (short)(pPlayer->vel.x * 100.0f);
    state.vel[1] = (short)(pPlayer->vel.y * 100.0f);
    state.vel[2] = (short)(pPlayer->vel.z * 100.0f);
    
    // Send message
    SendMessage(MSG_ID_PLAYER_UPDATE, &state, sizeof(state));
}
```

---

## Related Documentation

- [ARCHITECTURE.md](../ARCHITECTURE.md) - Overall engine architecture
- [SCRIPTING.md](../SCRIPTING.md) - Scripting system (network events)
- [docs/CODE_STYLE.md](../CODE_STYLE.md) - Coding conventions

## Implementation Files

### Core Network Layer
- `Code/Gel/Net/net.h/cpp` - Network manager (21,783 lines total)
- `Code/Gel/Net/netconn.cpp` - Connection management
- `Code/Gel/Net/Client/netclnt.h/cpp` - Client implementation
- `Code/Gel/Net/Server/netserv.h/cpp` - Server implementation
- `Code/Gel/Net/Dispatch/` - Message dispatching
- `Code/Gel/Net/Handler/` - Message handlers
- `Code/Gel/Net/App/` - Application base

### Game Network Layer
- `Code/Sk/GameNet/GameNet.h/cpp` - Game network manager (8,283 lines)
- `Code/Sk/GameNet/Lobby.h/cpp` - Lobby system (2,410 lines)
- `Code/Sk/GameNet/Player.cpp` - Player management (1,451 lines)
- `Code/Sk/GameNet/ServerList.cpp` - Server browser (2,109 lines)
- `Code/Sk/GameNet/GameMsg.h` - Game message definitions
- `Code/Sk/GameNet/ExportMsg.h` - Export message definitions

### Platform-Specific
- `Code/Gel/Net/Win32/` - Windows implementation (Winsock2)
- `Code/Sk/GameNet/ngps/` - PlayStation 2 implementation (SN Systems)
- Xbox and GameCube implementations (platform-specific APIs)

---

*This document describes the multiplayer networking system in Tony Hawk's Underground.*
