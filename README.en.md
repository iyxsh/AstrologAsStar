# AstrologAsStar

Astrolog32 Refactoring Project - A Natal Chart Calculation Engine Based on Swiss Ephemeris

## Project Overview

AstrologAsStar is a refactored project of the classic Astrolog32, preserving its core functionality while being rewritten in modern C++, integrating the Swiss Ephemeris library to provide high-precision celestial position calculations.

## Project Structure

```
astroproject/
├── include/              # Header files directory
│   ├── core/            # Core computation modules
│   │   ├── aspects.h   # Aspect calculations
│   │   ├── chart.h     # Natal chart computation
│   │   ├── ephemeris.h # Ephemeris management
│   │   ├── fixed_stars.h # Fixed star calculations
│   │   ├── houses.h    # House systems
│   │   ├── lunar_nodes.h # Lunar nodes
│   │   ├── options.h   # Option configuration
│   │   ├── planet.h    # Planet calculations
│   │   ├── progressions.h # Progressions
│   │   ├── synastry.h  # Synastry analysis
│   │   └── transits.h  # Transit calculations
│   ├── data/           # Data modules
│   │   └── signs.h     # Zodiac sign data
│   ├── models/         # Data models
│   │   ├── chart_data.h # Chart data
│   │   ├── settings.h  # Settings management
│   │   └── version.h   # Version information
│   ├── utils/          # Utility functions
│   │   ├── TransU.h    # Conversion utilities
│   │   ├── formatter.h # Formatting utilities
│   │   ├── parser.h    # Parser
│   │   └── utils.h     # General utilities
│   ├── astrolog.h      # Main header file
│   └── astrolog_lib.h  # Library interface header
├── src/                # Source code directory
│   ├── core/           # Core module implementations
│   │   ├── aspects.cpp
│   │   ├── chart.cpp
│   │   ├── ephemeris.cpp
│   │   ├── fixed_stars.cpp
│   │   ├── houses.cpp
│   │   ├── lunar_nodes.cpp
│   │   ├── planet.cpp
│   │   ├── progressions.cpp
│   │   ├── synastry.cpp
│   │   └── transits.cpp
│   ├── utils/          # Utility implementations
│   │   ├── TransU.cpp
│   │   ├── formatter.cpp
│   │   ├── parser.cpp
│   │   └── utils.cpp
│   └── astrolog.cpp    # Main program entry point
├── swe/                # Swiss Ephemeris library
│   ├── inc/           # Header files
│   └── lib/           # Precompiled libraries
├── test/              # Test programs
│   ├── apichart2.cpp
│   ├── apimainchart.cpp
│   └── oldmain.cpp
├── bin/               # Compilation output directory
├── lib/               # Library output directory
└── CMakeLists.txt     # CMake build configuration
```

## Core Features

### Natal Chart Calculation
- Supports calculation of multiple planets (Sun, Moon, Mercury, Venus, Mars, Jupiter, Saturn, Uranus, Neptune, Pluto, etc.)
- Accurate fixed star position calculations
- Support for multiple house systems (Placidus, Koch, Regiomontanus, Campanus, etc.)

### Aspect Analysis
- Automatic calculation of all planetary aspects
- Supports multiple aspect types: Conjunction, Sextile, Square, Trine, Opposition
- Supports retrograde calculations

### Advanced Features
- **Progressions**: Solar arc progressions and secondary progressions
- **Transits**: Transit chart calculations
- **Synastry**: Comparison of two natal charts
- **Solar/Lunar Returns**: Solar return and lunar return charts

### Data Output
- Text-format chart output
- Detailed aspect lists
- Planetary influence calculations

## Compilation Instructions

### System Requirements

- C++ compiler (MSVC, GCC, Clang)
- CMake 3.10+
- Windows or Linux operating system

### Basic Compilation Steps

```bash
# Create build directory
mkdir build
cd build

# Configure project
cmake ..

# Build
cmake --build .
```

### Compilation on Windows

The project is pre-configured with a Visual Studio solution; you can open and compile it directly in Visual Studio, or generate the solution using CMake:

```bash
cmake -G "Visual Studio 17 2022" -A Win32 ..
```

### Debug and Release Modes

#### Debug Mode (Default)
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

#### Release Mode
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
```

### Output Files

After compilation, executable files and libraries are located at:

- **Debug Mode**: `astroproject/bin/windows/Debug/`
- **Release Mode**: `astroproject/bin/windows/Release/`
- **Library Files**: `astroproject/lib/windows/Debug/` or `Release/`

## Library Interface

The project provides library interfaces for use as either a dynamic or static library:

### Static Library
- `astrolog32.lib` - Static linking library

### Dynamic Library
- `swedll32.dll` - Swiss Ephemeris runtime DLL

### API Example

```cpp
#include "astrolog.h"

// Set chart data
ChartInput chartInput;
chartInput.year = 2024;
chartInput.month = 1;
chartInput.day = 15;
chartInput.time = 12.0;  // 12:00
chartInput.dst = 0;
chartInput.zon = 8.0;    // UTC+8
chartInput.lon = 116.4;  // Beijing longitude
chartInput.lat = 39.9;   // Beijing latitude

// Set chart data and compute
SetChartData(chart_Newton, chartInput, true);

// Retrieve aspect results
std::wstring aspectResult = GetMainChartAspect();
```

## Planets and Celestial Bodies

The project supports calculations for the following celestial bodies:

| Body | Description |
|------|-------------|
| Sun | Sun |
| Moon | Moon |
| Mercury | Mercury |
| Venus | Venus |
| Mars | Mars |
| Jupiter | Jupiter |
| Saturn | Saturn |
| Uranus | Uranus |
| Neptune | Neptune |
| Pluto | Pluto |
| Chiron | Chiron |
| Vesta | Vesta |
| Pallas | Pallas |
| Juno | Juno |
| Ceres | Ceres |
| Lunar Nodes | North Node, South Node |

## Dependencies

- **Swiss Ephemeris**: Provides high-precision ephemeris calculations
- **C++ Standard Library**: Standard Template Library and string handling

## License

This project is licensed under the original Astrolog32 license. See the LICENSE file for details.

## Contributions

Issues and pull requests are welcome.

## References

- [Swiss Ephemeris](https://www.astro.com/swisseph/)
- [Astrolog](https://www.astrolog.org/)