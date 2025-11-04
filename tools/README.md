# THUG Level Editor

A standalone graphical level editor for Tony Hawk's Underground (THUG) that allows you to create and edit custom levels with full support for object manipulation, rail nodes, and goals.

## Features

### Object Management
- **Add, edit, and delete objects** in your level
- Support for multiple object types:
  - TrickObject
  - MovingObject
  - GameObject
  - Vehicle
  - Ped (Pedestrian)
  - Prop
- **Full 3D positioning** (X, Y, Z coordinates)
- **Rotation** control (degrees)
- **Custom properties** per object

### Rail System
- **Create rail strings** for grinding
- Support for both **loops** and **paths**
- **Multiple rail points** per string
- Rail types: **Metal** and **Wood**
- Position each rail point in 3D space

### Goal System
- **Place and configure goals** throughout your level
- Multiple goal types from THUG:
  - CompetitionGoal
  - RaceGoal
  - CATGoal
  - FindGapsGoal
  - HorseGoal
  - FilmGoal
  - SkatetrisGoal
  - BettingGoal
- Configure time limits and score requirements
- Position goals in 3D space

### Level Management
- **Create new levels** from scratch
- **Save/Load levels** in JSON format
- **Level statistics** viewer
- Keyboard shortcuts for common operations
- Clean, intuitive tabbed interface

## Installation

### Requirements

- Python 3.7 or higher
- tkinter (usually included with Python)

### Setup

1. Ensure Python is installed:
```bash
python3 --version
```

2. Navigate to the tools directory:
```bash
cd tools
```

3. Make the editor executable (Linux/Mac):
```bash
chmod +x level_editor.py
```

## Usage

### Starting the Editor

```bash
python3 level_editor.py
```

Or on Linux/Mac after making it executable:
```bash
./level_editor.py
```

### Creating a Level

1. **Start the Editor**: Run the command above
2. **Add Objects**: Go to the "Objects" tab and click "Add Object"
3. **Configure Objects**: Select an object from the list and edit its properties
4. **Add Rails**: Go to the "Rails" tab, add a rail, then add points to it
5. **Add Goals**: Go to the "Goals" tab and place goals in your level
6. **Save**: Press Ctrl+S or use File → Save

### Keyboard Shortcuts

- **Ctrl+N**: New level
- **Ctrl+O**: Open level
- **Ctrl+S**: Save level
- **Ctrl+Shift+S**: Save As

### File Format

Levels are saved as JSON files with the following structure:

```json
{
  "name": "My Custom Level",
  "objects": [
    {
      "id": 1,
      "name": "Object_1",
      "type": "Prop",
      "position": {"x": 0.0, "y": 0.0, "z": 0.0},
      "rotation": 0.0,
      "scale": {"x": 1.0, "y": 1.0, "z": 1.0},
      "properties": {}
    }
  ],
  "rails": [
    {
      "id": 1,
      "is_loop": false,
      "piece_id": 0,
      "points": [
        {
          "position": {"x": 0.0, "y": 0.0, "z": 0.0},
          "type": "metal",
          "object_id": 0
        }
      ]
    }
  ],
  "goals": [
    {
      "id": 1,
      "name": "Goal_1",
      "type": "CompetitionGoal",
      "position": {"x": 0.0, "y": 0.0, "z": 0.0},
      "time_limit": 120,
      "score_required": 0,
      "properties": {}
    }
  ],
  "metadata": {}
}
```

## Architecture

The level editor is based on the THUG ParkEditor2 system found in `Code/Sk/ParkEditor2/`. It uses Python and tkinter to provide a cross-platform GUI for level editing.

### Data Models

The editor implements data models that mirror the THUG codebase:

- **Vector3**: 3D position vector (based on `Mth::Vector`)
- **RailPoint**: Individual rail point (based on `Ed::RailPoint`)
- **RailString**: Rail path/loop (based on `Ed::RailString`)
- **GameObject**: Level object (based on game object system)
- **Goal**: Game objective (based on `Game::CGoal`)
- **Level**: Complete level data structure

### Code Structure

