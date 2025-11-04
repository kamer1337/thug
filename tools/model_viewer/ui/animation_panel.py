"""
Animation control panel widget.
"""

from PyQt5.QtWidgets import (
    QWidget, QVBoxLayout, QHBoxLayout, QLabel, QPushButton,
    QSlider, QSpinBox, QDoubleSpinBox, QGroupBox
)
from PyQt5.QtCore import Qt, pyqtSignal


class AnimationPanel(QWidget):
    """Panel for animation playback controls."""
    
    play_requested = pyqtSignal()
    pause_requested = pyqtSignal()
    stop_requested = pyqtSignal()
    frame_changed = pyqtSignal(float)
    speed_changed = pyqtSignal(float)
    
    def __init__(self, parent=None):
        super().__init__(parent)
        
        self.is_playing = False
        self.current_frame = 0.0
        self.total_frames = 0
        
        self._setup_ui()
    
    def _setup_ui(self):
        """Set up the user interface."""
        layout = QVBoxLayout(self)
        
        # Animation info group
        info_group = QGroupBox("Animation Info")
        info_layout = QVBoxLayout(info_group)
        
        self.name_label = QLabel("No animation loaded")
        self.frame_label = QLabel("Frame: 0 / 0")
        self.time_label = QLabel("Time: 0.00s / 0.00s")
        
        info_layout.addWidget(self.name_label)
        info_layout.addWidget(self.frame_label)
        info_layout.addWidget(self.time_label)
        
        layout.addWidget(info_group)
        
        # Playback controls
        controls_group = QGroupBox("Playback Controls")
        controls_layout = QVBoxLayout(controls_group)
        
        # Buttons
        button_layout = QHBoxLayout()
        
        self.play_button = QPushButton("Play")
        self.play_button.clicked.connect(self._on_play_clicked)
        
        self.pause_button = QPushButton("Pause")
        self.pause_button.clicked.connect(self._on_pause_clicked)
        self.pause_button.setEnabled(False)
        
        self.stop_button = QPushButton("Stop")
        self.stop_button.clicked.connect(self._on_stop_clicked)
        
        button_layout.addWidget(self.play_button)
        button_layout.addWidget(self.pause_button)
        button_layout.addWidget(self.stop_button)
        
        controls_layout.addLayout(button_layout)
        
        # Frame slider
        frame_layout = QHBoxLayout()
        frame_layout.addWidget(QLabel("Frame:"))
        
        self.frame_slider = QSlider(Qt.Horizontal)
        self.frame_slider.setMinimum(0)
        self.frame_slider.setMaximum(100)
        self.frame_slider.setValue(0)
        self.frame_slider.valueChanged.connect(self._on_frame_slider_changed)
        
        self.frame_spinbox = QSpinBox()
        self.frame_spinbox.setMinimum(0)
        self.frame_spinbox.setMaximum(100)
        self.frame_spinbox.setValue(0)
        self.frame_spinbox.valueChanged.connect(self._on_frame_spinbox_changed)
        
        frame_layout.addWidget(self.frame_slider)
        frame_layout.addWidget(self.frame_spinbox)
        
        controls_layout.addLayout(frame_layout)
        
        # Speed control
        speed_layout = QHBoxLayout()
        speed_layout.addWidget(QLabel("Speed:"))
        
        self.speed_spinbox = QDoubleSpinBox()
        self.speed_spinbox.setMinimum(0.1)
        self.speed_spinbox.setMaximum(4.0)
        self.speed_spinbox.setSingleStep(0.1)
        self.speed_spinbox.setValue(1.0)
        self.speed_spinbox.setSuffix("x")
        self.speed_spinbox.valueChanged.connect(self._on_speed_changed)
        
        speed_layout.addWidget(self.speed_spinbox)
        speed_layout.addStretch()
        
        controls_layout.addLayout(speed_layout)
        
        layout.addWidget(controls_group)
        
        # Add stretch to push everything to top
        layout.addStretch()
    
    def _on_play_clicked(self):
        """Handle play button click."""
        self.is_playing = True
        self.play_button.setEnabled(False)
        self.pause_button.setEnabled(True)
        self.play_requested.emit()
    
    def _on_pause_clicked(self):
        """Handle pause button click."""
        self.is_playing = False
        self.play_button.setEnabled(True)
        self.pause_button.setEnabled(False)
        self.pause_requested.emit()
    
    def _on_stop_clicked(self):
        """Handle stop button click."""
        self.is_playing = False
        self.play_button.setEnabled(True)
        self.pause_button.setEnabled(False)
        self.current_frame = 0.0
        self.frame_slider.setValue(0)
        self.frame_spinbox.setValue(0)
        self.stop_requested.emit()
    
    def _on_frame_slider_changed(self, value: int):
        """Handle frame slider change."""
        self.current_frame = float(value)
        self.frame_spinbox.blockSignals(True)
        self.frame_spinbox.setValue(value)
        self.frame_spinbox.blockSignals(False)
        self.frame_changed.emit(self.current_frame)
        self._update_labels()
    
    def _on_frame_spinbox_changed(self, value: int):
        """Handle frame spinbox change."""
        self.current_frame = float(value)
        self.frame_slider.blockSignals(True)
        self.frame_slider.setValue(value)
        self.frame_slider.blockSignals(False)
        self.frame_changed.emit(self.current_frame)
        self._update_labels()
    
    def _on_speed_changed(self, value: float):
        """Handle speed change."""
        self.speed_changed.emit(value)
    
    def update_current_frame(self, frame: float):
        """Update current frame display."""
        self.current_frame = frame
        
        # Update slider and spinbox without triggering signals
        self.frame_slider.blockSignals(True)
        self.frame_spinbox.blockSignals(True)
        
        self.frame_slider.setValue(int(frame))
        self.frame_spinbox.setValue(int(frame))
        
        self.frame_slider.blockSignals(False)
        self.frame_spinbox.blockSignals(False)
        
        self._update_labels()
    
    def set_animation_info(self, name: str, total_frames: int, frame_rate: float):
        """Set animation information."""
        self.name_label.setText(f"Animation: {name}")
        self.total_frames = total_frames
        
        self.frame_slider.setMaximum(total_frames)
        self.frame_spinbox.setMaximum(total_frames)
        
        self._update_labels()
    
    def _update_labels(self):
        """Update info labels."""
        self.frame_label.setText(f"Frame: {int(self.current_frame)} / {self.total_frames}")
        
        # Assuming 30 FPS for now
        current_time = self.current_frame / 30.0
        total_time = self.total_frames / 30.0
        self.time_label.setText(f"Time: {current_time:.2f}s / {total_time:.2f}s")
