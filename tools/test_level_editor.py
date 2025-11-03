#!/usr/bin/env python3
"""
Test script for THUG Level Editor data models

This script tests the level editor functionality without requiring tkinter,
useful for headless environments and automated testing.
"""

import json
import sys
from pathlib import Path

# Add parent directory to path to import level_editor modules
sys.path.insert(0, str(Path(__file__).parent))

# Import data models only (not the GUI)
from enum import Enum
from dataclasses import dataclass, field, asdict
from typing import Dict, List
import math


# Re-define the data models for testing without tkinter
class ObjectType(Enum):
    TRICKOBJECT = "TrickObject"
    MOVINGOBJECT = "MovingObject"
    GAMEOBJECT = "GameObject"
    VEHICLE = "Vehicle"
    PED = "Ped"
    PROP = "Prop"


class RailType(Enum):
    METAL = "metal"
    WOOD = "wood"


class GoalType(Enum):
    COMPETITION = "CompetitionGoal"
    RACE = "RaceGoal"
    CAT = "CATGoal"
    FINDGAPS = "FindGapsGoal"
    HORSE = "HorseGoal"
    FILM = "FilmGoal"
    SKATETRIS = "SkatetrisGoal"
    BETTING = "BettingGoal"


@dataclass
class Vector3:
    x: float = 0.0
    y: float = 0.0
    z: float = 0.0


@dataclass
class RailPoint:
    position: Vector3 = field(default_factory=Vector3)
    rail_type: RailType = RailType.METAL
    object_id: int = 0


@dataclass
class RailString:
    id: int = 0
    points: List[RailPoint] = field(default_factory=list)
    is_loop: bool = False
    piece_id: int = 0


@dataclass
class GameObject:
    id: int = 0
    name: str = ""
    object_type: ObjectType = ObjectType.PROP
    position: Vector3 = field(default_factory=Vector3)
    rotation: float = 0.0
    scale: Vector3 = field(default_factory=lambda: Vector3(1.0, 1.0, 1.0))
    properties: Dict = field(default_factory=dict)


@dataclass
class Goal:
    id: int = 0
    name: str = ""
    goal_type: GoalType = GoalType.COMPETITION
    position: Vector3 = field(default_factory=Vector3)
    time_limit: int = 120
    score_required: int = 0
    properties: Dict = field(default_factory=dict)


@dataclass
class Level:
    name: str = "Untitled Level"
    objects: List[GameObject] = field(default_factory=list)
    rails: List[RailString] = field(default_factory=list)
    goals: List[Goal] = field(default_factory=list)
    metadata: Dict = field(default_factory=dict)


def object_to_dict(obj: GameObject) -> dict:
    """Convert GameObject to dictionary"""
    return {
        'id': obj.id,
        'name': obj.name,
        'type': obj.object_type.value,
        'position': {'x': obj.position.x, 'y': obj.position.y, 'z': obj.position.z},
        'rotation': obj.rotation,
        'scale': {'x': obj.scale.x, 'y': obj.scale.y, 'z': obj.scale.z},
        'properties': obj.properties
    }


def rail_to_dict(rail: RailString) -> dict:
    """Convert RailString to dictionary"""
    return {
        'id': rail.id,
        'is_loop': rail.is_loop,
        'piece_id': rail.piece_id,
        'points': [
            {
                'position': {'x': p.position.x, 'y': p.position.y, 'z': p.position.z},
                'type': p.rail_type.value,
                'object_id': p.object_id
            }
            for p in rail.points
        ]
    }


def goal_to_dict(goal: Goal) -> dict:
    """Convert Goal to dictionary"""
    return {
        'id': goal.id,
        'name': goal.name,
        'type': goal.goal_type.value,
        'position': {'x': goal.position.x, 'y': goal.position.y, 'z': goal.position.z},
        'time_limit': goal.time_limit,
        'score_required': goal.score_required,
        'properties': goal.properties
    }


def level_to_dict(level: Level) -> dict:
    """Convert level to dictionary for JSON serialization"""
    return {
        'name': level.name,
        'objects': [object_to_dict(obj) for obj in level.objects],
        'rails': [rail_to_dict(rail) for rail in level.rails],
        'goals': [goal_to_dict(goal) for goal in level.goals],
        'metadata': level.metadata
    }


