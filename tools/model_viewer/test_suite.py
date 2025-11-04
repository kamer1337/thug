#!/usr/bin/env python3
"""
Test suite for THUG Model Viewer.

This script tests all core functionality without requiring GUI components.
Run this to verify the tool is working correctly.
"""

import sys
import os

# Add current directory to path for imports
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import numpy as np
from core.skeleton import Skeleton, Bone
from core.animation import Animation, BoneAnimation, BoneKeyframe, AnimationLoopMode
from core.model_loader import Model, ModelLoader, Mesh, Vertex, Face

# Try to import Camera (requires PyQt5)
try:
    from core.camera import Camera
    CAMERA_AVAILABLE = True
except ImportError:
    CAMERA_AVAILABLE = False
    print("Note: PyQt5 not available, skipping Camera tests")


def test_bone():
    """Test Bone class."""
    print("Testing Bone class...")
    
    bone = Bone("test_bone", 1)
    assert bone.name == "test_bone"
    assert bone.bone_id == 1
    assert bone.parent is None
    assert len(bone.children) == 0
    
    # Test adding child
    child = Bone("child_bone", 2)
    bone.add_child(child)
    assert len(bone.children) == 1
    assert child.parent == bone
    
    # Test local matrix calculation
    matrix = bone.get_local_matrix()
    assert matrix.shape == (4, 4)
    
    print("  ✓ Bone class tests passed")


def test_skeleton():
    """Test Skeleton class."""
    print("Testing Skeleton class...")
    
    skeleton = Skeleton("test_skeleton")
    
    # Create a simple hierarchy
    root = Bone("root", 0)
    child1 = Bone("child1", 1)
    child2 = Bone("child2", 2)
    
    root.add_child(child1)
    root.add_child(child2)
    
    skeleton.add_bone(root)
    skeleton.add_bone(child1)
    skeleton.add_bone(child2)
    
    skeleton.update_hierarchy()
    
    assert skeleton.get_bone_count() == 3
    assert len(skeleton.root_bones) == 1
    assert skeleton.get_bone_by_name("root") == root
    assert skeleton.get_bone_by_id(1) == child1
    assert skeleton.get_hierarchy_depth() == 2
    
    # Test world matrix update
    skeleton.update_world_matrices()
    
    print("  ✓ Skeleton class tests passed")


def test_animation():
    """Test Animation classes."""
    print("Testing Animation classes...")
    
    # Test BoneKeyframe
    kf1 = BoneKeyframe(0)
    kf2 = BoneKeyframe(10)
    kf2.position = np.array([1.0, 2.0, 3.0], dtype=np.float32)
    
    # Test interpolation
    pos, rot, scale = kf1.interpolate(kf2, 0.5)
    assert np.allclose(pos, [0.5, 1.0, 1.5])
    
    # Test BoneAnimation
    bone_anim = BoneAnimation("test_bone")
    bone_anim.add_keyframe(kf1)
    bone_anim.add_keyframe(kf2)
    
    # Test transform at frame
    pos, rot, scale = bone_anim.get_transform_at_frame(5.0)
    assert np.allclose(pos, [0.5, 1.0, 1.5])
    
    # Test Animation
    anim = Animation("test_anim")
    anim.add_bone_animation(bone_anim)
    
    assert anim.duration_frames == 10
    assert anim.get_duration_seconds() == 10.0 / 30.0
    
    # Test playback
    anim.play()
    assert anim.is_playing
    
    anim.update(0.1)
    assert anim.current_frame > 0
    
    anim.pause()
    assert not anim.is_playing
    
    anim.stop()
    assert anim.current_frame == 0.0
    
    print("  ✓ Animation class tests passed")


def test_model():
    """Test Model and Mesh classes."""
    print("Testing Model classes...")
    
    # Test Vertex
    vertex = Vertex()
    assert len(vertex.position) == 3
    assert len(vertex.normal) == 3
    
    # Test Face
    face = Face((0, 1, 2))
    assert len(face.indices) == 3
    
    # Test Mesh
    mesh = Mesh("test_mesh")
    
    # Add some vertices
    for i in range(4):
        v = Vertex()
        v.position = np.array([i, 0, 0], dtype=np.float32)
        mesh.vertices.append(v)
    
    # Add faces
    mesh.faces.append(Face((0, 1, 2)))
    mesh.faces.append(Face((0, 2, 3)))
    
    assert mesh.get_vertex_count() == 4
    assert mesh.get_face_count() == 2
    
    # Test bounds calculation
    min_pt, max_pt = mesh.calculate_bounds()
    assert np.allclose(min_pt, [0, 0, 0])
    assert np.allclose(max_pt, [3, 0, 0])
    
    # Test normal calculation
    mesh.calculate_normals()
    
    # Test Model
    model = Model("test_model")
    model.add_mesh(mesh)
    
    assert len(model.meshes) == 1
    assert model.get_total_vertex_count() == 4
    assert model.get_total_face_count() == 2
    
    print("  ✓ Model class tests passed")


def test_model_loader():
    """Test ModelLoader."""
    print("Testing ModelLoader...")
    
    # Test cube creation
    model = ModelLoader._create_test_cube()
    assert model is not None
    assert len(model.meshes) > 0
    assert model.get_total_vertex_count() > 0
    assert model.get_total_face_count() > 0
    
    # Test skeleton creation
    skeleton = ModelLoader.create_test_skeleton()
    assert skeleton is not None
    assert skeleton.get_bone_count() > 0
    assert len(skeleton.root_bones) > 0
    
    print("  ✓ ModelLoader tests passed")


def test_camera():
    """Test Camera class."""
    if not CAMERA_AVAILABLE:
        print("Skipping Camera tests (PyQt5 not available)...")
        return
    
    print("Testing Camera class...")
    
    camera = Camera()
    
    # Test initial state
    assert camera.distance > 0
    assert camera.azimuth >= 0
    
    # Test rotation
    initial_azimuth = camera.azimuth
    camera.rotate(10, 0)
    assert camera.azimuth != initial_azimuth
    
    # Test zoom
    initial_distance = camera.distance
    camera.zoom(1.0)
    assert camera.distance != initial_distance
    
    # Test reset
    camera.reset()
    assert camera.distance == 10.0
    
    # Test view matrix
    view_matrix = camera.get_view_matrix()
    assert view_matrix.shape == (4, 4)
    
    # Test projection matrix
    proj_matrix = camera.get_projection_matrix(60.0, 16.0/9.0, 0.1, 1000.0)
    assert proj_matrix.shape == (4, 4)
    
    print("  ✓ Camera class tests passed")


def run_all_tests():
    """Run all tests."""
    print("=" * 60)
    print("THUG Model Viewer Test Suite")
    print("=" * 60)
    print()
    
    try:
        test_bone()
        test_skeleton()
        test_animation()
        test_model()
        test_model_loader()
        test_camera()
        
        print()
        print("=" * 60)
        print("✓ All tests passed!")
        print("=" * 60)
        return True
        
    except AssertionError as e:
        print()
        print("=" * 60)
        print(f"✗ Test failed: {e}")
        print("=" * 60)
        return False
    except Exception as e:
        print()
        print("=" * 60)
        print(f"✗ Unexpected error: {e}")
        import traceback
        traceback.print_exc()
        print("=" * 60)
        return False


if __name__ == '__main__':
    success = run_all_tests()
    sys.exit(0 if success else 1)
