MeasLoader
==========

This ros-node reads the data contained in a file called "data/measdata.dat" and publishes the contained measurements at the specified times.
Its structure is kept very simple and it mainly serves debugging purposes.

INSTALLATION:
- add directory to ROS
- enter build folder
- execute cmake: "cmake .."
- compile: "make"

DEPENDENCIES:
- Standard library
- ROS (including sensor_msgs and geometry_msgs)
