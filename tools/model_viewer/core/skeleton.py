"""
Skeleton data structures for THUG model viewer.

This module defines the data structures for representing character skeletons
and bone hierarchies from THUG character models.
"""

import numpy as np
from typing import List, Dict, Optional, Tuple


class Bone:
    """Represents a single bone in a skeleton."""
    
    def __init__(self, name: str, bone_id: int):
        self.name = name
        self.bone_id = bone_id
        self.parent: Optional[Bone] = None
        self.children: List[Bone] = []
        
        # Transform data (local space)
        self.position = np.array([0.0, 0.0, 0.0], dtype=np.float32)
        self.rotation = np.array([0.0, 0.0, 0.0, 1.0], dtype=np.float32)  # Quaternion (x, y, z, w)
        self.scale = np.array([1.0, 1.0, 1.0], dtype=np.float32)
        
        # Neutral/bind pose matrices
        self.bind_pose_matrix = np.identity(4, dtype=np.float32)
        self.inverse_bind_pose_matrix = np.identity(4, dtype=np.float32)
        
        # World space matrix (computed)
        self.world_matrix = np.identity(4, dtype=np.float32)
        
        # Flags from THUG format
        self.flags = 0
        self.flip_index = -1
        
    def add_child(self, child: 'Bone'):
        """Add a child bone to this bone."""
        if child not in self.children:
            self.children.append(child)
            child.parent = self
    
    def remove_child(self, child: 'Bone'):
        """Remove a child bone from this bone."""
        if child in self.children:
            self.children.remove(child)
            child.parent = None
    
    def get_local_matrix(self) -> np.ndarray:
        """Calculate local transformation matrix from position, rotation, and scale."""
        # Create translation matrix
        T = np.identity(4, dtype=np.float32)
        T[0:3, 3] = self.position
        
        # Create rotation matrix from quaternion
        x, y, z, w = self.rotation
        R = np.array([
            [1 - 2*y*y - 2*z*z, 2*x*y - 2*w*z, 2*x*z + 2*w*y, 0],
            [2*x*y + 2*w*z, 1 - 2*x*x - 2*z*z, 2*y*z - 2*w*x, 0],
            [2*x*z - 2*w*y, 2*y*z + 2*w*x, 1 - 2*x*x - 2*y*y, 0],
            [0, 0, 0, 1]
        ], dtype=np.float32)
        
        # Create scale matrix
        S = np.identity(4, dtype=np.float32)
        S[0, 0] = self.scale[0]
        S[1, 1] = self.scale[1]
        S[2, 2] = self.scale[2]
        
        # Combine: M = T * R * S
        return T @ R @ S
    
    def update_world_matrix(self):
        """Update world space matrix based on parent's world matrix."""
        local_matrix = self.get_local_matrix()
        
        if self.parent:
            self.world_matrix = self.parent.world_matrix @ local_matrix
        else:
            self.world_matrix = local_matrix
        
        # Update children recursively
        for child in self.children:
            child.update_world_matrix()
    
    def get_world_position(self) -> np.ndarray:
        """Get bone position in world space."""
        return self.world_matrix[0:3, 3]
    
    def __repr__(self):
        return f"Bone('{self.name}', id={self.bone_id}, children={len(self.children)})"


class Skeleton:
    """Represents a complete character skeleton."""
    
    def __init__(self, name: str = "Skeleton"):
        self.name = name
        self.bones: List[Bone] = []
        self.bone_map: Dict[str, Bone] = {}
        self.bone_id_map: Dict[int, Bone] = {}
        self.root_bones: List[Bone] = []
        
        # Skeleton flags from THUG format
        self.flags = 0
        
    def add_bone(self, bone: Bone):
        """Add a bone to the skeleton."""
        if bone not in self.bones:
            self.bones.append(bone)
            self.bone_map[bone.name] = bone
            self.bone_id_map[bone.bone_id] = bone
            
            # Track root bones (bones without parents)
            if bone.parent is None and bone not in self.root_bones:
                self.root_bones.append(bone)
    
    def get_bone_by_name(self, name: str) -> Optional[Bone]:
        """Get a bone by its name."""
        return self.bone_map.get(name)
    
    def get_bone_by_id(self, bone_id: int) -> Optional[Bone]:
        """Get a bone by its ID."""
        return self.bone_id_map.get(bone_id)
    
    def update_hierarchy(self):
        """Update the bone hierarchy and root bones list."""
        self.root_bones = [bone for bone in self.bones if bone.parent is None]
    
    def update_world_matrices(self):
        """Update all world space matrices starting from root bones."""
        for root in self.root_bones:
            root.update_world_matrix()
    
    def get_bone_count(self) -> int:
        """Get the total number of bones."""
        return len(self.bones)
    
    def get_hierarchy_depth(self) -> int:
        """Calculate the maximum depth of the bone hierarchy."""
        def get_depth(bone: Bone) -> int:
            if not bone.children:
                return 1
            return 1 + max(get_depth(child) for child in bone.children)
        
        if not self.root_bones:
            return 0
        return max(get_depth(root) for root in self.root_bones)
    
    def print_hierarchy(self, indent: int = 0, bone: Optional[Bone] = None):
        """Print the bone hierarchy to console."""
        if bone is None:
            # Print all root bones
            for root in self.root_bones:
                self.print_hierarchy(0, root)
        else:
            print("  " * indent + f"- {bone.name} (ID: {bone.bone_id})")
            for child in bone.children:
                self.print_hierarchy(indent + 1, child)
    
    def __repr__(self):
        return f"Skeleton('{self.name}', bones={len(self.bones)}, roots={len(self.root_bones)})"
