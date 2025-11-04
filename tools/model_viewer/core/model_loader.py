"""
Model loading and data structures for THUG model viewer.

This module handles loading 3D models from THUG model files.
"""

import numpy as np
from typing import List, Optional, Tuple
import struct


class Vertex:
    """Represents a single vertex in a 3D model."""
    
    def __init__(self):
        self.position = np.array([0.0, 0.0, 0.0], dtype=np.float32)
        self.normal = np.array([0.0, 1.0, 0.0], dtype=np.float32)
        self.uv = np.array([0.0, 0.0], dtype=np.float32)
        self.color = np.array([1.0, 1.0, 1.0, 1.0], dtype=np.float32)
        
        # Skinning data
        self.bone_indices = []  # List of bone indices affecting this vertex
        self.bone_weights = []  # Corresponding weights (should sum to 1.0)


class Face:
    """Represents a triangle face."""
    
    def __init__(self, indices: Tuple[int, int, int]):
        self.indices = indices  # Tuple of 3 vertex indices


class Mesh:
    """Represents a mesh (collection of vertices and faces)."""
    
    def __init__(self, name: str = "Mesh"):
        self.name = name
        self.vertices: List[Vertex] = []
        self.faces: List[Face] = []
        self.material_name: Optional[str] = None
        
    def get_vertex_count(self) -> int:
        """Get number of vertices."""
        return len(self.vertices)
    
    def get_face_count(self) -> int:
        """Get number of faces."""
        return len(self.faces)
    
    def get_triangle_count(self) -> int:
        """Get number of triangles (same as face count)."""
        return len(self.faces)
    
    def calculate_bounds(self) -> Tuple[np.ndarray, np.ndarray]:
        """
        Calculate bounding box of the mesh.
        
        Returns:
            Tuple of (min_point, max_point)
        """
        if not self.vertices:
            return np.zeros(3), np.zeros(3)
        
        positions = np.array([v.position for v in self.vertices])
        return np.min(positions, axis=0), np.max(positions, axis=0)
    
    def calculate_normals(self):
        """Calculate face and vertex normals."""
        # Reset all normals
        for vertex in self.vertices:
            vertex.normal = np.array([0.0, 0.0, 0.0], dtype=np.float32)
        
        # Calculate face normals and accumulate to vertices
        for face in self.faces:
            v0 = self.vertices[face.indices[0]].position
            v1 = self.vertices[face.indices[1]].position
            v2 = self.vertices[face.indices[2]].position
            
            # Calculate face normal
            edge1 = v1 - v0
            edge2 = v2 - v0
            normal = np.cross(edge1, edge2)
            
            # Normalize
            length = np.linalg.norm(normal)
            if length > 0:
                normal = normal / length
            
            # Accumulate to vertices
            for idx in face.indices:
                self.vertices[idx].normal += normal
        
        # Normalize vertex normals
        for vertex in self.vertices:
            length = np.linalg.norm(vertex.normal)
            if length > 0:
                vertex.normal = vertex.normal / length
            else:
                vertex.normal = np.array([0.0, 1.0, 0.0], dtype=np.float32)


class Model:
    """Represents a complete 3D model."""
    
    def __init__(self, name: str = "Model"):
        self.name = name
        self.meshes: List[Mesh] = []
        self.skeleton = None  # Optional skeleton reference
        
    def add_mesh(self, mesh: Mesh):
        """Add a mesh to the model."""
        self.meshes.append(mesh)
    
    def get_total_vertex_count(self) -> int:
        """Get total number of vertices across all meshes."""
        return sum(mesh.get_vertex_count() for mesh in self.meshes)
    
    def get_total_face_count(self) -> int:
        """Get total number of faces across all meshes."""
        return sum(mesh.get_face_count() for mesh in self.meshes)
    
    def calculate_bounds(self) -> Tuple[np.ndarray, np.ndarray]:
        """
        Calculate bounding box of the entire model.
        
        Returns:
            Tuple of (min_point, max_point)
        """
        if not self.meshes:
            return np.zeros(3), np.zeros(3)
        
        all_mins = []
        all_maxs = []
        
        for mesh in self.meshes:
            min_pt, max_pt = mesh.calculate_bounds()
            all_mins.append(min_pt)
            all_maxs.append(max_pt)
        
        return np.min(all_mins, axis=0), np.max(all_maxs, axis=0)
    
    def __repr__(self):
        return f"Model('{self.name}', meshes={len(self.meshes)}, vertices={self.get_total_vertex_count()}, faces={self.get_total_face_count()})"


