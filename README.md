# godefarig
a (kind of) spherical terrain generator

The aim of this project is to experiment with procedural generation in order to create a system of planet creation.

# current state

It is currently able to create elevation and coloration on the cpu side.
It uses an octahedral base, each triangular face being divised into four smaller triangles at each iteration.

# long term goals

- perform calculations on the GPU
- add a subduction/erosion mecanism to generate the elevations

# dependencies

You will need boost installed in order to compile this program. (last version should be fine)
This program also uses glfw3. A source version is provided but I would advise installing it if possible.

# used libraries

In addition to boost and glfw3, this program uses the following libraries :
- glm
- glad

# compilation (out-of-source build)

From the build/ folder, launch the following commands :
`cmake ..`
`make -j6 gltest`
(the -j6 option is only here to make the compilation time shorter on multicore machines)

# usage

The res/ folder contains a number of configurations (.gfg files) which can be used as such :
`./gltest --config res/name_of_the_file.gfg`
alternatively, the option --help outputs a description of the main options

# licence

The licence is currently GPLv3, because I like the notion of copyleft.
Yet I feel that the intricacies of software licencing are lost on me, so this might change when I'll know more about it.
