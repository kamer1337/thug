"""
Camera controller for the 3D viewport.

This module provides an orbit camera that can rotate around a target point.
"""

import numpy as np
from PyQt5.QtCore import QPointF


class Camera:
    """Orbit camera for 3D viewport."""
    
    def __init__(self):
        # Camera parameters
        self.distance = 10.0
        self.min_distance = 1.0
        self.max_distance = 100.0
        
        self.azimuth = 45.0    # Horizontal rotation (degrees)
        self.elevation = 30.0  # Vertical rotation (degrees)
        self.min_elevation = -89.0
        self.max_elevation = 89.0
        
        self.target = np.array([0.0, 0.0, 0.0], dtype=np.float32)
        self.position = np.array([0.0, 0.0, 0.0], dtype=np.float32)
        
        # Mouse interaction state
        self.last_mouse_pos = QPointF(0, 0)
        
        # Sensitivity settings
        self.rotate_sensitivity = 0.5
        self.pan_sensitivity = 0.01
        self.zoom_sensitivity = 0.1
        
        self._update_position()
    
    def _update_position(self):
        """Update camera position based on azimuth, elevation, and distance."""
        # Convert to radians
        azimuth_rad = np.radians(self.azimuth)
        elevation_rad = np.radians(self.elevation)
        
        # Calculate position in spherical coordinates
        x = self.distance * np.cos(elevation_rad) * np.cos(azimuth_rad)
        y = self.distance * np.sin(elevation_rad)
        z = self.distance * np.cos(elevation_rad) * np.sin(azimuth_rad)
        
        self.position = self.target + np.array([x, y, z], dtype=np.float32)
    
    def rotate(self, delta_x: float, delta_y: float):
        """
        Rotate camera around target.
        
        Args:
            delta_x: Horizontal mouse movement
            delta_y: Vertical mouse movement
        """
        self.azimuth += delta_x * self.rotate_sensitivity
        self.elevation += delta_y * self.rotate_sensitivity
        
        # Wrap azimuth
        self.azimuth = self.azimuth % 360.0
        
        # Clamp elevation
        self.elevation = np.clip(self.elevation, self.min_elevation, self.max_elevation)
        
        self._update_position()
    
    def pan(self, delta_x: float, delta_y: float):
        """
        Pan camera (move target point).
        
        Args:
            delta_x: Horizontal mouse movement
            delta_y: Vertical mouse movement
        """
        # Calculate right and up vectors
        view_dir = self.target - self.position
        view_dir = view_dir / np.linalg.norm(view_dir)
        
        right = np.cross(view_dir, np.array([0, 1, 0]))
        right_length = np.linalg.norm(right)
        if right_length > 0:
            right = right / right_length
        else:
            right = np.array([1, 0, 0])
        
        up = np.cross(right, view_dir)
        up = up / np.linalg.norm(up)
        
        # Move target
        pan_scale = self.distance * self.pan_sensitivity
        self.target += right * delta_x * pan_scale
        self.target += up * delta_y * pan_scale
        
        self._update_position()
    
    def zoom(self, delta: float):
        """
        Zoom camera (change distance from target).
        
        Args:
            delta: Zoom amount (positive = zoom in, negative = zoom out)
        """
        self.distance -= delta * self.zoom_sensitivity * self.distance
        self.distance = np.clip(self.distance, self.min_distance, self.max_distance)
        self._update_position()
    
    def reset(self):
        """Reset camera to default position."""
        self.distance = 10.0
        self.azimuth = 45.0
        self.elevation = 30.0
        self.target = np.array([0.0, 0.0, 0.0], dtype=np.float32)
        self._update_position()
    
    def focus_on_bounds(self, min_point: np.ndarray, max_point: np.ndarray):
        """
        Focus camera on a bounding box.
        
        Args:
            min_point: Minimum point of bounding box
            max_point: Maximum point of bounding box
        """
        # Calculate center and size
        center = (min_point + max_point) / 2.0
        size = np.linalg.norm(max_point - min_point)
        
        # Set target to center
        self.target = center
        
        # Set distance to fit the object
        self.distance = size * 2.0
        self.distance = np.clip(self.distance, self.min_distance, self.max_distance)
        
        self._update_position()
    
    def get_view_matrix(self) -> np.ndarray:
        """
        Get the view matrix for rendering.
        
        Returns:
            4x4 view matrix
        """
        # Calculate view direction
        forward = self.target - self.position
        forward = forward / np.linalg.norm(forward)
        
        # Calculate right vector
        right = np.cross(forward, np.array([0, 1, 0]))
        right_length = np.linalg.norm(right)
        if right_length > 0:
            right = right / right_length
        else:
            right = np.array([1, 0, 0])
        
        # Calculate up vector
        up = np.cross(right, forward)
        up = up / np.linalg.norm(up)
        
        # Build view matrix
        view = np.identity(4, dtype=np.float32)
        view[0, 0:3] = right
        view[1, 0:3] = up
        view[2, 0:3] = -forward
        view[0:3, 3] = -np.array([
            np.dot(right, self.position),
            np.dot(up, self.position),
            np.dot(-forward, self.position)
        ])
        
        return view
    
    def get_projection_matrix(self, fov: float, aspect: float, near: float, far: float) -> np.ndarray:
        """
        Get the projection matrix for rendering.
        
        Args:
            fov: Field of view in degrees
            aspect: Aspect ratio (width / height)
            near: Near clipping plane
            far: Far clipping plane
        
        Returns:
            4x4 projection matrix
        """
        fov_rad = np.radians(fov)
        f = 1.0 / np.tan(fov_rad / 2.0)
        
        proj = np.zeros((4, 4), dtype=np.float32)
        proj[0, 0] = f / aspect
        proj[1, 1] = f
        proj[2, 2] = (far + near) / (near - far)
        proj[2, 3] = (2.0 * far * near) / (near - far)
        proj[3, 2] = -1.0
        
        return proj
