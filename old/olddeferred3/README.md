This project was created to test various rendering techniques. Currently stencil shadow volumes and deferred rendering.

TODO:
* Fix shadows
* Implement screen space ambient occlusion
* Implement high dynamic range lighting

ShaderManager:
* Handles shader and program creation/loading
* Reloading shader sources will recompile existing shader objects and relink existing program objects

TextureManager:
* Handles 2D and CubeMap DDS texture loading
* Reloading texture files will update existing texture objects

UniformManager:
* Handles setting of program uniforms
* Checks for unset uniforms
* Checks the correct uniform types are used

GeometryManager:
* Handles geometry loading
* Uses geometry and vertex array layout files to create a vertex array object
* Reloading geometry files or vertex array layout files will update existing vertex array objects

mexport.py:
* Simple Blender exporter for a custom geometry binary format
* Can also export to a corresponding JSON format
* Optionally exports indices adjacency information
* Exports all vertex positions, normals, texcoords, tangents and colours
* Discards duplicate vertices
* Smooth/flat shaded vertices affect the normals
* Exports draw information, organised by meshes and materials

FileMon:
* Cross platform (windows, linux) file monitor
* Notifies of file modifications after a specified delay (helps avoid accessing the file while it is being modified)

Window:
* Cross platform (windows, linux) window and OpenGL (3.0-3.3) context creation
* TODO : input callbacks and some window notifications for linux

Math3d:
* Some simple vec3 and mat4 operations
* Template based, able to specify base type (e.g. float, double)

Keyframer:
* Simple key frame animation, includes smoothing

MouseSmooth:
* Smooths mouse input

Font:
* Renders truetype fonts
* Needs a lot of cleaning up