class ModelLoader:
    """Loads 3D models from THUG file formats."""
    
    @staticmethod
    def load_model(filepath: str) -> Optional[Model]:
        """
        Load a model from a THUG .mdl file.
        
        Args:
            filepath: Path to the .mdl file
        
        Returns:
            Model object or None if loading failed
        """
        try:
            # For now, create a simple test cube since we don't have the actual format parser yet
            # This will be replaced with actual THUG format parsing
            model = ModelLoader._create_test_cube()
            model.name = filepath.split('/')[-1].replace('.mdl', '')
            return model
        except Exception as e:
            print(f"Error loading model: {e}")
            return None
    
    @staticmethod
    def _create_test_cube() -> Model:
        """Create a simple test cube model for testing."""
        model = Model("TestCube")
        mesh = Mesh("CubeMesh")
        
        # Define cube vertices (8 corners)
        cube_positions = [
            [-1, -1, -1], [1, -1, -1], [1, 1, -1], [-1, 1, -1],  # Front face
            [-1, -1, 1], [1, -1, 1], [1, 1, 1], [-1, 1, 1]       # Back face
        ]
        
        # Create vertices for each face (we need separate vertices for proper normals)
        # Front face
        for i in range(4):
            v = Vertex()
            v.position = np.array(cube_positions[i], dtype=np.float32)
            v.normal = np.array([0, 0, -1], dtype=np.float32)
            mesh.vertices.append(v)
        
        # Back face
        for i in range(4, 8):
            v = Vertex()
            v.position = np.array(cube_positions[i], dtype=np.float32)
            v.normal = np.array([0, 0, 1], dtype=np.float32)
            mesh.vertices.append(v)
        
        # Right face
        for i in [1, 5, 6, 2]:
            v = Vertex()
            v.position = np.array(cube_positions[i], dtype=np.float32)
            v.normal = np.array([1, 0, 0], dtype=np.float32)
            mesh.vertices.append(v)
        
        # Left face
        for i in [4, 0, 3, 7]:
            v = Vertex()
            v.position = np.array(cube_positions[i], dtype=np.float32)
            v.normal = np.array([-1, 0, 0], dtype=np.float32)
            mesh.vertices.append(v)
        
        # Top face
        for i in [3, 2, 6, 7]:
            v = Vertex()
            v.position = np.array(cube_positions[i], dtype=np.float32)
            v.normal = np.array([0, 1, 0], dtype=np.float32)
            mesh.vertices.append(v)
        
        # Bottom face
        for i in [4, 5, 1, 0]:
            v = Vertex()
            v.position = np.array(cube_positions[i], dtype=np.float32)
            v.normal = np.array([0, -1, 0], dtype=np.float32)
            mesh.vertices.append(v)
        
        # Define faces (2 triangles per face)
        face_indices = [
            # Front
            (0, 1, 2), (0, 2, 3),
            # Back
            (4, 6, 5), (4, 7, 6),
            # Right
            (8, 9, 10), (8, 10, 11),
            # Left
            (12, 13, 14), (12, 14, 15),
            # Top
            (16, 17, 18), (16, 18, 19),
            # Bottom
            (20, 21, 22), (20, 22, 23)
        ]
        
        for indices in face_indices:
            mesh.faces.append(Face(indices))
        
        model.add_mesh(mesh)
        return model
    
    @staticmethod
    def create_test_skeleton() -> 'Skeleton':
        """Create a simple test skeleton for testing."""
        from .skeleton import Skeleton, Bone
        
        skeleton = Skeleton("TestSkeleton")
        
        # Create a simple skeleton: root -> spine -> head
        #                                  -> left_arm
        #                                  -> right_arm
        
        root = Bone("root", 0)
        root.position = np.array([0, 0, 0], dtype=np.float32)
        skeleton.add_bone(root)
        
        spine = Bone("spine", 1)
        spine.position = np.array([0, 1, 0], dtype=np.float32)
        root.add_child(spine)
        skeleton.add_bone(spine)
        
        head = Bone("head", 2)
        head.position = np.array([0, 0.5, 0], dtype=np.float32)
        spine.add_child(head)
        skeleton.add_bone(head)
        
        left_arm = Bone("left_arm", 3)
        left_arm.position = np.array([-0.5, 0.3, 0], dtype=np.float32)
        spine.add_child(left_arm)
        skeleton.add_bone(left_arm)
        
        right_arm = Bone("right_arm", 4)
        right_arm.position = np.array([0.5, 0.3, 0], dtype=np.float32)
        spine.add_child(right_arm)
        skeleton.add_bone(right_arm)
        
        skeleton.update_hierarchy()
        skeleton.update_world_matrices()
        
        return skeleton
