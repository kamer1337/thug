"""
Animation data structures for THUG model viewer.

This module defines the data structures for representing character animations
and animation playback from THUG animation files.
"""

import numpy as np
from typing import List, Dict, Optional, Tuple
from enum import Enum
from .skeleton import Skeleton, Bone


class AnimationLoopMode(Enum):
    """Animation loop modes."""
    ONCE = 0        # Play once and stop
    LOOP = 1        # Loop continuously
    PINGPONG = 2    # Play forward then backward


class BoneKeyframe:
    """Represents a single keyframe for a bone."""
    
    def __init__(self, frame: int):
        self.frame = frame
        self.position = np.array([0.0, 0.0, 0.0], dtype=np.float32)
        self.rotation = np.array([0.0, 0.0, 0.0, 1.0], dtype=np.float32)  # Quaternion
        self.scale = np.array([1.0, 1.0, 1.0], dtype=np.float32)
        
    def interpolate(self, other: 'BoneKeyframe', t: float) -> Tuple[np.ndarray, np.ndarray, np.ndarray]:
        """
        Interpolate between this keyframe and another.
        
        Args:
            other: The other keyframe to interpolate with
            t: Interpolation factor (0.0 to 1.0)
        
        Returns:
            Tuple of (position, rotation, scale)
        """
        # Linear interpolation for position and scale
        position = self.position * (1.0 - t) + other.position * t
        scale = self.scale * (1.0 - t) + other.scale * t
        
        # Spherical linear interpolation (SLERP) for rotation
        rotation = self._slerp(self.rotation, other.rotation, t)
        
        return position, rotation, scale
    
    @staticmethod
    def _slerp(q1: np.ndarray, q2: np.ndarray, t: float) -> np.ndarray:
        """Spherical linear interpolation for quaternions."""
        # Compute dot product
        dot = np.dot(q1, q2)
        
        # If the dot product is negative, negate one quaternion to take the shorter path
        if dot < 0.0:
            q2 = -q2
            dot = -dot
        
        # If quaternions are very close, use linear interpolation
        if dot > 0.9995:
            result = q1 + t * (q2 - q1)
            return result / np.linalg.norm(result)
        
        # Calculate angle between quaternions
        theta = np.arccos(np.clip(dot, -1.0, 1.0))
        sin_theta = np.sin(theta)
        
        # Calculate interpolation coefficients
        w1 = np.sin((1.0 - t) * theta) / sin_theta
        w2 = np.sin(t * theta) / sin_theta
        
        # Interpolate
        return w1 * q1 + w2 * q2


class BoneAnimation:
    """Animation data for a single bone."""
    
    def __init__(self, bone_name: str):
        self.bone_name = bone_name
        self.keyframes: List[BoneKeyframe] = []
    
    def add_keyframe(self, keyframe: BoneKeyframe):
        """Add a keyframe to this bone animation."""
        self.keyframes.append(keyframe)
        # Keep keyframes sorted by frame number
        self.keyframes.sort(key=lambda k: k.frame)
    
    def get_transform_at_frame(self, frame: float) -> Tuple[np.ndarray, np.ndarray, np.ndarray]:
        """
        Get interpolated transform at a specific frame.
        
        Args:
            frame: Frame number (can be fractional for interpolation)
        
        Returns:
            Tuple of (position, rotation, scale)
        """
        if not self.keyframes:
            # Return identity transform
            return (
                np.array([0.0, 0.0, 0.0], dtype=np.float32),
                np.array([0.0, 0.0, 0.0, 1.0], dtype=np.float32),
                np.array([1.0, 1.0, 1.0], dtype=np.float32)
            )
        
        # Find surrounding keyframes
        prev_kf = self.keyframes[0]
        next_kf = self.keyframes[-1]
        
        for i in range(len(self.keyframes) - 1):
            if self.keyframes[i].frame <= frame <= self.keyframes[i + 1].frame:
                prev_kf = self.keyframes[i]
                next_kf = self.keyframes[i + 1]
                break
        
        # If frame is exactly on a keyframe
        if prev_kf.frame == next_kf.frame:
            return prev_kf.position, prev_kf.rotation, prev_kf.scale
        
        # Calculate interpolation factor
        t = (frame - prev_kf.frame) / (next_kf.frame - prev_kf.frame)
        
        # Interpolate between keyframes
        return prev_kf.interpolate(next_kf, t)


