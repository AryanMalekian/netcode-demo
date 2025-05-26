# Netcode Demo

[![Build Status](https://github.com/aryanmalekian/netcode-demo/workflows/Build%20(CMake%20+%20vcpkg%20on%20Windows)/badge.svg)](https://github.com/aryanmalekian/netcode-demo/actions) | [API Dokumentasjon](https://aryanmalekian.github.io/netcode-demo/)

*Netcode-løsning med client-side prediction og server reconciliation*

## Introduksjon

Dette prosjektet implementerer netcode-teknikker for å håndtere nettverksforsinkelse og pakketap. Løsningen viser hvordan client-side prediction, input-buffering og server reconciliation kan brukes for å skape responsiv nettverksopplevelse selv med høy latency. Det er mulig å velge mellom ulike latencies for å se hvordan de ulike teknikkene løser det, i en visuell demo.

Prosjektet sammenligner fem forskjellige tilnærminger side-om-side:
- **Lokal input** (grønn) - umiddelbar respons
- **Server state** (rød) - autoritativ, men forsinket  
- **Naiv prediction** (blå) - enkel ekstrapolering
- **Prediction med reconciliation** (magenta) - korrigerer feilpredictions
- **Interpolation** (oransje) - jevn mellom server-oppdateringer

**MERK!:** Arkitekturale beslutninger, designvalg, prioriteringer og overordnet implementasjonsstrategi er tatt av forfatteren. Den detaljerte kodeimplementasjonen er utviklet i samarbeid med AI-modeller (Claude og ChatGPT) basert på forfatterens spesifikasjoner og krav.

## Implementert Funksjonalitet

### Kjerneteknikker
- **3-thread pipeline architecture** med Producer-Consumer pattern
- **Thread-safe message passing** via `ThreadSafeQueue<Packet>` mellom tråder  
- **Client-side prediction** med input-buffering og rollback
- **Server reconciliation** for å korrigere feilpredictions
- **Artificial network delay simulation** for realistisk testing (80-180ms randomized)
- **Linear interpolation** mellom server states

### Visualisering og Metrics
- **Realtime sammenligning** av fem prediction-metoder
- **Live performance metrics** (FPS, RTT, packet loss, buffer size)
- **Movement tracing** for å visualisere forsinkelse
- **Cross-platform UDP sockets** (Windows/Unix)

### Cross-Platform Implementasjon
Prosjektet bruker kompiler-betingede direktiver for å støtte forskjellige plattformer:

**Windows (Winsock2):**
```cpp
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET socket_t;
#pragma comment(lib, "Ws2_32.lib")
```

**Unix/Linux (BSD Sockets):**
```cpp
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
typedef int socket_t;
```

Non-blocking sockets håndteres forskjellig på hver plattform, og feilhåndtering tilpasses det respektive API-et.

## Eksterne Avhengigheter

### Byggverktøy
- **CMake (3.20+)**: Cross-platform byggesystem for prosjektorganisering
- **vcpkg**: C++ pakkemanager som håndterer bibliotekinstallasjon automatisk

### Biblioteker
- **SFML**: Simple and Fast Multimedia Library
  - Brukt til: Grafikk-rendering, vindushåndtering, input-håndtering og grunnleggende nettverksabstraksjon
  - Komponenter: sfml-graphics, sfml-window, sfml-system
- **Catch2**: Testing framework for C++
  - Brukt til: Unit tests, integration tests og test-rapportering

### Platform-spesifikke
- **Winsock2** (Windows): Windows Socket API for UDP-kommunikasjon
- **BSD Sockets** (Unix/Linux): POSIX socket API for nettverkskommunikasjon

## Installasjon og Oppstart

### Forutsetninger
- C++17-kompatibel kompiler (MSVC 2019+, GCC 9+, Clang 10+)
- Git
- CMake 3.20+
- vcpkg (global installasjon)

### Steg 1: Installer vcpkg (hvis ikke allerede installert)

**Windows:**
```bash
git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg
C:\vcpkg\bootstrap-vcpkg.bat
```

**Linux/macOS:**
```bash
git clone https://github.com/Microsoft/vcpkg.git ~/vcpkg
~/vcpkg/bootstrap-vcpkg.sh
```

### Steg 2: Klon og installer dependencies

```bash
# Klon repository
git clone https://github.com/aryanmalekian/netcode-demo.git
cd netcode-demo

# Installer dependencies med vcpkg manifest mode
vcpkg install
```

### Steg 3: Bygg prosjektet

**Windows:**
```bash
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake --build build --config Release
```

**Linux/macOS:**
```bash
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="~/vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake --build build --config Release
```

### Steg 4: Start applikasjonen

**1. Start server (første terminal):**
```bash
cd build/bin/Release
./netcode-server
```

**2. Start klient (nytt terminal):**
```bash
cd build/bin/Release
./netcode-client
```

### Kontroller og bruk
- **Piltaster**: Beveg objektet
- **1-5**: Velg latency nivåer (5-450ms)
- **C**: Nullstill movement traces
- **Lukk vindu**: Avslutt programmet

Følg de fem fargede prikkene som viser forskjellige prediction-metoder. Live metrics vises øverst med FPS, RTT, packet loss og buffer status.

## Kjøring av Tester

Prosjektet har omfattende test-suite med både unit tests og integration tests.

### Test-kategorier
- **Packet tests**: Serialization, validation, network byte order
- **Prediction tests**: PredictionSystem, reconciliation, edge cases  
- **Interpolation tests**: Linear interpolasjon matematikk
- **Client/Server tests**: Core funksjonalitet og echo-testing
- **Integration tests**: DelaySimulator, Trail, metrics, error handling

### Kjør tester
```bash
cd build

# Kjør alle tester
ctest --output-on-failure

# Eller kjør direkte
cd bin/Release
./netcode_tests

# Spesifikke test-kategorier
./netcode_tests "[Packet]"         # Packet-relaterte tester
./netcode_tests "[Prediction]"     # Prediction-algoritme tester  
./netcode_tests "[Interpolation]"  # Interpolation tester
./netcode_tests "[Integration]"    # Integration tester
./netcode_tests "[client]"         # Client tester
./netcode_tests "[server]"         # Server tester
```

Test coverage dekker ca. 80% av core-funksjonaliteten (ekskluderer SFML rendering og socket operasjoner).

## API Dokumentasjon

Dokumentasjonen er tilgjengelig på GitHub Pages: [https://aryanmalekian.github.io/netcode-demo/](https://aryanmalekian.github.io/netcode-demo/)

For lokal generering av dokumentasjon:
```bash
# Generer dokumentasjon (krever Doxygen installert)
doxygen Doxyfile

# Åpne dokumentasjon
# Windows: start docs/html/index.html  
# Linux: xdg-open docs/html/index.html
# macOS: open docs/html/index.html
```

## Fremtidig Arbeid

### Nåværende Mangler/Svakheter
- **Server-side lag compensation** - enkel autoritativ server implementert, men ikke avansert lag compensation
- **Flere klienter samtidig** - støtter kun én klient
- **Pakke-kompresjon** for båndbredde-optimalisering
- **Adaptive prediction** basert på nettverkskvalitet

### Mulige Utvidelser
- Prediction rollback for andre spillere
- Autoritativ fysikk på server-side  
- Nettverkssimulering med jitter og variable packet loss
- Optimistisk kollisjonshåndtering

## Arkitektur og Design

### Prosjektstruktur
Prosjektet følger standard C++ header/implementation separasjon:

```
include/netcode/           # Public headers
├── common/               # Delte datatyper og algoritmer
│   ├── packet.hpp       # UDP pakkeformat
│   ├── prediction.hpp   # Prediksjons-API
│   ├── interpolation.hpp # Interpolasjonsalgoritmer
│   └── input.hpp        # Input-kommando struktur
src/                      # Implementasjoner
├── common/              # Delte implementasjoner
├── client/              # Klient-kode
└── server/              # Server-kode
tests/                    # Test-kode organisert etter komponent
```

### 3-Thread Pipeline Design
- **Main thread**: Håndterer rendering, input og game logic (60 FPS)
- **Delay Simulation thread**: Behandler artificial network delay, jitter og packet loss simulation
- **Network I/O thread**: Ren UDP send/receive operasjoner
- **Thread-safe pipeline**: Producer-Consumer chains med `ThreadSafeQueue<Packet>`
- **Condition variables**: Effektiv thread-kommunikasjon uten polling

Pipeline dataflow:
```
Main Thread → Delay Sim Thread → Network I/O Thread → UDP
UDP → Network I/O Thread → Delay Sim Thread → Main Thread
```

Kommunikasjon mellom tråder skjer via thread-safe køer med condition variables for effektiv event-driven processing, og size-begrensninger for å unngå minneproblemer.

### Prediction Algorithm
Implementerer moderne netcode-prinsipper:
1. **Input Prediction**: Anvend lokal input umiddelbart for responsivitet
2. **Input Buffering**: Lagre inputs til server bekrefter mottak
3. **Server Reconciliation**: Sammenlign, rull tilbake og replay ved avvik
4. **Smooth Correction**: Gradvis correction over flere frames for å unngå synlige hopp

### Cross-Platform Abstraksjon
Socket-håndtering abstrakseres gjennom:
- Felles `socket_t` typedef
- Platform-spesifikke error-håndtering
- Betinget kompilering for non-blocking socket setup
- Unified API for alle nettverksoperasjoner

### Build System
CMake organiserer prosjektet i moduler:
- `netcode-common`: Static library med delte komponenter
- `client`: Executable som linker mot common og SFML
- `server`: Minimal executable for UDP echo
- `netcode-tests`: Test executable med Catch2

vcpkg manifest mode (`vcpkg.json`) sikrer konsistent dependency-håndtering på tvers av plattformer.

## Continuous Integration

GitHub Actions bygger og tester automatisk på Windows med MSVC, vcpkg og Ninja. CI-pipeline inkluderer:
- Automatisk vcpkg setup og caching
- Release-bygging med optimalisering
- Kjøring av alle test-suiter
- Doxygen dokumentasjonsgenerering

Se `.github/workflows/build.yml` for fullstendige CI/CD-konfigurasjon.

---

**Prosjekt utviklet som del av IDATT-2104 Frivillig Prosjekt for å demonstrere en Netcode løsning**
