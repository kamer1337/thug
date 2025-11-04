"""
Main window for the THUG model viewer application.
"""

from PyQt5.QtWidgets import (
    QMainWindow, QWidget, QVBoxLayout, QHBoxLayout, QSplitter,
    QMenuBar, QMenu, QAction, QFileDialog, QMessageBox, QStatusBar
)
from PyQt5.QtCore import Qt, QTimer
from typing import Optional

from .viewport import Viewport
from .animation_panel import AnimationPanel
from .skeleton_tree import SkeletonTree
from .properties_panel import PropertiesPanel
from core.model_loader import ModelLoader, Model
from core.skeleton import Skeleton
from core.animation import Animation


class MainWindow(QMainWindow):
    """Main application window."""
    
    def __init__(self):
        super().__init__()
        
        self.model: Optional[Model] = None
        self.skeleton: Optional[Skeleton] = None
        self.animation: Optional[Animation] = None
        
        self._setup_ui()
        self._create_menus()
        self._connect_signals()
        
        # Timer for animation updates
        self.update_timer = QTimer()
        self.update_timer.timeout.connect(self._update_animation)
        self.update_timer.start(16)  # ~60 FPS
        
        self.setWindowTitle("THUG Model Viewer & Skeleton/Animation Editor")
        self.resize(1280, 720)
    
    def _setup_ui(self):
        """Set up the user interface."""
        # Create central widget
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        
        # Main layout
        main_layout = QHBoxLayout(central_widget)
        
        # Create splitter for resizable panels
        self.splitter = QSplitter(Qt.Horizontal)
        
        # Left panel: Skeleton tree
        self.skeleton_tree = SkeletonTree()
        self.splitter.addWidget(self.skeleton_tree)
        
        # Center: Viewport
        self.viewport = Viewport()
        self.splitter.addWidget(self.viewport)
        
        # Right panel: Contains animation controls and properties
        right_panel = QWidget()
        right_layout = QVBoxLayout(right_panel)
        
        self.animation_panel = AnimationPanel()
        self.properties_panel = PropertiesPanel()
        
        right_layout.addWidget(self.animation_panel)
        right_layout.addWidget(self.properties_panel)
        
        self.splitter.addWidget(right_panel)
        
        # Set splitter sizes (left: 20%, center: 60%, right: 20%)
        self.splitter.setSizes([250, 780, 250])
        
        main_layout.addWidget(self.splitter)
        
        # Status bar
        self.status_bar = QStatusBar()
        self.setStatusBar(self.status_bar)
        self.status_bar.showMessage("Ready")
    
    def _create_menus(self):
        """Create menu bar and menus."""
        menubar = self.menuBar()
        
        # File menu
        file_menu = menubar.addMenu("&File")
        
        open_model_action = QAction("Open &Model...", self)
        open_model_action.setShortcut("Ctrl+O")
        open_model_action.triggered.connect(self._open_model_dialog)
        file_menu.addAction(open_model_action)
        
        open_skeleton_action = QAction("Open &Skeleton...", self)
        open_skeleton_action.triggered.connect(self._open_skeleton_dialog)
        file_menu.addAction(open_skeleton_action)
        
        open_anim_action = QAction("Open &Animation...", self)
        open_anim_action.setShortcut("Ctrl+A")
        open_anim_action.triggered.connect(self._open_animation_dialog)
        file_menu.addAction(open_anim_action)
        
        file_menu.addSeparator()
        
        exit_action = QAction("E&xit", self)
        exit_action.setShortcut("Ctrl+Q")
        exit_action.triggered.connect(self.close)
        file_menu.addAction(exit_action)
        
        # View menu
        view_menu = menubar.addMenu("&View")
        
        wireframe_action = QAction("&Wireframe Mode", self)
        wireframe_action.setShortcut("W")
        wireframe_action.setCheckable(True)
        wireframe_action.triggered.connect(self._toggle_wireframe)
        view_menu.addAction(wireframe_action)
        
        skeleton_action = QAction("Show &Skeleton", self)
        skeleton_action.setShortcut("S")
        skeleton_action.setCheckable(True)
        skeleton_action.setChecked(True)
        skeleton_action.triggered.connect(self._toggle_skeleton)
        view_menu.addAction(skeleton_action)
        
        grid_action = QAction("Show &Grid", self)
        grid_action.setShortcut("G")
        grid_action.setCheckable(True)
        grid_action.setChecked(True)
        grid_action.triggered.connect(self._toggle_grid)
        view_menu.addAction(grid_action)
        
        view_menu.addSeparator()
        
        reset_camera_action = QAction("&Reset Camera", self)
        reset_camera_action.setShortcut("R")
        reset_camera_action.triggered.connect(self.viewport.reset_camera)
        view_menu.addAction(reset_camera_action)
        
        # Help menu
        help_menu = menubar.addMenu("&Help")
        
        about_action = QAction("&About", self)
        about_action.triggered.connect(self._show_about)
        help_menu.addAction(about_action)
    
    def _connect_signals(self):
        """Connect signals between components."""
        # Skeleton tree selection
        self.skeleton_tree.bone_selected.connect(self._on_bone_selected)
        
        # Animation panel controls
        self.animation_panel.play_requested.connect(self._play_animation)
        self.animation_panel.pause_requested.connect(self._pause_animation)
        self.animation_panel.stop_requested.connect(self._stop_animation)
        self.animation_panel.frame_changed.connect(self._set_animation_frame)
        self.animation_panel.speed_changed.connect(self._set_animation_speed)
    
    def _update_animation(self):
        """Update animation (called by timer)."""
        if self.animation and self.skeleton:
            # Update animation
            self.animation.update(0.016)  # ~60 FPS
            
            # Apply to skeleton
            self.animation.apply_to_skeleton(self.skeleton)
            
            # Update UI
            self.animation_panel.update_current_frame(self.animation.current_frame)
            
            # Update viewport
            self.viewport.update()
    
    def load_model(self, filepath: str):
        """Load a model from file."""
        try:
            self.model = ModelLoader.load_model(filepath)
            if self.model:
                self.viewport.set_model(self.model)
                self.status_bar.showMessage(f"Loaded model: {filepath}")
            else:
                self.status_bar.showMessage(f"Failed to load model: {filepath}")
        except Exception as e:
            QMessageBox.critical(self, "Error", f"Failed to load model: {str(e)}")
    
    def load_skeleton(self, filepath: str):
        """Load a skeleton from file."""
        try:
            # For now, create a test skeleton
            self.skeleton = ModelLoader.create_test_skeleton()
            self.viewport.set_skeleton(self.skeleton)
            self.skeleton_tree.set_skeleton(self.skeleton)
            self.status_bar.showMessage(f"Loaded skeleton: {filepath}")
        except Exception as e:
            QMessageBox.critical(self, "Error", f"Failed to load skeleton: {str(e)}")
    
    def load_animation(self, filepath: str):
        """Load an animation from file."""
        try:
            # TODO: Implement animation loading
            self.status_bar.showMessage(f"Animation loading not yet implemented: {filepath}")
        except Exception as e:
            QMessageBox.critical(self, "Error", f"Failed to load animation: {str(e)}")
    
    def set_wireframe_mode(self, enabled: bool):
        """Set wireframe rendering mode."""
        self.viewport.set_wireframe_mode(enabled)
    
    def _open_model_dialog(self):
        """Open file dialog to select a model."""
        filepath, _ = QFileDialog.getOpenFileName(
            self,
            "Open Model",
            "",
            "Model Files (*.mdl);;All Files (*)"
        )
        if filepath:
            self.load_model(filepath)
    
    def _open_skeleton_dialog(self):
        """Open file dialog to select a skeleton."""
        filepath, _ = QFileDialog.getOpenFileName(
            self,
            "Open Skeleton",
            "",
            "Skeleton Files (*.ska);;All Files (*)"
        )
        if filepath:
            self.load_skeleton(filepath)
    
    def _open_animation_dialog(self):
        """Open file dialog to select an animation."""
        filepath, _ = QFileDialog.getOpenFileName(
            self,
            "Open Animation",
            "",
            "Animation Files (*.ska);;All Files (*)"
        )
        if filepath:
            self.load_animation(filepath)
    
    def _toggle_wireframe(self, checked: bool):
        """Toggle wireframe mode."""
        self.viewport.set_wireframe_mode(checked)
    
    def _toggle_skeleton(self, checked: bool):
        """Toggle skeleton visibility."""
        self.viewport.set_skeleton_visible(checked)
    
    def _toggle_grid(self, checked: bool):
        """Toggle grid visibility."""
        self.viewport.set_grid_visible(checked)
    
    def _on_bone_selected(self, bone_name: str):
        """Handle bone selection from tree."""
        if self.skeleton:
            bone = self.skeleton.get_bone_by_name(bone_name)
            if bone:
                self.properties_panel.set_bone(bone)
                self.viewport.set_selected_bone(bone)
    
    def _play_animation(self):
        """Play animation."""
        if self.animation:
            self.animation.play()
    
    def _pause_animation(self):
        """Pause animation."""
        if self.animation:
            self.animation.pause()
    
    def _stop_animation(self):
        """Stop animation."""
        if self.animation:
            self.animation.stop()
    
    def _set_animation_frame(self, frame: float):
        """Set animation to specific frame."""
        if self.animation:
            self.animation.set_frame(frame)
    
    def _set_animation_speed(self, speed: float):
        """Set animation playback speed."""
        if self.animation:
            self.animation.playback_speed = speed
    
    def _show_about(self):
        """Show about dialog."""
        QMessageBox.about(
            self,
            "About THUG Model Viewer",
            "THUG 3D Model Viewer & Skeleton/Animation Editor\n\n"
            "A standalone tool for viewing and editing character models,\n"
            "skeletons, and animations from Tony Hawk's Underground.\n\n"
            "Part of the THUG source code preservation project."
        )