```
level_editor.py
├── Data Models (lines 20-120)
│   ├── ObjectType, RailType, GoalType enums
│   ├── Vector3, RailPoint, RailString classes
│   └── GameObject, Goal, Level dataclasses
│
├── LevelEditor Application (lines 120+)
│   ├── UI Creation
│   ├── Object Management
│   ├── Rail Management
│   ├── Goal Management
│   ├── Level Management
│   ├── Data Serialization
│   └── UI Helpers
```

## Examples

### Example 1: Simple Skate Park

```python
# Create a simple skate park with a rail and a goal
level = Level(name="Simple Park")

# Add a ramp object
ramp = GameObject(
    id=1, name="Quarter Pipe",
    object_type=ObjectType.PROP,
    position=Vector3(0, 0, 0)
)

# Add a rail
rail = RailString(id=1, is_loop=False)
rail.points = [
    RailPoint(position=Vector3(-10, 2, 0)),
    RailPoint(position=Vector3(10, 2, 0))
]

# Add a competition goal
goal = Goal(
    id=1, name="High Score",
    goal_type=GoalType.COMPETITION,
    position=Vector3(0, 0, 0),
    time_limit=120,
    score_required=50000
)

level.objects.append(ramp)
level.rails.append(rail)
level.goals.append(goal)
```

### Example 2: Race Course

```python
# Create a race course with checkpoints
level = Level(name="Race Track")

# Add start line
start_obj = GameObject(
    id=1, name="Start Line",
    object_type=ObjectType.GAMEOBJECT,
    position=Vector3(0, 0, 0)
)

# Add finish line
finish_obj = GameObject(
    id=2, name="Finish Line",
    object_type=ObjectType.GAMEOBJECT,
    position=Vector3(100, 0, 0)
)

# Add race goal
race_goal = Goal(
    id=1, name="Street Race",
    goal_type=GoalType.RACE,
    position=Vector3(0, 0, 0),
    time_limit=180
)

level.objects.extend([start_obj, finish_obj])
level.goals.append(race_goal)
```

## Integration with THUG

The editor outputs JSON files that can be converted to THUG's native format. The data structure is designed to be compatible with:

- **ParkEditor2 system** (`Code/Sk/ParkEditor2/`)
- **Goal system** (`Code/Sk/Modules/Skate/Goal.h`)
- **Rail system** (`Code/Sk/parkeditor/EdRail.h`)
- **Object system** (`Code/Sk/Objects/`)

### Conversion to THUG Format

While this editor outputs JSON, you can write a conversion script to transform the data into THUG's native binary format or script format. The data models match the C++ structures in the codebase.

## Troubleshooting

### Editor won't start
- Ensure Python 3.7+ is installed
- Check that tkinter is available: `python3 -m tkinter`
- On Linux, install tkinter: `sudo apt-get install python3-tk`

### Can't save files
- Check file permissions in the directory
- Ensure you have write access to the destination folder

### Objects not appearing
- Verify JSON format is correct
- Check that all required fields are present
- Validate position coordinates are numbers

## Development

### Adding New Features

The editor is designed to be extensible. To add new features:

1. **New Object Types**: Add to `ObjectType` enum
2. **New Rail Types**: Add to `RailType` enum
3. **New Goal Types**: Add to `GoalType` enum
4. **Custom Properties**: Use the `properties` dict on objects/goals

### Code Style

The code follows Python best practices:
- Type hints for all functions
- Docstrings for all classes and methods
- Dataclasses for data models
- Clear separation of concerns

## Contributing

To contribute improvements to the level editor:

1. Test your changes thoroughly
2. Maintain compatibility with existing level files
3. Document new features
4. Follow the existing code style

## License

This tool is part of the THUG source code repository and follows the same licensing terms. See the main repository's LICENSE file for details.

## Credits

- Based on the Tony Hawk's Underground source code
- ParkEditor2 system by Neversoft Entertainment
- Tool implementation based on THUG codebase structure

## References

- **THUG Repository**: Main repository README
- **ParkEditor2**: `Code/Sk/ParkEditor2/ParkEd.h`
- **Rail System**: `Code/Sk/parkeditor/EdRail.h`
- **Goal System**: `Code/Sk/Modules/Skate/Goal.h`
- **TASKS.md**: `TASK-DOC-009` - Document Park Editor
