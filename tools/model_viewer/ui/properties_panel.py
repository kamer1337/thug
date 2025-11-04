"""
Properties panel for displaying and editing bone properties.
"""

from PyQt5.QtWidgets import (
    QWidget, QVBoxLayout, QFormLayout, QLabel, QLineEdit,
    QDoubleSpinBox, QGroupBox
)
from typing import Optional
import numpy as np

from core.skeleton import Bone


class PropertiesPanel(QWidget):
    """Panel for displaying and editing bone properties."""
    
    def __init__(self, parent=None):
        super().__init__(parent)
        
        self.current_bone: Optional[Bone] = None
        self._setup_ui()
    
    def _setup_ui(self):
        """Set up the user interface."""
        layout = QVBoxLayout(self)
        
        # Bone info group
        info_group = QGroupBox("Bone Properties")
        info_layout = QFormLayout(info_group)
        
        self.name_label = QLabel("None")
        self.id_label = QLabel("N/A")
        self.parent_label = QLabel("None")
        
        info_layout.addRow("Name:", self.name_label)
        info_layout.addRow("ID:", self.id_label)
        info_layout.addRow("Parent:", self.parent_label)
        
        layout.addWidget(info_group)
        
        # Transform group
        transform_group = QGroupBox("Transform")
        transform_layout = QFormLayout(transform_group)
        
        # Position
        self.pos_x = QDoubleSpinBox()
        self.pos_x.setRange(-1000, 1000)
        self.pos_x.setDecimals(3)
        self.pos_x.valueChanged.connect(self._on_position_changed)
        
        self.pos_y = QDoubleSpinBox()
        self.pos_y.setRange(-1000, 1000)
        self.pos_y.setDecimals(3)
        self.pos_y.valueChanged.connect(self._on_position_changed)
        
        self.pos_z = QDoubleSpinBox()
        self.pos_z.setRange(-1000, 1000)
        self.pos_z.setDecimals(3)
        self.pos_z.valueChanged.connect(self._on_position_changed)
        
        transform_layout.addRow("Position X:", self.pos_x)
        transform_layout.addRow("Position Y:", self.pos_y)
        transform_layout.addRow("Position Z:", self.pos_z)
        
        # Rotation (showing quaternion components)
        self.rot_x = QDoubleSpinBox()
        self.rot_x.setRange(-1, 1)
        self.rot_x.setDecimals(4)
        self.rot_x.setReadOnly(True)
        
        self.rot_y = QDoubleSpinBox()
        self.rot_y.setRange(-1, 1)
        self.rot_y.setDecimals(4)
        self.rot_y.setReadOnly(True)
        
        self.rot_z = QDoubleSpinBox()
        self.rot_z.setRange(-1, 1)
        self.rot_z.setDecimals(4)
        self.rot_z.setReadOnly(True)
        
        self.rot_w = QDoubleSpinBox()
        self.rot_w.setRange(-1, 1)
        self.rot_w.setDecimals(4)
        self.rot_w.setReadOnly(True)
        
        transform_layout.addRow("Rotation X:", self.rot_x)
        transform_layout.addRow("Rotation Y:", self.rot_y)
        transform_layout.addRow("Rotation Z:", self.rot_z)
        transform_layout.addRow("Rotation W:", self.rot_w)
        
        # Scale
        self.scale_x = QDoubleSpinBox()
        self.scale_x.setRange(0.01, 100)
        self.scale_x.setDecimals(3)
        self.scale_x.setValue(1.0)
        self.scale_x.valueChanged.connect(self._on_scale_changed)
        
        self.scale_y = QDoubleSpinBox()
        self.scale_y.setRange(0.01, 100)
        self.scale_y.setDecimals(3)
        self.scale_y.setValue(1.0)
        self.scale_y.valueChanged.connect(self._on_scale_changed)
        
        self.scale_z = QDoubleSpinBox()
        self.scale_z.setRange(0.01, 100)
        self.scale_z.setDecimals(3)
        self.scale_z.setValue(1.0)
        self.scale_z.valueChanged.connect(self._on_scale_changed)
        
        transform_layout.addRow("Scale X:", self.scale_x)
        transform_layout.addRow("Scale Y:", self.scale_y)
        transform_layout.addRow("Scale Z:", self.scale_z)
        
        layout.addWidget(transform_group)
        
        # Add stretch to push everything to top
        layout.addStretch()
        
        # Initially disable all controls
        self._set_enabled(False)
    
    def set_bone(self, bone: Optional[Bone]):
        """Set the bone to display properties for."""
        self.current_bone = bone
        
        if bone:
            self._set_enabled(True)
            self._update_from_bone()
        else:
            self._set_enabled(False)
            self._clear()
    
    def _update_from_bone(self):
        """Update UI from bone data."""
        if not self.current_bone:
            return
        
        # Block signals to avoid triggering changes
        self._block_signals(True)
        
        # Update info
        self.name_label.setText(self.current_bone.name)
        self.id_label.setText(str(self.current_bone.bone_id))
        
        if self.current_bone.parent:
            self.parent_label.setText(self.current_bone.parent.name)
        else:
            self.parent_label.setText("None (Root)")
        
        # Update position
        self.pos_x.setValue(float(self.current_bone.position[0]))
        self.pos_y.setValue(float(self.current_bone.position[1]))
        self.pos_z.setValue(float(self.current_bone.position[2]))
        
        # Update rotation (quaternion)
        self.rot_x.setValue(float(self.current_bone.rotation[0]))
        self.rot_y.setValue(float(self.current_bone.rotation[1]))
        self.rot_z.setValue(float(self.current_bone.rotation[2]))
        self.rot_w.setValue(float(self.current_bone.rotation[3]))
        
        # Update scale
        self.scale_x.setValue(float(self.current_bone.scale[0]))
        self.scale_y.setValue(float(self.current_bone.scale[1]))
        self.scale_z.setValue(float(self.current_bone.scale[2]))
        
        self._block_signals(False)
    
    def _clear(self):
        """Clear all fields."""
        self.name_label.setText("None")
        self.id_label.setText("N/A")
        self.parent_label.setText("None")
    
    def _on_position_changed(self):
        """Handle position change."""
        if self.current_bone:
            self.current_bone.position = np.array([
                self.pos_x.value(),
                self.pos_y.value(),
                self.pos_z.value()
            ], dtype=np.float32)
            self.current_bone.update_world_matrix()
    
    def _on_scale_changed(self):
        """Handle scale change."""
        if self.current_bone:
            self.current_bone.scale = np.array([
                self.scale_x.value(),
                self.scale_y.value(),
                self.scale_z.value()
            ], dtype=np.float32)
            self.current_bone.update_world_matrix()
    
    def _set_enabled(self, enabled: bool):
        """Enable or disable all controls."""
        self.pos_x.setEnabled(enabled)
        self.pos_y.setEnabled(enabled)
        self.pos_z.setEnabled(enabled)
        self.scale_x.setEnabled(enabled)
        self.scale_y.setEnabled(enabled)
        self.scale_z.setEnabled(enabled)
    
    def _block_signals(self, block: bool):
        """Block or unblock signals from spin boxes."""
        self.pos_x.blockSignals(block)
        self.pos_y.blockSignals(block)
        self.pos_z.blockSignals(block)
        self.scale_x.blockSignals(block)
        self.scale_y.blockSignals(block)
        self.scale_z.blockSignals(block)
