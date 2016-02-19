This assignment consists of one program.
        - a6

Features:

	- Loads bunny_59k.smf simple mesh file
	- Places one fixed white light front left
	- Places one adjustable light which can be changed with keys u, j, i, k, o, l
	- Calculates the normal for each face and adds it to the vertex normal of that face
	- Implements Phong Model
	- Implements Phong and Gouraud algorithm.
	- Can toggle between both shading algorithms.
	- Camera Rotates around the object when toggled
	- Camera can be adjusted by six keys (see a4 print out for details)
	- Allows the user to choose between parallel and perspective projections.
	- Can choose between three different materials Emerald, Green Plastic, Green Rubber
	- Camera light that follows the camera and shines at the model.

How to Run:

 make
        To compile all of source files

 make run
        To run the program.

 make clean
        Cleans all artifacts.
