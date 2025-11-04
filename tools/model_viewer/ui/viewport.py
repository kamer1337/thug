"""
OpenGL viewport widget for 3D rendering.
"""

from PyQt5.QtWidgets import QOpenGLWidget
from PyQt5.QtCore import Qt, QTimer, pyqtSignal
from PyQt5.QtGui import QMouseEvent, QWheelEvent
from OpenGL.GL import *
from typing import Optional

from core.renderer import Renderer
from core.camera import Camera
from core.model_loader import Model
from core.skeleton import Skeleton, Bone


class Viewport(QOpenGLWidget):
    """OpenGL viewport widget for rendering 3D content."""
    
    def __init__(self, parent=None):
        super().__init__(parent)
        
        self.renderer = Renderer()
        self.camera = Camera()
        
        self.model: Optional[Model] = None
        self.skeleton: Optional[Skeleton] = None
        self.selected_bone: Optional[Bone] = None
        
        # Mouse interaction
        self.last_mouse_x = 0
        self.last_mouse_y = 0
        self.mouse_button = Qt.NoButton
        
        # Set focus policy to receive keyboard events
        self.setFocusPolicy(Qt.StrongFocus)
        
        # Enable mouse tracking
        self.setMouseTracking(True)
    
    def initializeGL(self):
        """Initialize OpenGL context."""
        self.renderer.initialize()
    
    def resizeGL(self, width: int, height: int):
        """Handle viewport resize."""
        pass  # Handled in paintGL
    
    def paintGL(self):
        """Render the scene."""
        # Clear viewport
        self.renderer.clear()
        
        # Set up viewport and camera
        self.renderer.setup_viewport(self.width(), self.height(), self.camera)
        
        # Render grid
        self.renderer.render_grid()
        
        # Render coordinate axes
        self.renderer.render_axes(2.0)
        
        # Render model
        if self.model:
            self.renderer.render_model(self.model)
        
        # Render skeleton
        if self.skeleton:
            self.renderer.render_skeleton(self.skeleton)
        
        # Render selected bone with highlight
        if self.selected_bone:
            self.renderer.render_bone(self.selected_bone, highlight=True)
    
    def mousePressEvent(self, event: QMouseEvent):
        """Handle mouse press."""
        self.last_mouse_x = event.x()
        self.last_mouse_y = event.y()
        self.mouse_button = event.button()
    
    def mouseMoveEvent(self, event: QMouseEvent):
        """Handle mouse move."""
        delta_x = event.x() - self.last_mouse_x
        delta_y = event.y() - self.last_mouse_y
        
        if self.mouse_button == Qt.LeftButton:
            # Rotate camera
            self.camera.rotate(delta_x, -delta_y)
            self.update()
        elif self.mouse_button == Qt.RightButton:
            # Pan camera
            self.camera.pan(-delta_x, delta_y)
            self.update()
        elif self.mouse_button == Qt.MiddleButton:
            # Zoom camera
            self.camera.zoom(delta_y)
            self.update()
        
        self.last_mouse_x = event.x()
        self.last_mouse_y = event.y()
    
    def mouseReleaseEvent(self, event: QMouseEvent):
        """Handle mouse release."""
        self.mouse_button = Qt.NoButton
    
    def wheelEvent(self, event: QWheelEvent):
        """Handle mouse wheel for zooming."""
        delta = event.angleDelta().y() / 120.0
        self.camera.zoom(delta)
        self.update()
    
    def keyPressEvent(self, event):
        """Handle keyboard input."""
        key = event.key()
        
        if key == Qt.Key_W:
            # Toggle wireframe
            self.renderer.wireframe_mode = not self.renderer.wireframe_mode
            self.update()
        elif key == Qt.Key_S:
            # Toggle skeleton
            self.renderer.show_skeleton = not self.renderer.show_skeleton
            self.update()
        elif key == Qt.Key_G:
            # Toggle grid
            self.renderer.show_grid = not self.renderer.show_grid
            self.update()
        elif key == Qt.Key_R:
            # Reset camera
            self.reset_camera()
        elif key == Qt.Key_H:
            # Hide/show UI (not implemented yet)
            pass
    
    def set_model(self, model: Model):
        """Set the model to display."""
        self.model = model
        
        # Focus camera on model
        if model:
            min_pt, max_pt = model.calculate_bounds()
            self.camera.focus_on_bounds(min_pt, max_pt)
        
        self.update()
    
    def set_skeleton(self, skeleton: Skeleton):
        """Set the skeleton to display."""
        self.skeleton = skeleton
        self.update()
    
    def set_selected_bone(self, bone: Optional[Bone]):
        """Set the selected bone."""
        self.selected_bone = bone
        self.update()
    
    def set_wireframe_mode(self, enabled: bool):
        """Set wireframe rendering mode."""
        self.renderer.wireframe_mode = enabled
        self.update()
    
    def set_skeleton_visible(self, visible: bool):
        """Set skeleton visibility."""
        self.renderer.show_skeleton = visible
        self.update()
    
    def set_grid_visible(self, visible: bool):
        """Set grid visibility."""
        self.renderer.show_grid = visible
        self.update()
    
    def reset_camera(self):
        """Reset camera to default position."""
        self.camera.reset()
        
        # If we have a model, focus on it
        if self.model:
            min_pt, max_pt = self.model.calculate_bounds()
            self.camera.focus_on_bounds(min_pt, max_pt)
        
        self.update()
