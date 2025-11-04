"""
Skeleton tree widget for displaying bone hierarchy.
"""

from PyQt5.QtWidgets import QWidget, QVBoxLayout, QTreeWidget, QTreeWidgetItem, QLabel, QGroupBox
from PyQt5.QtCore import pyqtSignal, Qt
from typing import Optional

from core.skeleton import Skeleton, Bone


class SkeletonTree(QWidget):
    """Widget for displaying skeleton bone hierarchy."""
    
    bone_selected = pyqtSignal(str)  # Emits bone name
    
    def __init__(self, parent=None):
        super().__init__(parent)
        
        self.skeleton: Optional[Skeleton] = None
        self._setup_ui()
    
    def _setup_ui(self):
        """Set up the user interface."""
        layout = QVBoxLayout(self)
        
        # Info group
        info_group = QGroupBox("Skeleton Info")
        info_layout = QVBoxLayout(info_group)
        
        self.skeleton_name_label = QLabel("No skeleton loaded")
        self.bone_count_label = QLabel("Bones: 0")
        self.depth_label = QLabel("Depth: 0")
        
        info_layout.addWidget(self.skeleton_name_label)
        info_layout.addWidget(self.bone_count_label)
        info_layout.addWidget(self.depth_label)
        
        layout.addWidget(info_group)
        
        # Tree widget
        tree_group = QGroupBox("Bone Hierarchy")
        tree_layout = QVBoxLayout(tree_group)
        
        self.tree = QTreeWidget()
        self.tree.setHeaderLabels(["Bone Name", "ID"])
        self.tree.setColumnWidth(0, 150)
        self.tree.itemClicked.connect(self._on_item_clicked)
        
        tree_layout.addWidget(self.tree)
        
        layout.addWidget(tree_group)
    
    def set_skeleton(self, skeleton: Skeleton):
        """Set the skeleton to display."""
        self.skeleton = skeleton
        self._update_tree()
        self._update_info()
    
    def _update_tree(self):
        """Update the tree widget with skeleton data."""
        self.tree.clear()
        
        if not self.skeleton:
            return
        
        # Add root bones
        for root in self.skeleton.root_bones:
            self._add_bone_to_tree(root, None)
        
        self.tree.expandAll()
    
    def _add_bone_to_tree(self, bone: Bone, parent_item: Optional[QTreeWidgetItem]):
        """Recursively add bones to tree."""
        # Create tree item
        if parent_item:
            item = QTreeWidgetItem(parent_item)
        else:
            item = QTreeWidgetItem(self.tree)
        
        # Set data
        item.setText(0, bone.name)
        item.setText(1, str(bone.bone_id))
        item.setData(0, Qt.UserRole, bone.name)  # Store bone name
        
        # Add children
        for child in bone.children:
            self._add_bone_to_tree(child, item)
    
    def _update_info(self):
        """Update info labels."""
        if not self.skeleton:
            self.skeleton_name_label.setText("No skeleton loaded")
            self.bone_count_label.setText("Bones: 0")
            self.depth_label.setText("Depth: 0")
            return
        
        self.skeleton_name_label.setText(f"Skeleton: {self.skeleton.name}")
        self.bone_count_label.setText(f"Bones: {self.skeleton.get_bone_count()}")
        self.depth_label.setText(f"Depth: {self.skeleton.get_hierarchy_depth()}")
    
    def _on_item_clicked(self, item: QTreeWidgetItem, column: int):
        """Handle tree item click."""
        bone_name = item.data(0, Qt.UserRole)
        if bone_name:
            self.bone_selected.emit(bone_name)
