# Space Football

A 2D physics-based soccer game built with C++ and SDL2, featuring gameplay across different planetary environments with unique physics properties.

## Overview

Space Football is a team-based soccer game where players compete on different celestial bodies - Earth, Moon, and Space/Mars. Each environment features distinct physics properties that affect player movement and ball behavior, creating varied gameplay experiences.

## Features

- **Multiple Environments**: Play on Earth, Moon, or Space with different friction and physics
- **Game Modes**:
  - PVP: Local multiplayer for two players
  - PVE: Single player vs AI
- **Player Types**: Choose from specialized player classes with unique attributes
  - Strikers: Power Shooter, Controller, Speeder
  - Defenders: Tackle, Shield
- **Physics-Based Gameplay**: Realistic movement with acceleration, velocity, and friction
- **Team Management**: Switch between active players during gameplay
- **Match System**: Timed matches with scoring and goal detection

## Requirements

- C++ compiler with C++17 support (GCC recommended)
- SDL2 development libraries (included in project)
- Windows environment (MSYS2/MinGW recommended)

## Installation

1. Clone the repository:

```bash
git clone <repository-url>
cd Space-Football
```

2. Ensure you have a C++ compiler installed (GCC/MinGW)

3. The project includes all necessary SDL2 libraries in the `lib/` and `include/` directories

## Building and Running

### Build the Game

```bash
g++ -o main.exe main.cpp settings.cpp utils.cpp object.cpp gameplay.cpp -I include\SDL2 -Llib -lmingw32 -lSDL2main -lSDL2
```

### Run the Game

```bash
./main.exe
```

## Controls

### Player 1 (Red Team)

- **W**: Move up
- **S**: Move down
- **A**: Move left
- **D**: Move right

### Player 2 (Blue Team) - PVP Mode Only

- **↑**: Move up
- **↓**: Move down
- **←**: Move left
- **→**: Move right

## Gameplay

### Game Flow

1. **Main Menu**: Select game mode (PVP/PVE) and access help
2. **Choose Map**: Select playing environment (Earth/Moon/Space)
3. **Choose Players**: Pick your team composition
4. **Playing**: Core gameplay with physics-based movement
5. **Scoring**: Goal celebration and score updates
6. **Time Up**: Match results and rematch options

### Player Attributes

Each player type has unique characteristics:

- **Toughness**: Affects player vs player collisions
- **Ball Control**: Influences how easily the player controls the ball
- **Power**: Determines shooting strength, especially near opponent goals

### Environments

- **Earth**: Standard friction and gravity
- **Moon**: Reduced friction, floatier movement
- **Space/Mars**: Minimal friction, momentum-based gameplay

## Project Structure

```
Space-Football/
├── main.cpp/h          # Entry point and main game loop
├── gameplay.cpp/h      # Game logic and team management
├── object.cpp/h        # Player and Ball classes
├── artist.cpp/h        # Rendering and graphics
├── settings.cpp/h      # Game constants and configuration
├── utils.cpp/h         # Utility functions and math
├── assets/image/       # Game sprites and backgrounds
├── include/SDL2/       # SDL2 header files
├── lib/                # SDL2 libraries
└── SDL2.dll           # Required SDL2 runtime
```

## Development

### Adding New Features

1. Add function declarations to appropriate header files
2. Implement functions in corresponding .cpp files
3. Include new .cpp files in the compile command if creating new modules
4. Follow existing code patterns and conventions

### Debugging

- Set `GAME_STATE` manually before the main loop for testing specific states
- Use printf statements for debugging (comments note inconsistent commenting style)

## Contributing

1. Create a new branch for your feature
2. Follow existing code style and patterns
3. Test your changes thoroughly
4. Submit a pull request with a clear description

## License

This project is for educational purposes. Please respect any third-party library licenses (SDL2).

## Technical Details

- **Engine**: Custom C++ game engine with SDL2
- **Physics**: Time-based delta calculations for smooth movement
- **Rendering**: SDL2 with hardware acceleration
- **Frame Rate**: Fixed 60 FPS update loop
- **Architecture**: Object-oriented design with clear separation of concerns
