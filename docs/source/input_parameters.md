# Input parameters

`sqsgenerator` uses a `dict`-like configuration to find all the value it needs. Thus it can it read its configuration from anything which can store Python `dict`s (e. g. `json`, `yaml` or `pickle`). However by default `sqsgenerator` expects an input file in **YAML** format. 

Each of the parameters below represents an entry in the **YAML** (or key in a `dict` if called directly from Python).

(The order corresponds to what is parsed in `sqsgenerator.settings.py`)

---

## Parameters

### `atol`

Absolute tolerance for calculating the default distances of the coordination shells (unit $\text{\AA}$). `atol` and `rtol` are used to determine if two numbers are close.

- **Required:** No
- **Default**: `1e-3`
- **Accepted:** positive floating point numbers (`float`)

### `rtol`

relative tolerance for calculating the default distances of the coordination shells (unit $\text{\AA}$). `atol` and `rtol` are used to determine if two numbers are close.

- **Required:** No
- **Default**: `1e-5`
- **Accepted:** positive floating point numbers (`float`)

````{hint}
Have a look on the `shell_distances` parameter, by entering:

  ```{code-block} bash
  sqsgenerator params show input.yaml --param shell_distances
  ```
In case you get some distances which are really close e. g. 4.12345 and 4.12346 it is maybe a good idea to increase `rtol` and/or `atol` such that `sqsgenerator` groups them into the same coordination shell
````

### `max_output_configurations`

The maximum number of output configurations. 

- **Required:** No
- **Default:** 10
- **Accepted:** positive finite integer number (`int`)

```{note}
In case the code runs with MPI parallelization, **each MPI rank** will generate at most `max_output_configurations` which are gathered at the head rank.
```

### `composition` 

The composition of the output configuration, defined as an dictionary.  Keys are symbols of chemical elements, whereas values are the number of atoms of the corresponding species. 

- **Required:** Yes
- **Accepted:** a dictionary with chemical symbols as keys (`dict[str, int]`) 

```{note}
The sum of the atoms distributed must **exactly** match the number of positions on the lattice 
```

#### Examples
- Ternary alloy, consisting of 54 atoms ($\text{Ti}_{18}\text{Al}_{18}\text{Mo}_{18}$)

    ```{code-block} yaml
    composition:
      Ti: 18
      Al: 18
      Mo: 18
    ```

- *fcc*-Aluminum cell,  64 atoms, randomly distribute  8 vacancies

    ```{code-block} yaml
    composition:
      Al: 56
      0: 8
    ```

### `composition.which`

Used to select a sublattice (collection of lattice sites) from the specified input structure. Note that the number of atoms in the `composition` paramter has to sum up to the number of selected lattice positions

- **Required:** No
- **Default:** *all*
- **Accepted:**
  - either *all* or a chemical symbol specified in the input structure (`str`)
  - list of indices to choose from the input structure (`list[int]`)

```{note} 
The sum of the atoms distributed must **exactly** match the number of **selected** lattice positions. 
```

#### Examples
- Ternary alloy, 54 atoms, create ($\text{Ti}_{18}\text{Al}_{18}\text{Mo}_{18}$)
  
    ```{code-block} yaml
    composition:
      which: all
      Ti: 18
      Al: 18
      Mo: 18
    ```
    
- *rock-salt* TiN (B1),  64 atoms, randomly distribute B and N on the N sublattice $\text{Ti}_{32}(\text{B}_{16}\text{N}_{16}) = \text{Ti}(\text{B}_{0.5}\text{N}_{0.5})$
  
    ```{code-block} yaml
    composition:
      which: N
      N: 16
      B: 16
    ```
    
- *rock-salt* TiN (B1),  64 atoms, randomly distribute Al, V and Ti on the Ti sublattice $(\text{Ti}_{16}\text{Al}_{8}\text{V}_{8})\text{N}_{32} = (\text{Ti}_{0.5}\text{Al}_{0.25}\text{V}_{0.25})\text{N}$
  
    ```{code-block} yaml
    composition:
      which: Ti
      Ti: 16
      Al: 8
      V: 8
    ```
    
- select all **even** sites from your structure, 16 atoms, using a index, list and distribute W, Ta and Mo on those sites
  
    ```{code-block} yaml
    composition:
      which: [0, 2, 4, 6, 8, 10, 12, 14]
      W: 3
      Ta: 3
      Mo: 2
    ```

### `structure`

the structure where `sqsgenerator` will operate on. `composition.which` will select the sites from the specified structure. The coordinates must be supplied in **fractional** style. It can be specified by supplying a filename or directly as a dictionary

- **Required:** Yes
- **Accepted:**
  - dictionary with a `file` key (`dict`)
  - dictionary with a `lattice`, `coords` and `species` key (`dict`)

