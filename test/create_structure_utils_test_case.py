import io
import sys
import math
import itertools
import numpy as np
from pymatgen.io.vasp import Poscar
from pymatgen.util.coord import pbc_shortest_vectors

def write_array(stream, array: np.ndarray, name=None, fmt="{0:.8f}"):
    stream.write(f"{name}::array::begin\n")
    stream.write(f"{name}::array::ndims {len(array.shape)}\n")
    stream.write(f"{name}::array::shape {' '.join(map(str, array.shape))}\n")
    stream.write(f"{name}::array::data")
    for v in array.flat: stream.write(" " + fmt.format(v))
    stream.write("\n")
    stream.write(f"{name}::array::end\n")


def nditer(A: np.ndarray):
    return itertools.product(*map(range, A.shape))

ABS_TOL = 1.0e-5
REL_TOL = 1.0e-8

if __name__ == "__main__":
    try:
        _, name, poscar_path = sys.argv
    except ValueError:
        print("I'm expecting exactly two arguments, namely a test case name and a path to a POSCAR file")
        sys.exit()
    structure = Poscar.from_file(poscar_path).structure
    l = structure.lattice.matrix
    d2 = structure.distance_matrix
    fc = structure.frac_coords
    vecs = pbc_shortest_vectors(structure.lattice, fc, fc)

    s = np.zeros_like(d2, dtype=int)
    heights = {0.0: []}

    shell_border_nearby = lambda x: any(math.isclose(x, h, rel_tol=REL_TOL, abs_tol=ABS_TOL) for h in heights)
    closest_shell = lambda x: min(heights.keys(), key=lambda h : abs(x -h))

    for i, j in nditer(d2):
        d = d2[i ,j]
        if shell_border_nearby(d): heights[closest_shell(d)].append(d)
        else: heights[d] = [d]
        if j < i: continue

    shell_dists = [max(heights[k]) for k in sorted(heights.keys())]

    print(shell_dists)
    for i, j in nditer(d2):
        d = d2[i ,j]
        actual_shell = min(shell_dists, key=lambda x: abs(x-d))
        assert abs(actual_shell - d) <= 2.0*ABS_TOL
        s[i,j] = shell_dists.index(actual_shell) + 1
        if i == j: s[i, j] = 0


    with open(f"{name}.test.data", "w") as stream:
        write_array(stream, l, "lattice")
        write_array(stream, fc, "fcoords")
        write_array(stream, d2, "distances")
        write_array(stream, s, "shells", fmt="{0}")
        write_array(stream, vecs, "vecs")

