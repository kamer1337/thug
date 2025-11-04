"""
THUG Level Editor - Data Models

Shared data models for the THUG level editor, based on the existing
ParkEditor2 system in the THUG codebase.

These models can be used by both the GUI editor and headless/testing scripts.
"""

from dataclasses import dataclass, field
from enum import Enum
from typing import Dict, List
import math


class ObjectType(Enum):
    """Types of objects in the level"""
    TRICKOBJECT = "TrickObject"
    MOVINGOBJECT = "MovingObject"
    GAMEOBJECT = "GameObject"
    VEHICLE = "Vehicle"
    PED = "Ped"
    PROP = "Prop"


class RailType(Enum):
    """Rail types from Code/Sk/parkeditor/EdRail.h"""
    METAL = "metal"
    WOOD = "wood"


class GoalType(Enum):
    """Goal types from Code/Sk/Modules/Skate/"""
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
    """3D Vector (based on Mth::Vector)"""
    x: float = 0.0
    y: float = 0.0
    z: float = 0.0

    def distance_to(self, other: 'Vector3') -> float:
        """Calculate distance to another vector"""
        return math.sqrt((self.x - other.x)**2 + (self.y - other.y)**2 + (self.z - other.z)**2)


@dataclass
class RailPoint:
    """Rail point data (based on Ed::RailPoint)"""
    position: Vector3 = field(default_factory=Vector3)
    rail_type: RailType = RailType.METAL
    object_id: int = 0


@dataclass
class RailString:
    """Rail string/path (based on Ed::RailString)"""
    id: int = 0
    points: List[RailPoint] = field(default_factory=list)
    is_loop: bool = False
    piece_id: int = 0


@dataclass
class GameObject:
    """Game object in the level"""
    id: int = 0
    name: str = ""
    object_type: ObjectType = ObjectType.PROP
    position: Vector3 = field(default_factory=Vector3)
    rotation: float = 0.0
    scale: Vector3 = field(default_factory=lambda: Vector3(1.0, 1.0, 1.0))
    properties: Dict = field(default_factory=dict)


@dataclass
class Goal:
    """Goal/objective in the level"""
    id: int = 0
    name: str = ""
    goal_type: GoalType = GoalType.COMPETITION
    position: Vector3 = field(default_factory=Vector3)
    time_limit: int = 120
    score_required: int = 0
    properties: Dict = field(default_factory=dict)


@dataclass
class Level:
    """Complete level data"""
    name: str = "Untitled Level"
    objects: List[GameObject] = field(default_factory=list)
    rails: List[RailString] = field(default_factory=list)
    goals: List[Goal] = field(default_factory=list)
    metadata: Dict = field(default_factory=dict)
