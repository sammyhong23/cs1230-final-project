# Fall 2022 CSCI1230 Final Project
## No Free Lunch (jcai20, spyon, jrhee6, shong41)

### Overall Idea
We wanted to simulate flowing water textures using procedurally generated heightmaps and flowmaps.

### Technical Features
Procedural generation of height maps for the water ripples. <br>
Flow mapping to incorporate movement into the water. <br>
Bump mapping to simulate the texture of water. <br>
Bezier curves for camera movement during our demo. <br>

### Implementation
To display our water texture implementation, we had to use a scene file to instantiate an object and light to see our texture in action. <br><br>
Our pipeline first involves generating a height map. The default noise used for the height map is Perlin noise, but the Worley noise toggle will change the height map to be constructed from Worley noise. There are two parameters for generating the height map as well: Parameter 1 changes the frequency of noise and Parameter 2 changes the strech of the noise in the X direction. The height mapping code was written to an FBO and used as a texture that is applied to the surface of objects. <br><br>
Next, we applied flow mapping to the generated height map in order to imitate the movement of water. The flow maps contain the x and y values of the direction to change the uv-coordinate values of the texture, stored in the R and G channels of the flow map image. We use different flow maps for different water flow directions. To prevent the appearance of looping and make the flow appear more smooth, we blended the original flow map with an offset of itself.  <br><br>
The third step of our pipeline involves applying the height map onto the surface of the objects. These height map perturbs the normals of the object surface in order to imitate the appearance of bumps/dips in the water. To implement this feature, we added uv coordinates to each vertex of each shape as well as a U direction vector indicating the direction that the u-coordinate in the texture map increases on the surface of the object. We obtained the normal at each vertex in tangent space by averaging the cross products with vectors to nearby height points. These tangent-space normals were then transformed into object space and then into world space for use in our Phong illumination model. Bump mapping can be turned on by toggling the Bump Mapping toggle in the GUI.<br><br>
To have smooth camera movement, we implemented Bezier curves. Our bezier function takes in 4 points and produces a vector of positional points. When we press the B key, we loop through the vector of points and update our camera position.

### Usage
1. The Worley noise toggle will change the height map to be constructed from Worley noise (instead of the default Perlin noise). There are two parameters for generating the height map as well: Parameter 1 changes the frequency of noise and Parameter 2 changes the strech of the noise in the X direction, both of which are sliders in the GUI. <br>
2. Flow mapping can be included in the demo by toggling the Flow mapping toggle in the GUI. We also stored the flow speed as a variable that can be changed using the flow speed slider in the GUI. To change what flow map is used in the texture, edit the filepath to the flow map image in the realtime.cpp file around line 113 (different flow map images are located in the resources/images/). <br>
3. The bump map feature can be toggled using the Bump mapping toggle in the GUI.
4. The user can traverse the curve by using the Bezier Curve toggle in the GUI, disabling the usage of the W, A, S, and D keys to move the camera. The B key will allow the user to traverse the Bezier curve, and the F key will snap the camera back to the starting position of the Bezier curve. 
