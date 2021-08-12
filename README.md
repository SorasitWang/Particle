# Particle

Simulate particles movement in box both in 2D and 3D which can change the environment parameters.

Feature
 - Particle collision both with another and walls 
 - Using Imgui library to allow player to change some paremeters : G , Box size , Liftime , Box Friction , Velocity loss when collision
 - Add new Particle by specify position and velocity (only in 2D)
 - Drawing effect when collide with Box wall (only in 3D)
 - Can chaning between 2D - 3D mode , Edit - Not edit mode
 - Moving and rotating camera to walkaround.
 
How to use
 - TAB to change between 2D - 3D mode (2D is defalut). It will recreate particles.
 - SPACE to change between Edit - None edit mode (None edit is default). 
 - W S : increase - decrease size of box along y axis.
   A D : increase - decrease size of box along x axis.
   Q E : increase - decrease size of box along Z axis (only in 3D).
 - UP DOWN : move camera to forward - backward.
   LEFT RIGHT : move camera to left - right.
 - MOUSE : 
       Edit mode : change the parameter , clicking to add new particle and draging to set the direction and velocity. 
       None edit mode : change the view.
