#!/usr/bin/env python3
"""
THUG 3D Model Viewer & Skeleton/Animation Editor

A standalone tool for viewing, editing, and analyzing 3D character models,
skeletons, and animations from Tony Hawk's Underground.

Usage:
    python model_viewer.py [--model PATH] [--anim PATH] [--skeleton PATH]
"""

import sys
import argparse
from PyQt5.QtWidgets import QApplication
from ui.main_window import MainWindow


def parse_arguments():
    """Parse command line arguments."""
    parser = argparse.ArgumentParser(
        description='THUG 3D Model Viewer & Skeleton/Animation Editor'
    )
    
    parser.add_argument(
        '--model',
        type=str,
        help='Path to model file (.mdl)',
        default=None
    )
    
    parser.add_argument(
        '--skeleton',
        type=str,
        help='Path to skeleton file',
        default=None
    )
    
    parser.add_argument(
        '--anim',
        type=str,
        help='Path to animation file (.ska)',
        default=None
    )
    
    parser.add_argument(
        '--fullscreen',
        action='store_true',
        help='Start in fullscreen mode'
    )
    
    parser.add_argument(
        '--wireframe',
        action='store_true',
        help='Start in wireframe mode'
    )
    
    return parser.parse_args()


def main():
    """Main entry point for the application."""
    args = parse_arguments()
    
    # Create Qt application
    app = QApplication(sys.argv)
    app.setApplicationName('THUG Model Viewer')
    app.setOrganizationName('THUG Project')
    
    # Create and show main window
    window = MainWindow()
    
    # Apply command line options
    if args.fullscreen:
        window.showFullScreen()
    else:
        window.show()
    
    if args.wireframe:
        window.set_wireframe_mode(True)
    
    # Load files if specified
    if args.model:
        window.load_model(args.model)
    
    if args.skeleton:
        window.load_skeleton(args.skeleton)
    
    if args.anim:
        window.load_animation(args.anim)
    
    # Run application
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
