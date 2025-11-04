"""
OpenGL renderer for the 3D viewport.

This module handles all OpenGL rendering for models, skeletons, and animations.
"""

import numpy as np
from typing import Optional
from OpenGL.GL import *
from OpenGL.GLU import *

from .model_loader import Model, Mesh
from .skeleton import Skeleton, Bone
from .camera import Camera


class Renderer:
    """OpenGL renderer for 3D models and skeletons."""
    
    def __init__(self):
        self.wireframe_mode = False
        self.show_skeleton = True
        self.show_bone_names = False
        self.show_grid = True
        
        # Rendering colors
        self.grid_color = (0.3, 0.3, 0.3, 1.0)
        self.bone_color = (1.0, 0.5, 0.0, 1.0)
        self.joint_color = (1.0, 1.0, 0.0, 1.0)
        self.model_color = (0.7, 0.7, 0.7, 1.0)
        self.wireframe_color = (0.0, 1.0, 0.0, 1.0)
        
        self.initialized = False
    
    def initialize(self):
        """Initialize OpenGL settings."""
        if self.initialized:
            return
        
        # Enable depth testing
        glEnable(GL_DEPTH_TEST)
        glDepthFunc(GL_LESS)
        
        # Enable back-face culling
        glEnable(GL_CULL_FACE)
        glCullFace(GL_BACK)
        glFrontFace(GL_CCW)
        
        # Set up lighting
        glEnable(GL_LIGHTING)
        glEnable(GL_LIGHT0)
        
        # Light properties
        light_position = [5.0, 5.0, 5.0, 1.0]
        light_ambient = [0.2, 0.2, 0.2, 1.0]
        light_diffuse = [0.8, 0.8, 0.8, 1.0]
        light_specular = [1.0, 1.0, 1.0, 1.0]
        
        glLightfv(GL_LIGHT0, GL_POSITION, light_position)
        glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient)
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse)
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular)
        
        # Material properties
        glMaterialfv(GL_FRONT, GL_AMBIENT, [0.2, 0.2, 0.2, 1.0])
        glMaterialfv(GL_FRONT, GL_DIFFUSE, [0.8, 0.8, 0.8, 1.0])
        glMaterialfv(GL_FRONT, GL_SPECULAR, [1.0, 1.0, 1.0, 1.0])
        glMaterialf(GL_FRONT, GL_SHININESS, 50.0)
        
        # Enable color material
        glEnable(GL_COLOR_MATERIAL)
        glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE)
        
        # Set clear color (dark gray background)
        glClearColor(0.2, 0.2, 0.25, 1.0)
        
        self.initialized = True
    
    def clear(self):
        """Clear the viewport."""
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    
    def setup_viewport(self, width: int, height: int, camera: Camera):
        """
        Set up viewport and projection.
        
        Args:
            width: Viewport width
            height: Viewport height
            camera: Camera to use for view
        """
        glViewport(0, 0, width, height)
        
        # Set up projection matrix
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        
        aspect = width / height if height > 0 else 1.0
        proj_matrix = camera.get_projection_matrix(60.0, aspect, 0.1, 1000.0)
        glLoadMatrixf(proj_matrix.T)
        
        # Set up view matrix
        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()
        
        view_matrix = camera.get_view_matrix()
        glLoadMatrixf(view_matrix.T)
    
    def render_grid(self, size: float = 10.0, divisions: int = 10):
        """
        Render a grid on the ground plane.
        
        Args:
            size: Size of the grid
            divisions: Number of divisions
        """
        if not self.show_grid:
            return
        
        glDisable(GL_LIGHTING)
        glColor4f(*self.grid_color)
        
        glBegin(GL_LINES)
        
        step = size / divisions
        start = -size / 2.0
        
        # Draw lines parallel to X axis
        for i in range(divisions + 1):
            z = start + i * step
            glVertex3f(start, 0, z)
            glVertex3f(-start, 0, z)
        
        # Draw lines parallel to Z axis
        for i in range(divisions + 1):
            x = start + i * step
            glVertex3f(x, 0, start)
            glVertex3f(x, 0, -start)
        
        glEnd()
        
        glEnable(GL_LIGHTING)
    
    def render_model(self, model: Model):
        """
        Render a 3D model.
        
        Args:
            model: Model to render
        """
        if self.wireframe_mode:
            glDisable(GL_LIGHTING)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)
            glColor4f(*self.wireframe_color)
        else:
            glEnable(GL_LIGHTING)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)
            glColor4f(*self.model_color)
        
        for mesh in model.meshes:
            self._render_mesh(mesh)
        
        # Reset polygon mode
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)
    
    def _render_mesh(self, mesh: Mesh):
        """Render a single mesh."""
        glBegin(GL_TRIANGLES)
        
        for face in mesh.faces:
            for idx in face.indices:
                vertex = mesh.vertices[idx]
                glNormal3fv(vertex.normal)
                glVertex3fv(vertex.position)
        
        glEnd()
    
    def render_skeleton(self, skeleton: Skeleton):
        """
        Render a skeleton with bones and joints.
        
        Args:
            skeleton: Skeleton to render
        """
        if not self.show_skeleton:
            return
        
        glDisable(GL_LIGHTING)
        
        # Render bones (connections between joints)
        glColor4f(*self.bone_color)
        glLineWidth(3.0)
        
        glBegin(GL_LINES)
        for bone in skeleton.bones:
            if bone.parent:
                parent_pos = bone.parent.get_world_position()
                bone_pos = bone.get_world_position()
                glVertex3fv(parent_pos)
                glVertex3fv(bone_pos)
        glEnd()
        
        # Render joints (bone positions)
        glColor4f(*self.joint_color)
        glPointSize(8.0)
        
        glBegin(GL_POINTS)
        for bone in skeleton.bones:
            pos = bone.get_world_position()
            glVertex3fv(pos)
        glEnd()
        
        # Reset line width and point size
        glLineWidth(1.0)
        glPointSize(1.0)
        
        glEnable(GL_LIGHTING)
    
    def render_bone(self, bone: Bone, highlight: bool = False):
        """
        Render a single bone with optional highlighting.
        
        Args:
            bone: Bone to render
            highlight: Whether to highlight this bone
        """
        glDisable(GL_LIGHTING)
        
        if highlight:
            glColor4f(1.0, 0.0, 0.0, 1.0)
            glLineWidth(5.0)
            glPointSize(12.0)
        else:
            glColor4f(*self.bone_color)
            glLineWidth(3.0)
            glPointSize(8.0)
        
        # Render bone connection to parent
        if bone.parent:
            glBegin(GL_LINES)
            parent_pos = bone.parent.get_world_position()
            bone_pos = bone.get_world_position()
            glVertex3fv(parent_pos)
            glVertex3fv(bone_pos)
            glEnd()
        
        # Render joint
        glColor4f(*self.joint_color if not highlight else (1.0, 0.0, 0.0, 1.0))
        glBegin(GL_POINTS)
        pos = bone.get_world_position()
        glVertex3fv(pos)
        glEnd()
        
        # Reset
        glLineWidth(1.0)
        glPointSize(1.0)
        glEnable(GL_LIGHTING)
    
    def render_axes(self, size: float = 1.0):
        """
        Render coordinate axes.
        
        Args:
            size: Length of axes
        """
        glDisable(GL_LIGHTING)
        glLineWidth(2.0)
        
        glBegin(GL_LINES)
        
        # X axis (red)
        glColor3f(1.0, 0.0, 0.0)
        glVertex3f(0, 0, 0)
        glVertex3f(size, 0, 0)
        
        # Y axis (green)
        glColor3f(0.0, 1.0, 0.0)
        glVertex3f(0, 0, 0)
        glVertex3f(0, size, 0)
        
        # Z axis (blue)
        glColor3f(0.0, 0.0, 1.0)
        glVertex3f(0, 0, 0)
        glVertex3f(0, 0, size)
        
        glEnd()
        
        glLineWidth(1.0)
        glEnable(GL_LIGHTING)
