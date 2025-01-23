# Thread Functions C Game

A simple ASCII-based game demonstrating thread usage in C, with progressive gameplay complexity.

## Game Entities

### Obstacles
- Represented by 'O'
- 2×2 square on the game map
- Stationary
- Instant player elimination upon contact

### Enemies
- Represented by 'E'
- Single field on game map
- Move randomly
- Destroy player upon contact

### Player
- Represented by 'P'
- Controlled via WASD keys
- Game quits when:
  - 'q' is pressed
  - Player hits an enemy
  - Player reaches map border
- Lifepoints reset to 0 on elimination

### Collectibles
- Spawned at random locations
- Collected by player movement
- Removed after collection
- Provide points when collected

## Setup

### Prerequisites

#### Debian/Ubuntu
```bash
sudo apt-get install build-essential libncurses-dev
```

#### Additional Ncurses Versions
```bash
sudo apt-get install libncurses6-dev libncursesw6-dev
```

> **Note**: Use ncurses5 only as a last resort after seeking support.

## Installation

1. Clone the repository
2. Compile with ncurses library
3. Run the executable

