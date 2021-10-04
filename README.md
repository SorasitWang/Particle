# Particle

Simulate particles movement in box both in 2D and 3D which can change the environment parameters.

## Feature
 - Particle collision both with another and walls 
 - Using Imgui library to allow player to change some paremeters : G , Box size , Liftime , Box Friction , Velocity loss when collision
 - Add new Particle by specify position and velocity (only in 2D)
 - Drawing effect when collide with Box wall (only in 3D)
 - Can chaning between 2D - 3D mode , Edit - Not edit mode
 - Moving and rotating camera to walkaround.
 
## How to use
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

## Parameter
 - Friction : friction on the wall
 - G : gravity
 - LifeTime : particle's transparency depend on remaining time
 - VelocityLostWall : percentage of velocityBeforeCollisonWithWall / velocityAfterCollisonWithWall
 - VelocityLostBall : percentage of velocityBeforeCollisonWithParticle / velocityAfterCollisonWithPaticle
## Preview

https://user-images.githubusercontent.com/60053627/135568601-b96e4fb0-32c1-48de-a9a4-285fa580fd8d.mp4



https://user-images.githubusercontent.com/60053627/135568609-ab538ede-7719-4608-950d-2ac7846d7fd5.mp4