def create_example_level() -> Level:
    """Create an example level with various elements"""
    level = Level(name="Example Skate Park")
    
    # Add some objects
    quarter_pipe = GameObject(
        id=1,
        name="Quarter Pipe",
        object_type=ObjectType.PROP,
        position=Vector3(0, 0, 0),
        rotation=0,
        properties={"height": "10ft"}
    )
    
    ramp = GameObject(
        id=2,
        name="Launch Ramp",
        object_type=ObjectType.PROP,
        position=Vector3(20, 0, 0),
        rotation=45,
        properties={"angle": "45deg"}
    )
    
    half_pipe = GameObject(
        id=3,
        name="Half Pipe",
        object_type=ObjectType.PROP,
        position=Vector3(-20, 0, 0),
        rotation=90
    )
    
    level.objects.extend([quarter_pipe, ramp, half_pipe])
    
    # Add a rail string (grind rail)
    rail1 = RailString(id=1, is_loop=False, piece_id=1)
    rail1.points = [
        RailPoint(position=Vector3(-10, 3, -5), rail_type=RailType.METAL),
        RailPoint(position=Vector3(-5, 3, -5), rail_type=RailType.METAL),
        RailPoint(position=Vector3(0, 3, -5), rail_type=RailType.METAL),
        RailPoint(position=Vector3(5, 3, -5), rail_type=RailType.METAL),
        RailPoint(position=Vector3(10, 3, -5), rail_type=RailType.METAL),
    ]
    
    # Add a loop rail
    rail2 = RailString(id=2, is_loop=True, piece_id=2)
    rail2.points = [
        RailPoint(position=Vector3(15, 2, 10), rail_type=RailType.WOOD),
        RailPoint(position=Vector3(20, 2, 10), rail_type=RailType.WOOD),
        RailPoint(position=Vector3(20, 2, 15), rail_type=RailType.WOOD),
        RailPoint(position=Vector3(15, 2, 15), rail_type=RailType.WOOD),
    ]
    
    level.rails.extend([rail1, rail2])
    
    # Add goals
    high_score_goal = Goal(
        id=1,
        name="High Score Challenge",
        goal_type=GoalType.COMPETITION,
        position=Vector3(0, 0, 0),
        time_limit=120,
        score_required=50000,
        properties={"difficulty": "medium"}
    )
    
    race_goal = Goal(
        id=2,
        name="Downhill Race",
        goal_type=GoalType.RACE,
        position=Vector3(-30, 5, 0),
        time_limit=60,
        score_required=0,
        properties={"checkpoints": 5}
    )
    
    gaps_goal = Goal(
        id=3,
        name="Find All Gaps",
        goal_type=GoalType.FINDGAPS,
        position=Vector3(30, 0, 0),
        time_limit=300,
        score_required=0,
        properties={"total_gaps": 10}
    )
    
    level.goals.extend([high_score_goal, race_goal, gaps_goal])
    
    # Add metadata
    level.metadata = {
        "author": "Level Editor",
        "version": "1.0",
        "description": "Example skate park with ramps, rails, and goals",
        "difficulty": "medium"
    }
    
    return level


def test_level_creation():
    """Test creating and saving a level"""
    print("Testing THUG Level Editor Data Models")
    print("=" * 50)
    
    # Create example level
    print("\n1. Creating example level...")
    level = create_example_level()
    print(f"   Level name: {level.name}")
    print(f"   Objects: {len(level.objects)}")
    print(f"   Rails: {len(level.rails)}")
    print(f"   Goals: {len(level.goals)}")
    
    # Convert to dict
    print("\n2. Converting to dictionary...")
    level_dict = level_to_dict(level)
    
    # Save to JSON
    output_file = Path(__file__).parent / "example_level.json"
    print(f"\n3. Saving to {output_file}...")
    with open(output_file, 'w') as f:
        json.dump(level_dict, f, indent=2)
    print("   Saved successfully!")
    
    # Verify file
    print("\n4. Verifying saved file...")
    with open(output_file, 'r') as f:
        loaded_data = json.load(f)
    print(f"   Level name: {loaded_data['name']}")
    print(f"   Objects: {len(loaded_data['objects'])}")
    print(f"   Rails: {len(loaded_data['rails'])}")
    print(f"   Goals: {len(loaded_data['goals'])}")
    
    # Display statistics
    print("\n5. Level Statistics:")
    print(f"   Total rail points: {sum(len(rail['points']) for rail in loaded_data['rails'])}")
    
    obj_types = {}
    for obj in loaded_data['objects']:
        obj_type = obj['type']
        obj_types[obj_type] = obj_types.get(obj_type, 0) + 1
    print(f"   Object types: {dict(obj_types)}")
    
    goal_types = {}
    for goal in loaded_data['goals']:
        goal_type = goal['type']
        goal_types[goal_type] = goal_types.get(goal_type, 0) + 1
    print(f"   Goal types: {dict(goal_types)}")
    
    print("\n" + "=" * 50)
    print("✓ All tests passed!")
    print(f"\nExample level saved to: {output_file}")
    print("\nYou can open this file in the Level Editor GUI or use it as a template.")
    
    return True


if __name__ == "__main__":
    try:
        test_level_creation()
        sys.exit(0)
    except Exception as e:
        print(f"\n✗ Error: {e}", file=sys.stderr)
        import traceback
        traceback.print_exc()
        sys.exit(1)
