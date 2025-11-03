#!/usr/bin/env python3
"""
THUG Level Editor - Standalone Tool

A graphical level editor for Tony Hawk's Underground that allows:
- Object manipulation (add, edit, delete, move)
- Rail node creation and editing
- Goal placement and configuration
- Level data import/export

Based on the existing ParkEditor2 system in the THUG codebase.
"""

import tkinter as tk
from tkinter import ttk, messagebox, filedialog, scrolledtext
import json
import os
import math
from typing import Dict, List, Tuple, Optional
from dataclasses import dataclass, field, asdict
from enum import Enum


# ============================================================================
# Data Models (Based on THUG Code Structure)
# ============================================================================

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


# ============================================================================
# Level Editor Application
# ============================================================================

class LevelEditor:
    """Main level editor application"""

    def __init__(self, root: tk.Tk):
        self.root = root
        self.root.title("THUG Level Editor")
        self.root.geometry("1200x800")

        # Current level data
        self.level = Level()
        self.selected_object: Optional[GameObject] = None
        self.selected_rail: Optional[RailString] = None
        self.selected_goal: Optional[Goal] = None
        self.current_file: Optional[str] = None

        # ID counters
        self.next_object_id = 1
        self.next_rail_id = 1
        self.next_goal_id = 1

        # Setup UI
        self._create_menu()
        self._create_ui()
        self._update_title()

    def _create_menu(self):
        """Create menu bar"""
        menubar = tk.Menu(self.root)
        self.root.config(menu=menubar)

        # File menu
        file_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="File", menu=file_menu)
        file_menu.add_command(label="New", command=self.new_level, accelerator="Ctrl+N")
        file_menu.add_command(label="Open...", command=self.open_level, accelerator="Ctrl+O")
        file_menu.add_command(label="Save", command=self.save_level, accelerator="Ctrl+S")
        file_menu.add_command(label="Save As...", command=self.save_level_as, accelerator="Ctrl+Shift+S")
        file_menu.add_separator()
        file_menu.add_command(label="Exit", command=self.root.quit)

        # Edit menu
        edit_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="Edit", menu=edit_menu)
        edit_menu.add_command(label="Clear All", command=self.clear_all)

        # Help menu
        help_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="Help", menu=help_menu)
        help_menu.add_command(label="About", command=self.show_about)

        # Keyboard shortcuts
        self.root.bind('<Control-n>', lambda e: self.new_level())
        self.root.bind('<Control-o>', lambda e: self.open_level())
        self.root.bind('<Control-s>', lambda e: self.save_level())

    def _create_ui(self):
        """Create main UI layout"""
        # Main container with notebook (tabs)
        self.notebook = ttk.Notebook(self.root)
        self.notebook.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)

        # Create tabs
        self._create_objects_tab()
        self._create_rails_tab()
        self._create_goals_tab()
        self._create_level_info_tab()

        # Status bar
        self.status_bar = tk.Label(self.root, text="Ready", bd=1, relief=tk.SUNKEN, anchor=tk.W)
        self.status_bar.pack(side=tk.BOTTOM, fill=tk.X)

    def _create_objects_tab(self):
        """Create objects management tab"""
        frame = ttk.Frame(self.notebook)
        self.notebook.add(frame, text="Objects")

        # Left panel - object list
        left_frame = ttk.Frame(frame)
        left_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=5, pady=5)

        ttk.Label(left_frame, text="Objects in Level:", font=('Arial', 10, 'bold')).pack(anchor=tk.W)

        # Object list with scrollbar
        list_frame = ttk.Frame(left_frame)
        list_frame.pack(fill=tk.BOTH, expand=True, pady=5)

        scrollbar = ttk.Scrollbar(list_frame)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

        self.objects_list = tk.Listbox(list_frame, yscrollcommand=scrollbar.set)
        self.objects_list.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        scrollbar.config(command=self.objects_list.yview)
        self.objects_list.bind('<<ListboxSelect>>', self._on_object_select)

        # Buttons
        btn_frame = ttk.Frame(left_frame)
        btn_frame.pack(fill=tk.X, pady=5)

        ttk.Button(btn_frame, text="Add Object", command=self.add_object).pack(side=tk.LEFT, padx=2)
        ttk.Button(btn_frame, text="Delete", command=self.delete_object).pack(side=tk.LEFT, padx=2)

        # Right panel - object properties
        right_frame = ttk.Frame(frame)
        right_frame.pack(side=tk.RIGHT, fill=tk.BOTH, expand=True, padx=5, pady=5)

        ttk.Label(right_frame, text="Object Properties:", font=('Arial', 10, 'bold')).pack(anchor=tk.W)

        # Properties form
        props_frame = ttk.Frame(right_frame)
        props_frame.pack(fill=tk.BOTH, expand=True, pady=5)

        # Name
        ttk.Label(props_frame, text="Name:").grid(row=0, column=0, sticky=tk.W, pady=2)
        self.obj_name_var = tk.StringVar()
        ttk.Entry(props_frame, textvariable=self.obj_name_var, width=30).grid(row=0, column=1, pady=2, padx=5)

        # Type
        ttk.Label(props_frame, text="Type:").grid(row=1, column=0, sticky=tk.W, pady=2)
        self.obj_type_var = tk.StringVar()
        ttk.Combobox(props_frame, textvariable=self.obj_type_var, 
                     values=[t.value for t in ObjectType], state='readonly', width=28).grid(row=1, column=1, pady=2, padx=5)

        # Position
        ttk.Label(props_frame, text="Position X:").grid(row=2, column=0, sticky=tk.W, pady=2)
        self.obj_pos_x_var = tk.DoubleVar()
        ttk.Entry(props_frame, textvariable=self.obj_pos_x_var, width=30).grid(row=2, column=1, pady=2, padx=5)

        ttk.Label(props_frame, text="Position Y:").grid(row=3, column=0, sticky=tk.W, pady=2)
        self.obj_pos_y_var = tk.DoubleVar()
        ttk.Entry(props_frame, textvariable=self.obj_pos_y_var, width=30).grid(row=3, column=1, pady=2, padx=5)

        ttk.Label(props_frame, text="Position Z:").grid(row=4, column=0, sticky=tk.W, pady=2)
        self.obj_pos_z_var = tk.DoubleVar()
        ttk.Entry(props_frame, textvariable=self.obj_pos_z_var, width=30).grid(row=4, column=1, pady=2, padx=5)

        # Rotation
        ttk.Label(props_frame, text="Rotation (degrees):").grid(row=5, column=0, sticky=tk.W, pady=2)
        self.obj_rotation_var = tk.DoubleVar()
        ttk.Entry(props_frame, textvariable=self.obj_rotation_var, width=30).grid(row=5, column=1, pady=2, padx=5)

        # Update button
        ttk.Button(props_frame, text="Update Object", command=self.update_object).grid(row=6, column=0, columnspan=2, pady=10)

    def _create_rails_tab(self):
        """Create rails management tab"""
        frame = ttk.Frame(self.notebook)
        self.notebook.add(frame, text="Rails")

        # Left panel - rail list
        left_frame = ttk.Frame(frame)
        left_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=5, pady=5)

        ttk.Label(left_frame, text="Rail Strings:", font=('Arial', 10, 'bold')).pack(anchor=tk.W)

        # Rail list
        list_frame = ttk.Frame(left_frame)
        list_frame.pack(fill=tk.BOTH, expand=True, pady=5)

        scrollbar = ttk.Scrollbar(list_frame)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

        self.rails_list = tk.Listbox(list_frame, yscrollcommand=scrollbar.set)
        self.rails_list.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        scrollbar.config(command=self.rails_list.yview)
        self.rails_list.bind('<<ListboxSelect>>', self._on_rail_select)

        # Buttons
        btn_frame = ttk.Frame(left_frame)
        btn_frame.pack(fill=tk.X, pady=5)

        ttk.Button(btn_frame, text="Add Rail", command=self.add_rail).pack(side=tk.LEFT, padx=2)
        ttk.Button(btn_frame, text="Delete", command=self.delete_rail).pack(side=tk.LEFT, padx=2)

        # Right panel - rail properties
        right_frame = ttk.Frame(frame)
        right_frame.pack(side=tk.RIGHT, fill=tk.BOTH, expand=True, padx=5, pady=5)

        ttk.Label(right_frame, text="Rail Properties:", font=('Arial', 10, 'bold')).pack(anchor=tk.W)

        props_frame = ttk.Frame(right_frame)
        props_frame.pack(fill=tk.BOTH, expand=True, pady=5)

        # Is Loop
        self.rail_loop_var = tk.BooleanVar()
        ttk.Checkbutton(props_frame, text="Is Loop", variable=self.rail_loop_var).grid(row=0, column=0, columnspan=2, pady=5, sticky=tk.W)

        ttk.Label(props_frame, text="Points:", font=('Arial', 9, 'bold')).grid(row=1, column=0, columnspan=2, pady=5, sticky=tk.W)

        # Points list
        points_list_frame = ttk.Frame(props_frame)
        points_list_frame.grid(row=2, column=0, columnspan=2, pady=5, sticky=(tk.W, tk.E, tk.N, tk.S))

        points_scrollbar = ttk.Scrollbar(points_list_frame)
        points_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

        self.rail_points_list = tk.Listbox(points_list_frame, yscrollcommand=points_scrollbar.set, height=10)
        self.rail_points_list.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        points_scrollbar.config(command=self.rail_points_list.yview)

        # Point buttons
        point_btn_frame = ttk.Frame(props_frame)
        point_btn_frame.grid(row=3, column=0, columnspan=2, pady=5)

        ttk.Button(point_btn_frame, text="Add Point", command=self.add_rail_point).pack(side=tk.LEFT, padx=2)
        ttk.Button(point_btn_frame, text="Remove Point", command=self.remove_rail_point).pack(side=tk.LEFT, padx=2)

        # Update button
        ttk.Button(props_frame, text="Update Rail", command=self.update_rail).grid(row=4, column=0, columnspan=2, pady=10)

    def _create_goals_tab(self):
        """Create goals management tab"""
        frame = ttk.Frame(self.notebook)
        self.notebook.add(frame, text="Goals")

        # Left panel - goal list
        left_frame = ttk.Frame(frame)
        left_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=5, pady=5)

        ttk.Label(left_frame, text="Goals in Level:", font=('Arial', 10, 'bold')).pack(anchor=tk.W)

        # Goal list
        list_frame = ttk.Frame(left_frame)
        list_frame.pack(fill=tk.BOTH, expand=True, pady=5)

        scrollbar = ttk.Scrollbar(list_frame)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

        self.goals_list = tk.Listbox(list_frame, yscrollcommand=scrollbar.set)
        self.goals_list.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        scrollbar.config(command=self.goals_list.yview)
        self.goals_list.bind('<<ListboxSelect>>', self._on_goal_select)

        # Buttons
        btn_frame = ttk.Frame(left_frame)
        btn_frame.pack(fill=tk.X, pady=5)

        ttk.Button(btn_frame, text="Add Goal", command=self.add_goal).pack(side=tk.LEFT, padx=2)
        ttk.Button(btn_frame, text="Delete", command=self.delete_goal).pack(side=tk.LEFT, padx=2)

        # Right panel - goal properties
        right_frame = ttk.Frame(frame)
        right_frame.pack(side=tk.RIGHT, fill=tk.BOTH, expand=True, padx=5, pady=5)

        ttk.Label(right_frame, text="Goal Properties:", font=('Arial', 10, 'bold')).pack(anchor=tk.W)

        props_frame = ttk.Frame(right_frame)
        props_frame.pack(fill=tk.BOTH, expand=True, pady=5)

        # Name
        ttk.Label(props_frame, text="Name:").grid(row=0, column=0, sticky=tk.W, pady=2)
        self.goal_name_var = tk.StringVar()
        ttk.Entry(props_frame, textvariable=self.goal_name_var, width=30).grid(row=0, column=1, pady=2, padx=5)

        # Type
        ttk.Label(props_frame, text="Type:").grid(row=1, column=0, sticky=tk.W, pady=2)
        self.goal_type_var = tk.StringVar()
        ttk.Combobox(props_frame, textvariable=self.goal_type_var, 
                     values=[t.value for t in GoalType], state='readonly', width=28).grid(row=1, column=1, pady=2, padx=5)

        # Position
        ttk.Label(props_frame, text="Position X:").grid(row=2, column=0, sticky=tk.W, pady=2)
        self.goal_pos_x_var = tk.DoubleVar()
        ttk.Entry(props_frame, textvariable=self.goal_pos_x_var, width=30).grid(row=2, column=1, pady=2, padx=5)

        ttk.Label(props_frame, text="Position Y:").grid(row=3, column=0, sticky=tk.W, pady=2)
        self.goal_pos_y_var = tk.DoubleVar()
        ttk.Entry(props_frame, textvariable=self.goal_pos_y_var, width=30).grid(row=3, column=1, pady=2, padx=5)

        ttk.Label(props_frame, text="Position Z:").grid(row=4, column=0, sticky=tk.W, pady=2)
        self.goal_pos_z_var = tk.DoubleVar()
        ttk.Entry(props_frame, textvariable=self.goal_pos_z_var, width=30).grid(row=4, column=1, pady=2, padx=5)

        # Time limit
        ttk.Label(props_frame, text="Time Limit (sec):").grid(row=5, column=0, sticky=tk.W, pady=2)
        self.goal_time_var = tk.IntVar(value=120)
        ttk.Entry(props_frame, textvariable=self.goal_time_var, width=30).grid(row=5, column=1, pady=2, padx=5)

        # Score required
        ttk.Label(props_frame, text="Score Required:").grid(row=6, column=0, sticky=tk.W, pady=2)
        self.goal_score_var = tk.IntVar(value=0)
        ttk.Entry(props_frame, textvariable=self.goal_score_var, width=30).grid(row=6, column=1, pady=2, padx=5)

        # Update button
        ttk.Button(props_frame, text="Update Goal", command=self.update_goal).grid(row=7, column=0, columnspan=2, pady=10)

    def _create_level_info_tab(self):
        """Create level information tab"""
        frame = ttk.Frame(self.notebook)
        self.notebook.add(frame, text="Level Info")

        # Level properties
        props_frame = ttk.LabelFrame(frame, text="Level Properties", padding=10)
        props_frame.pack(fill=tk.X, padx=10, pady=10)

        ttk.Label(props_frame, text="Level Name:").grid(row=0, column=0, sticky=tk.W, pady=5)
        self.level_name_var = tk.StringVar(value="Untitled Level")
        ttk.Entry(props_frame, textvariable=self.level_name_var, width=40).grid(row=0, column=1, pady=5, padx=5)

        ttk.Button(props_frame, text="Update Level Name", command=self.update_level_name).grid(row=1, column=0, columnspan=2, pady=5)

        # Statistics
        stats_frame = ttk.LabelFrame(frame, text="Statistics", padding=10)
        stats_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

        self.stats_text = scrolledtext.ScrolledText(stats_frame, height=15, state='disabled')
        self.stats_text.pack(fill=tk.BOTH, expand=True)

        ttk.Button(stats_frame, text="Refresh Statistics", command=self.refresh_statistics).pack(pady=5)

    # ========================================================================
    # Object Management
    # ========================================================================

    def add_object(self):
        """Add a new object to the level"""
        obj = GameObject(
            id=self.next_object_id,
            name=f"Object_{self.next_object_id}",
            object_type=ObjectType.PROP,
            position=Vector3(0, 0, 0)
        )
        self.next_object_id += 1
        self.level.objects.append(obj)
        self._refresh_objects_list()
        self._set_status(f"Added object: {obj.name}")

    def delete_object(self):
        """Delete selected object"""
        if self.selected_object:
            self.level.objects.remove(self.selected_object)
            self._refresh_objects_list()
            self.selected_object = None
            self._clear_object_form()
            self._set_status("Object deleted")

    def update_object(self):
        """Update selected object with form values"""
        if self.selected_object:
            self.selected_object.name = self.obj_name_var.get()
            self.selected_object.object_type = ObjectType(self.obj_type_var.get())
            self.selected_object.position.x = self.obj_pos_x_var.get()
            self.selected_object.position.y = self.obj_pos_y_var.get()
            self.selected_object.position.z = self.obj_pos_z_var.get()
            self.selected_object.rotation = self.obj_rotation_var.get()
            self._refresh_objects_list()
            self._set_status(f"Updated object: {self.selected_object.name}")

    def _on_object_select(self, event):
        """Handle object selection from list"""
        selection = self.objects_list.curselection()
        if selection:
            idx = selection[0]
            self.selected_object = self.level.objects[idx]
            self._load_object_form(self.selected_object)

    def _load_object_form(self, obj: GameObject):
        """Load object data into form"""
        self.obj_name_var.set(obj.name)
        self.obj_type_var.set(obj.object_type.value)
        self.obj_pos_x_var.set(obj.position.x)
        self.obj_pos_y_var.set(obj.position.y)
        self.obj_pos_z_var.set(obj.position.z)
        self.obj_rotation_var.set(obj.rotation)

    def _clear_object_form(self):
        """Clear object form"""
        self.obj_name_var.set("")
        self.obj_type_var.set("")
        self.obj_pos_x_var.set(0.0)
        self.obj_pos_y_var.set(0.0)
        self.obj_pos_z_var.set(0.0)
        self.obj_rotation_var.set(0.0)

    def _refresh_objects_list(self):
        """Refresh objects listbox"""
        self.objects_list.delete(0, tk.END)
        for obj in self.level.objects:
            self.objects_list.insert(tk.END, f"{obj.id}: {obj.name} ({obj.object_type.value})")

    # ========================================================================
    # Rail Management
    # ========================================================================

    def add_rail(self):
        """Add a new rail string"""
        rail = RailString(
            id=self.next_rail_id,
            is_loop=False,
            points=[]
        )
        self.next_rail_id += 1
        self.level.rails.append(rail)
        self._refresh_rails_list()
        self._set_status(f"Added rail {rail.id}")

    def delete_rail(self):
        """Delete selected rail"""
        if self.selected_rail:
            self.level.rails.remove(self.selected_rail)
            self._refresh_rails_list()
            self.selected_rail = None
            self._set_status("Rail deleted")

    def add_rail_point(self):
        """Add point to selected rail"""
        if self.selected_rail:
            point = RailPoint(position=Vector3(0, 0, 0), rail_type=RailType.METAL)
            self.selected_rail.points.append(point)
            self._refresh_rail_points_list()
            self._set_status("Added rail point")

    def remove_rail_point(self):
        """Remove selected point from rail"""
        if self.selected_rail:
            selection = self.rail_points_list.curselection()
            if selection:
                idx = selection[0]
                self.selected_rail.points.pop(idx)
                self._refresh_rail_points_list()
                self._set_status("Removed rail point")

    def update_rail(self):
        """Update selected rail with form values"""
        if self.selected_rail:
            self.selected_rail.is_loop = self.rail_loop_var.get()
            self._refresh_rails_list()
            self._set_status(f"Updated rail {self.selected_rail.id}")

    def _on_rail_select(self, event):
        """Handle rail selection from list"""
        selection = self.rails_list.curselection()
        if selection:
            idx = selection[0]
            self.selected_rail = self.level.rails[idx]
            self._load_rail_form(self.selected_rail)

    def _load_rail_form(self, rail: RailString):
        """Load rail data into form"""
        self.rail_loop_var.set(rail.is_loop)
        self._refresh_rail_points_list()

    def _refresh_rails_list(self):
        """Refresh rails listbox"""
        self.rails_list.delete(0, tk.END)
        for rail in self.level.rails:
            loop_str = "Loop" if rail.is_loop else "Path"
            self.rails_list.insert(tk.END, f"Rail {rail.id}: {len(rail.points)} points ({loop_str})")

    def _refresh_rail_points_list(self):
        """Refresh rail points listbox"""
        self.rail_points_list.delete(0, tk.END)
        if self.selected_rail:
            for i, point in enumerate(self.selected_rail.points):
                pos = point.position
                self.rail_points_list.insert(tk.END, 
                    f"Point {i}: ({pos.x:.1f}, {pos.y:.1f}, {pos.z:.1f}) - {point.rail_type.value}")

    # ========================================================================
    # Goal Management
    # ========================================================================

    def add_goal(self):
        """Add a new goal"""
        goal = Goal(
            id=self.next_goal_id,
            name=f"Goal_{self.next_goal_id}",
            goal_type=GoalType.COMPETITION,
            position=Vector3(0, 0, 0),
            time_limit=120,
            score_required=0
        )
        self.next_goal_id += 1
        self.level.goals.append(goal)
        self._refresh_goals_list()
        self._set_status(f"Added goal: {goal.name}")

    def delete_goal(self):
        """Delete selected goal"""
        if self.selected_goal:
            self.level.goals.remove(self.selected_goal)
            self._refresh_goals_list()
            self.selected_goal = None
            self._clear_goal_form()
            self._set_status("Goal deleted")

    def update_goal(self):
        """Update selected goal with form values"""
        if self.selected_goal:
            self.selected_goal.name = self.goal_name_var.get()
            self.selected_goal.goal_type = GoalType(self.goal_type_var.get())
            self.selected_goal.position.x = self.goal_pos_x_var.get()
            self.selected_goal.position.y = self.goal_pos_y_var.get()
            self.selected_goal.position.z = self.goal_pos_z_var.get()
            self.selected_goal.time_limit = self.goal_time_var.get()
            self.selected_goal.score_required = self.goal_score_var.get()
            self._refresh_goals_list()
            self._set_status(f"Updated goal: {self.selected_goal.name}")

    def _on_goal_select(self, event):
        """Handle goal selection from list"""
        selection = self.goals_list.curselection()
        if selection:
            idx = selection[0]
            self.selected_goal = self.level.goals[idx]
            self._load_goal_form(self.selected_goal)

    def _load_goal_form(self, goal: Goal):
        """Load goal data into form"""
        self.goal_name_var.set(goal.name)
        self.goal_type_var.set(goal.goal_type.value)
        self.goal_pos_x_var.set(goal.position.x)
        self.goal_pos_y_var.set(goal.position.y)
        self.goal_pos_z_var.set(goal.position.z)
        self.goal_time_var.set(goal.time_limit)
        self.goal_score_var.set(goal.score_required)

    def _clear_goal_form(self):
        """Clear goal form"""
        self.goal_name_var.set("")
        self.goal_type_var.set("")
        self.goal_pos_x_var.set(0.0)
        self.goal_pos_y_var.set(0.0)
        self.goal_pos_z_var.set(0.0)
        self.goal_time_var.set(120)
        self.goal_score_var.set(0)

    def _refresh_goals_list(self):
        """Refresh goals listbox"""
        self.goals_list.delete(0, tk.END)
        for goal in self.level.goals:
            self.goals_list.insert(tk.END, f"{goal.id}: {goal.name} ({goal.goal_type.value})")

    # ========================================================================
    # Level Management
    # ========================================================================

    def new_level(self):
        """Create a new level"""
        if messagebox.askyesno("New Level", "Create a new level? Unsaved changes will be lost."):
            self.level = Level()
            self.current_file = None
            self.next_object_id = 1
            self.next_rail_id = 1
            self.next_goal_id = 1
            self._refresh_all_lists()
            self.level_name_var.set("Untitled Level")
            self._update_title()
            self._set_status("New level created")

    def open_level(self):
        """Open a level from file"""
        filename = filedialog.askopenfilename(
            title="Open Level",
            filetypes=[("JSON files", "*.json"), ("All files", "*.*")]
        )
        if filename:
            try:
                with open(filename, 'r') as f:
                    data = json.load(f)
                self._load_level_from_dict(data)
                self.current_file = filename
                self._update_title()
                self._set_status(f"Loaded level from {filename}")
            except Exception as e:
                messagebox.showerror("Error", f"Failed to load level: {str(e)}")

    def save_level(self):
        """Save level to current file or prompt for new file"""
        if self.current_file:
            self._save_to_file(self.current_file)
        else:
            self.save_level_as()

    def save_level_as(self):
        """Save level to a new file"""
        filename = filedialog.asksaveasfilename(
            title="Save Level As",
            defaultextension=".json",
            filetypes=[("JSON files", "*.json"), ("All files", "*.*")]
        )
        if filename:
            self._save_to_file(filename)
            self.current_file = filename
            self._update_title()

    def _save_to_file(self, filename: str):
        """Save level data to file"""
        try:
            data = self._level_to_dict()
            with open(filename, 'w') as f:
                json.dump(data, f, indent=2)
            self._set_status(f"Saved level to {filename}")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to save level: {str(e)}")

    def update_level_name(self):
        """Update level name"""
        self.level.name = self.level_name_var.get()
        self._update_title()
        self._set_status(f"Level name updated to: {self.level.name}")

    def clear_all(self):
        """Clear all level data"""
        if messagebox.askyesno("Clear All", "Clear all objects, rails, and goals?"):
            self.level.objects.clear()
            self.level.rails.clear()
            self.level.goals.clear()
            self._refresh_all_lists()
            self._set_status("Level cleared")

    def refresh_statistics(self):
        """Refresh level statistics"""
        stats = []
        stats.append(f"Level: {self.level.name}\n")
        stats.append("=" * 50 + "\n\n")
        
        stats.append(f"Objects: {len(self.level.objects)}\n")
        obj_types = {}
        for obj in self.level.objects:
            obj_types[obj.object_type.value] = obj_types.get(obj.object_type.value, 0) + 1
        for obj_type, count in sorted(obj_types.items()):
            stats.append(f"  {obj_type}: {count}\n")
        
        stats.append(f"\nRails: {len(self.level.rails)}\n")
        total_points = sum(len(rail.points) for rail in self.level.rails)
        stats.append(f"  Total rail points: {total_points}\n")
        loop_count = sum(1 for rail in self.level.rails if rail.is_loop)
        stats.append(f"  Loops: {loop_count}\n")
        stats.append(f"  Paths: {len(self.level.rails) - loop_count}\n")
        
        stats.append(f"\nGoals: {len(self.level.goals)}\n")
        goal_types = {}
        for goal in self.level.goals:
            goal_types[goal.goal_type.value] = goal_types.get(goal.goal_type.value, 0) + 1
        for goal_type, count in sorted(goal_types.items()):
            stats.append(f"  {goal_type}: {count}\n")
        
        self.stats_text.config(state='normal')
        self.stats_text.delete(1.0, tk.END)
        self.stats_text.insert(1.0, ''.join(stats))
        self.stats_text.config(state='disabled')

    # ========================================================================
    # Data Serialization
    # ========================================================================

    def _level_to_dict(self) -> dict:
        """Convert level to dictionary for JSON serialization"""
        return {
            'name': self.level.name,
            'objects': [self._object_to_dict(obj) for obj in self.level.objects],
            'rails': [self._rail_to_dict(rail) for rail in self.level.rails],
            'goals': [self._goal_to_dict(goal) for goal in self.level.goals],
            'metadata': self.level.metadata
        }

    def _object_to_dict(self, obj: GameObject) -> dict:
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

    def _rail_to_dict(self, rail: RailString) -> dict:
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

    def _goal_to_dict(self, goal: Goal) -> dict:
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

    def _load_level_from_dict(self, data: dict):
        """Load level from dictionary"""
        self.level = Level()
        self.level.name = data.get('name', 'Untitled Level')
        self.level.metadata = data.get('metadata', {})

        # Load objects
        for obj_data in data.get('objects', []):
            obj = GameObject(
                id=obj_data['id'],
                name=obj_data['name'],
                object_type=ObjectType(obj_data['type']),
                position=Vector3(**obj_data['position']),
                rotation=obj_data['rotation'],
                scale=Vector3(**obj_data['scale']),
                properties=obj_data.get('properties', {})
            )
            self.level.objects.append(obj)
            self.next_object_id = max(self.next_object_id, obj.id + 1)

        # Load rails
        for rail_data in data.get('rails', []):
            rail = RailString(
                id=rail_data['id'],
                is_loop=rail_data['is_loop'],
                piece_id=rail_data.get('piece_id', 0)
            )
            for point_data in rail_data['points']:
                point = RailPoint(
                    position=Vector3(**point_data['position']),
                    rail_type=RailType(point_data['type']),
                    object_id=point_data.get('object_id', 0)
                )
                rail.points.append(point)
            self.level.rails.append(rail)
            self.next_rail_id = max(self.next_rail_id, rail.id + 1)

        # Load goals
        for goal_data in data.get('goals', []):
            goal = Goal(
                id=goal_data['id'],
                name=goal_data['name'],
                goal_type=GoalType(goal_data['type']),
                position=Vector3(**goal_data['position']),
                time_limit=goal_data['time_limit'],
                score_required=goal_data['score_required'],
                properties=goal_data.get('properties', {})
            )
            self.level.goals.append(goal)
            self.next_goal_id = max(self.next_goal_id, goal.id + 1)

        # Refresh UI
        self._refresh_all_lists()
        self.level_name_var.set(self.level.name)

    # ========================================================================
    # UI Helpers
    # ========================================================================

    def _refresh_all_lists(self):
        """Refresh all list boxes"""
        self._refresh_objects_list()
        self._refresh_rails_list()
        self._refresh_goals_list()

    def _update_title(self):
        """Update window title"""
        filename = os.path.basename(self.current_file) if self.current_file else "Untitled"
        self.root.title(f"THUG Level Editor - {filename}")

    def _set_status(self, message: str):
        """Set status bar message"""
        self.status_bar.config(text=message)

    def show_about(self):
        """Show about dialog"""
        messagebox.showinfo(
            "About THUG Level Editor",
            "THUG Level Editor v1.0\n\n"
            "A standalone level editor for Tony Hawk's Underground.\n\n"
            "Features:\n"
            "- Object manipulation\n"
            "- Rail node creation\n"
            "- Goal placement\n"
            "- Level data import/export\n\n"
            "Based on the THUG codebase (Code/Sk/ParkEditor2)"
        )


# ============================================================================
# Main Entry Point
# ============================================================================

def main():
    """Main entry point"""
    root = tk.Tk()
    app = LevelEditor(root)
    root.mainloop()


if __name__ == "__main__":
    main()