```{note}
  - If a filename is specified, and `ase` is available `sqsgenerator` will automatically use it to load the structure using [`ase.io.read`](https://wiki.fysik.dtu.dk/ase/ase/io/io.html#ase.io.read). Alternatively it will fall back to `pymatgen` ( [`pymatgen.core.IStructure.from_file`](https://pymatgen.org/pymatgen.core.structure.html#pymatgen.core.structure.IStructure.from_file)). If both packages are not available it will raise an `FeatureError`.
  - You can explicitly instruct to use one of the packages by settings `structure.reader` to either *ase* or *pymatgen*
  - You can pass custom arguments to the reader function ( [`ase.io.read`](https://wiki.fysik.dtu.dk/ase/ase/io/io.html#ase.io.read) or [`pymatgen.core.IStructure.from_file`](https://pymatgen.org/pymatgen.core.structure.html#pymatgen.core.structure.IStructure.from_file)) by specifying `structure.args` (last example)
```

#### Examples

- directly specify $\text{CsCl}$ (B2) structure in the input file

    ```{code-block} yaml
    structure:  
      lattice:
        - [4.123, 0.0, 0.0]
        - [0.0, 4.123, 0.0]
        - [0.0, 0.0, 4.123]
      coords: # put fractional coordinates here -> not cartesian
        - [0.0, 0.0, 0.0]
        - [0.5, 0.5, 0.5]
      species:
        - Cs
        - Cl
    ```
    Please note that for each entry in `coords` there must be an corresponding species specified in the `species` list
    
-  specify a file (must be readable by `ase.io.read` , fallback to `pymatgen` if `ase` is not present)
   
    ```{code-block} yaml
    structure:
      file: cs-cl.vasp # POSCAR/CONTCAR format
    ```
    
- specify a file and explicitly set a reader for reading the structure file
  
    ```{code-block} yaml
    structure:
       file: cs-cl.cif
       reader: pymatgen # use pymatgen to read the CIF file
    ```
  
- specify read a file and  pass arguments to the reading package. E. g. read las configuration from a MD-trajectory
  
    ```{code-block} yaml
    structure:
      file: md.traj
      reader: ase
      args:
        index: -1
    ```
    if `args` is present in will be unpacked (`**`) into `ase.io.read`


### `structure.supercell`

Instructs `sqsgenerator` to create a supercell of the the specified structure

- **Required:** No
- **Accepted:** a list/tuple of positive integer number of length 3 (`tuple[int]`)

#### Examples

- Create a $3\times3\times3$ supercell of the $\text{CsCl}$ (B2) structure

    ```{code-block} yaml
    structure:
      supercell: [3, 3, 3]
      lattice:
        - [4.123, 0.0, 0.0]
        - [0.0, 4.123, 0.0]
        - [0.0, 0.0, 4.123]
      coords: # put fractional coordinates here -> not cartesian
        - [0.0, 0.0, 0.0]
        - [0.5, 0.5, 0.5]
      species:
        - Cs
        - Cl
    ```

