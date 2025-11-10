# Roll - Command Line Dice Roller

## Introduction
A command-line dice roller for tabletop RPGs. 

## Compilation
```bash
gcc -o roll main.c
```
or
```bash
make install
```

## Usage
```
roll [options] [initial_value] [operations...]
```

### Options
- `-l` Show detailed operation process
- `-c` Show boolean result (success/failure)
- `-g` Higher comparison (for dc)
- `-s` Enable great success detection
- `-f` Enable major failure detection
- `-u` Round up results
- `-h` Show help

### Operations
- `+ [num]` Add number
- `- [num]` Subtract number
- `* [num]` Multiply by number
- `/ [num]` Divide by number
- `d [num]` Roll dice: XdY
- `dc [num]` Dice check against target number
- `adv [num]` Advantage: roll X+1, take highest X
- `dis [num]` Disadvantage: roll X+1, take lowest X

## Examples
```
roll 5 d 6          # Roll 5d6
roll -l 2 adv 20    # Advantage roll with details
roll 15 dc 10       # Check if result beats target 10
roll -l -s -f 5 d 20 dc 15  # Complex check with options
```

## Features
- True random number generation
- Advantage/disadvantage mechanics
- Success/failure detection
- Great success/major failure detection
- Flexible arithmetic operations
- Detailed roll process display

## Notes
- Operators and numbers must be space-separated
- Uses system's true random number generator when available