class Animation:
    """Represents a complete character animation."""
    
    def __init__(self, name: str = "Animation"):
        self.name = name
        self.bone_animations: Dict[str, BoneAnimation] = {}
        self.duration_frames = 0
        self.frame_rate = 30.0  # Default to 30 FPS
        
        # Playback state
        self.current_frame = 0.0
        self.is_playing = False
        self.loop_mode = AnimationLoopMode.LOOP
        self.playback_speed = 1.0
        self.playback_direction = 1.0  # 1.0 for forward, -1.0 for backward
    
    def add_bone_animation(self, bone_anim: BoneAnimation):
        """Add animation data for a bone."""
        self.bone_animations[bone_anim.bone_name] = bone_anim
        
        # Update duration based on longest animation
        if bone_anim.keyframes:
            last_frame = bone_anim.keyframes[-1].frame
            self.duration_frames = max(self.duration_frames, last_frame)
    
    def get_bone_animation(self, bone_name: str) -> Optional[BoneAnimation]:
        """Get animation data for a specific bone."""
        return self.bone_animations.get(bone_name)
    
    def apply_to_skeleton(self, skeleton: Skeleton):
        """Apply current animation frame to a skeleton."""
        for bone in skeleton.bones:
            bone_anim = self.bone_animations.get(bone.name)
            if bone_anim:
                position, rotation, scale = bone_anim.get_transform_at_frame(self.current_frame)
                bone.position = position
                bone.rotation = rotation
                bone.scale = scale
        
        # Update world matrices
        skeleton.update_world_matrices()
    
    def update(self, delta_time: float):
        """
        Update animation playback.
        
        Args:
            delta_time: Time elapsed since last update in seconds
        """
        if not self.is_playing or self.duration_frames == 0:
            return
        
        # Calculate frame increment
        frames_per_second = self.frame_rate * self.playback_speed
        frame_increment = frames_per_second * delta_time * self.playback_direction
        
        # Update current frame
        self.current_frame += frame_increment
        
        # Handle looping
        if self.loop_mode == AnimationLoopMode.ONCE:
            if self.current_frame >= self.duration_frames:
                self.current_frame = self.duration_frames
                self.is_playing = False
            elif self.current_frame < 0:
                self.current_frame = 0
                self.is_playing = False
                
        elif self.loop_mode == AnimationLoopMode.LOOP:
            if self.current_frame >= self.duration_frames:
                self.current_frame = self.current_frame % self.duration_frames
            elif self.current_frame < 0:
                self.current_frame = self.duration_frames + (self.current_frame % self.duration_frames)
                
        elif self.loop_mode == AnimationLoopMode.PINGPONG:
            if self.current_frame >= self.duration_frames:
                self.current_frame = self.duration_frames
                self.playback_direction = -1.0
            elif self.current_frame < 0:
                self.current_frame = 0
                self.playback_direction = 1.0
    
    def play(self):
        """Start animation playback."""
        self.is_playing = True
    
    def pause(self):
        """Pause animation playback."""
        self.is_playing = False
    
    def stop(self):
        """Stop animation playback and reset to first frame."""
        self.is_playing = False
        self.current_frame = 0.0
        self.playback_direction = 1.0
    
    def step_forward(self):
        """Step forward one frame."""
        self.current_frame = min(self.current_frame + 1.0, self.duration_frames)
    
    def step_backward(self):
        """Step backward one frame."""
        self.current_frame = max(self.current_frame - 1.0, 0.0)
    
    def set_frame(self, frame: float):
        """Set current frame."""
        self.current_frame = np.clip(frame, 0.0, self.duration_frames)
    
    def get_duration_seconds(self) -> float:
        """Get animation duration in seconds."""
        return self.duration_frames / self.frame_rate
    
    def __repr__(self):
        return f"Animation('{self.name}', duration={self.duration_frames} frames, bones={len(self.bone_animations)})"
