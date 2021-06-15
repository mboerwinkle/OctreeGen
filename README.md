# OctreeGen

![Octree Hubble; Rendered with FSTL](https://789012.xyz/octreegen/hubble.png)

Generates an octree from an stl file. Inefficient, but extremely robust especially with regards to non-watertight meshes.

Supports export as `.stl` (as shown above) or a raw octree format.
## Compiling
Run `make`.

For a debug build, run `make clean; make BUILD=debug`.
## Usage
See `./octreegen --help` for full details.
To generate the hubble model shown, run:

`wget https://789012.xyz/octreegen/hubble.stl;`

`./octreegen -i "hubble.stl" -o "hubbleoct.stl" -r 10 -s 1740`