- Create a  $3\times3\times3$ supercell of a structure file

    ```{code-block} yaml
    structure:
      supercell:
        - 3
        - 3
        - 3
      file: cs-cl.cif

### `mode`

The iteration mode specifies how new structures are generated. 

- *random* the configuration will be shuffled randomly

- *systematic* will instruct the code generate configurations in lexicographical order and to scan the **complete configurational space**. In case *systematic* is specified the `iterations` parameter will be ignored, since the number of permutations is predefined. Therefore for a system with $N$ atoms with $M$ species, will lead to

$$
N_{\text{iterations}} = \dfrac{N!}{\prod_m^M N_m!} \quad \text{where} \quad \sum_m^M N_m = N
$$

- **Required:** No
- **Default:** *random*
- **Accepted:** *random* or *systematic* (`str`)

````{hint}
- In case you use the *systematic* make sure that the total number of configurations to check is a number a computer can check in finite times. To compute the total number of permutations you can use:
```{code-block} bash
sqsgenerator compute total-permutations
```
- `sqsgenerator` also allows you to guess how long it might take to check a certain number of iterations:
```{code-block} bash
sqsgenerator compute estimated-time
```
````

### `iterations`
number of configurations to check. This parameter is ignored if `mode` was set to *systematic*

- **Required:** No
- **Default:** $10^5$ if `mode` is *random*
- **Accepted:** a positive integer number (`int`)

### `shell_distances`
the radii of the coordination shells in Angstrom. All lattice positions will be binned into the specified coordination shells

- **Required:** No
- **Default:** automatically determined by `sqsgenerator`
- **Accepted:** a list of positive floating point numbers (`list[float]`)

````{hint}
You can have a look at the the computed shell distances, and check if they are fine using:
```{code-block} bash
sqsgenerator params show input.yaml -p shell_distances
```
````

### `shell_weights`
accounts for the fact that coodination shells which are farther away are less important. This parameter also determines 
**which** shells should be taken into account. It corresponds to $w^i$ {eq}`eqn:objective` in the objective function eq}`eqn:objective`.

- **Required**: No
- **Default**: $\frac{1}{i}$ where $i$ is the index of the coordination shell. Automatically determined by `sqsgenerator`
- **Accepted:** a dictionary where keys are the shell indices and the values $w^i$ parameters (`dict[int, float]`)

````{note}
- If nothing is specified **all available** coordination shells are used
- You can improve the performace of `sqsgenerator` by neglecting some coordination shells
- If you specify a shell index which does not exist a `BadSettings` error will be raised
- If a shell index is missing the coordination shell is not taken into accoutn for the optimization
- You can have a look on the generated weights by checking
```{code-block} bash
sqsgenerator params show input.yaml -p shell_weights
```
````

#### Examples
To consider all coordination shells, simply do not specify any value

- Only use the first coordination shell
  
  ```{code-block} yaml
  shell_weights:
    1: 1.0
  ```

- Use first and second coodination shell
  
  ```{code-block} yaml
  shell_weights:
    1: 1.0 # this are the default values anyway
    2: 0.5
  ```
  
### `pair_weights`
thr "*pair weights*" $p_{\xi\eta}$  {eq}`eqn:objective` used to differentiate bonds between atomic species.
Note that `sqsgenerator` sorts the atomic species interally in ascending order by their ordinal number.
Please refer to the `target_objective` parameter documentation for further details regarding the interal reordering.

- **Required:** No
- **Default:** an array of **ones** of shape $\left(N_{\text{species}}, N_{\text{species}} \right)$
- **Accepted:**  a 2D matrix of shape $\left( N_{\text{species}}, N_{\text{species}} \right)$ (`np.ndarray`)

### `target_objective`
the target objective $\alpha'_{\eta\xi}$ {eq}`eqn:objective`, which the SRO parameters {eq}`eqn:wc-sro-multi` are minimzed against. It is an array of three-dimensions of shape $\left( N_{\text{shells}}, N_{\text{species}}, N_{\text{species}} \right)$. By passing custom values you can fine-tune the individual SRO paramters.

- **Requrired:** No
- **Default:** an array of **zeros** of shape $\left( N_{\text{shells}}, N_{\text{species}}, N_{\text{species}} \right)$
- **Accepted:**
  - a single scalar value. An array filled with the scalar value of shape $\left( N_{\text{shells}}, N_{\text{species}}, N_{\text{species}} \right)$ will be created (`float`)
  - a 2D matrix of shape $\left( N_{\text{species}}, N_{\text{species}} \right)$ the matrix will be stacked along the first dimension $N_{\text{shells}}$ times to generate the $\left( N_{\text{shells}}, N_{\text{species}}, N_{\text{species}} \right)$ array (`np.ndarray`)
  - a 3D array of shape $\left( N_{\text{shells}}, N_{\text{species}}, N_{\text{species}} \right)$ (`np.ndarray`)

````{note}
-  because of $\alpha'_{\eta\xi} = \alpha'_{\xi\eta}$ the `target_objective` is a **symmetric** quantity. Thus in case an  $\alpha_{\eta\xi}^{'T} \neq \alpha'_{\xi\eta}$ an `BadSettings` error is raised
-  the atomic species specified in `strcuture` by their ordinal number in **ascending order** 
    - In case of the $\text{CsCl}$ the actual ordering is $\text{Cl}(Z=17), \text{Cs}(Z=55)$.
    ```{math}
    \boldsymbol{\alpha}' = \left[
    \begin{array}{cc}
    \alpha_{\text{Cl-Cl}} & \alpha_{\text{Cl-Cs}} \\
    \alpha_{\text{Cs-Cl}} & \alpha_{\text{Cs-Cs}}
    \end{array}
    \right]
    ```
    - In case of the $\text{TiAlMo}$ the actual ordering is $\text{Al}(Z=13), \text{Ti}(Z=22), \text{Mo}(Z=42)$.
    ```{math}
    \boldsymbol{\alpha}' = \left[
    \begin{array}{ccc}
    \alpha_{\text{Al-Al}} & \alpha_{\text{Al-Ti}} & \alpha_{\text{Al-Mo}} \\
    \alpha_{\text{Ti-Al}} & \alpha_{\text{Ti-Ti}} & \alpha_{\text{Ti-Mo}} \\
    \alpha_{\text{Mo-Al}} & \alpha_{\text{Mo-Ti}} & \alpha_{\text{Mo-Mo}} \\
    \end{array}
    \right]
    ```
````

#### Examples
- distribute everything randomly
  
  ```{code-block} yaml
  target_objective: 0 # this is the default behaviour
  ```
- search for a clustered $\text{CsCl}$ structure

  ```{code-block} yaml
  target_objective: 1 # which is equivalent to 
  target_objective:
    - [1, 1]
    - [1, 1]
  ``` 
  
- custom settings for a ternary alloy (unknown use case :smile: )

  ```{code-block} yaml
  target_objective:
    - [ 1, -1, 0]
    - [-1,  1, 0]
    - [ 0,  0, 1]
  ``` 

 