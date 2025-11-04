#!/usr/bin/env python3
"""
Example: Using the THUG Model Viewer API

This script demonstrates how to use the model viewer components
programmatically without the GUI.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from core.skeleton import Skeleton, Bone
from core.animation import Animation, BoneAnimation, BoneKeyframe, AnimationLoopMode
from core.model_loader import ModelLoader
import numpy as np


def example_create_skeleton():
    """Example: Create a custom skeleton."""
    print("=" * 60)
    print("Example 1: Creating a Custom Skeleton")
    print("=" * 60)
    
    # Create skeleton
    skeleton = Skeleton("CustomSkeleton")
    
    # Create bones
    pelvis = Bone("pelvis", 0)
    pelvis.position = np.array([0, 1, 0], dtype=np.float32)
    skeleton.add_bone(pelvis)
    
    spine = Bone("spine", 1)
    spine.position = np.array([0, 0.5, 0], dtype=np.float32)
    pelvis.add_child(spine)
    skeleton.add_bone(spine)
    
    head = Bone("head", 2)
    head.position = np.array([0, 0.3, 0], dtype=np.float32)
    spine.add_child(head)
    skeleton.add_bone(head)
    
    left_shoulder = Bone("left_shoulder", 3)
    left_shoulder.position = np.array([-0.2, 0.2, 0], dtype=np.float32)
    spine.add_child(left_shoulder)
    skeleton.add_bone(left_shoulder)
    
    left_arm = Bone("left_arm", 4)
    left_arm.position = np.array([-0.3, 0, 0], dtype=np.float32)
    left_shoulder.add_child(left_arm)
    skeleton.add_bone(left_arm)
    
    # Update hierarchy
    skeleton.update_hierarchy()
    skeleton.update_world_matrices()
    
    # Print skeleton info
    print(f"\nCreated skeleton: {skeleton}")
    print(f"Total bones: {skeleton.get_bone_count()}")
    print(f"Hierarchy depth: {skeleton.get_hierarchy_depth()}")
    print("\nBone hierarchy:")
    skeleton.print_hierarchy()
    
    return skeleton


def example_create_animation():
    """Example: Create an animation."""
    print("\n" + "=" * 60)
    print("Example 2: Creating an Animation")
    print("=" * 60)
    
    # Create animation
    anim = Animation("WalkCycle")
    anim.frame_rate = 30.0
    
    # Animate spine bone
    spine_anim = BoneAnimation("spine")
    
    # Frame 0: Start position
    kf0 = BoneKeyframe(0)
    kf0.position = np.array([0, 0, 0], dtype=np.float32)
    kf0.rotation = np.array([0, 0, 0, 1], dtype=np.float32)
    spine_anim.add_keyframe(kf0)
    
    # Frame 15: Mid position (bobbing up)
    kf1 = BoneKeyframe(15)
    kf1.position = np.array([0, 0.1, 0], dtype=np.float32)
    kf1.rotation = np.array([0, 0, 0, 1], dtype=np.float32)
    spine_anim.add_keyframe(kf1)
    
    # Frame 30: End position (back to start)
    kf2 = BoneKeyframe(30)
    kf2.position = np.array([0, 0, 0], dtype=np.float32)
    kf2.rotation = np.array([0, 0, 0, 1], dtype=np.float32)
    spine_anim.add_keyframe(kf2)
    
    anim.add_bone_animation(spine_anim)
    
    # Animate left arm
    arm_anim = BoneAnimation("left_arm")
    
    kf0 = BoneKeyframe(0)
    kf0.rotation = np.array([0, 0, 0, 1], dtype=np.float32)
    arm_anim.add_keyframe(kf0)
    
    kf1 = BoneKeyframe(15)
    # Rotate arm forward (quaternion for 45 degree rotation around Z)
    kf1.rotation = np.array([0, 0, 0.3827, 0.9239], dtype=np.float32)
    arm_anim.add_keyframe(kf1)
    
    kf2 = BoneKeyframe(30)
    kf2.rotation = np.array([0, 0, 0, 1], dtype=np.float32)
    arm_anim.add_keyframe(kf2)
    
    anim.add_bone_animation(arm_anim)
    
    print(f"\nCreated animation: {anim}")
    print(f"Duration: {anim.duration_frames} frames ({anim.get_duration_seconds():.2f}s)")
    print(f"Animated bones: {len(anim.bone_animations)}")
    
    return anim


def example_apply_animation():
    """Example: Apply animation to skeleton."""
    print("\n" + "=" * 60)
    print("Example 3: Applying Animation to Skeleton")
    print("=" * 60)
    
    # Create skeleton and animation
    skeleton = example_create_skeleton()
    anim = example_create_animation()
    
    # Apply animation at different frames
    frames_to_test = [0, 7, 15, 22, 30]
    
    print(f"\nApplying animation at different frames:")
    for frame in frames_to_test:
        anim.set_frame(frame)
        anim.apply_to_skeleton(skeleton)
        
        # Get spine bone position
        spine = skeleton.get_bone_by_name("spine")
        if spine:
            world_pos = spine.get_world_position()
            print(f"  Frame {frame:2d}: spine position = [{world_pos[0]:6.3f}, {world_pos[1]:6.3f}, {world_pos[2]:6.3f}]")


def example_model_loading():
    """Example: Load and inspect a model."""
    print("\n" + "=" * 60)
    print("Example 4: Model Loading and Inspection")
    print("=" * 60)
    
    # Load test cube
    model = ModelLoader._create_test_cube()
    
    print(f"\nLoaded model: {model}")
    print(f"Number of meshes: {len(model.meshes)}")
    print(f"Total vertices: {model.get_total_vertex_count()}")
    print(f"Total faces: {model.get_total_face_count()}")
    
    # Calculate bounds
    min_pt, max_pt = model.calculate_bounds()
    print(f"\nBounding box:")
    print(f"  Min: [{min_pt[0]:6.3f}, {min_pt[1]:6.3f}, {min_pt[2]:6.3f}]")
    print(f"  Max: [{max_pt[0]:6.3f}, {max_pt[1]:6.3f}, {max_pt[2]:6.3f}]")
    
    # Inspect first mesh
    if model.meshes:
        mesh = model.meshes[0]
        print(f"\nFirst mesh: {mesh.name}")
        print(f"  Vertices: {mesh.get_vertex_count()}")
        print(f"  Faces: {mesh.get_face_count()}")
        
        # Show first vertex
        if mesh.vertices:
            v = mesh.vertices[0]
            print(f"\nFirst vertex:")
            print(f"  Position: [{v.position[0]:6.3f}, {v.position[1]:6.3f}, {v.position[2]:6.3f}]")
            print(f"  Normal:   [{v.normal[0]:6.3f}, {v.normal[1]:6.3f}, {v.normal[2]:6.3f}]")


def main():
    """Run all examples."""
    print("\n" + "=" * 60)
    print("THUG Model Viewer - API Examples")
    print("=" * 60)
    print()
    
    example_create_skeleton()
    example_create_animation()
    example_apply_animation()
    example_model_loading()
    
    print("\n" + "=" * 60)
    print("Examples completed!")
    print("=" * 60)
    print("\nTo run the full GUI application, use:")
    print("  python model_viewer.py")
    print()


if __name__ == '__main__':
    main()
