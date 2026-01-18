
# Table of Contents

-   [RandWalk](#randwalk)
-   [Demo](#demo)
-   [Installation](#installation)
    -   [Dependencies](#installation-dependencies)
        -   [macOS](#installation-dependencies-macos)
        -   [Ubuntu/Debian](#installation-dependencies-ubuntu-debian)
        -   [Fedora](#installation-dependencies-fedora)
    -   [Building](#installation-building)
    -   [Running](#installation-running)
-   [Controls](#controls)
-   [Features](#features)



<a id="randwalk"></a>

# RandWalk

A mesmerizing random walk visualization built with SDL2. Multiple colorful particles traverse the screen in perpendicular random walks, creating abstract generative art.


<a id="demo"></a>

# Demo

Particles start from the center and walk in perpendicular directions (alternating between horizontal and vertical movement), changing color on boundary collisions.


<a id="installation"></a>

# Installation


<a id="installation-dependencies"></a>

## Dependencies

-   SDL2


<a id="installation-dependencies-macos"></a>

### macOS

    brew install sdl2


<a id="installation-dependencies-ubuntu-debian"></a>

### Ubuntu/Debian

    sudo apt-get install libsdl2-dev


<a id="installation-dependencies-fedora"></a>

### Fedora

    sudo dnf install SDL2-devel


<a id="installation-building"></a>

## Building

    gcc -o randwalk main.c $(sdl2-config --cflags --libs) -lm

Or with a Makefile:

    make


<a id="installation-running"></a>

## Running

    ./randwalk


<a id="controls"></a>

# Controls

<table border="2" cellspacing="0" cellpadding="6" rules="groups" frame="hsides">


<colgroup>
<col  class="org-left" />

<col  class="org-left" />
</colgroup>
<thead>
<tr>
<th scope="col" class="org-left">Key</th>
<th scope="col" class="org-left">Action</th>
</tr>
</thead>
<tbody>
<tr>
<td class="org-left"><code>0</code></td>
<td class="org-left">Clear screen</td>
</tr>

<tr>
<td class="org-left"><code>c</code></td>
<td class="org-left">Toggle periodic screen clearing</td>
</tr>

<tr>
<td class="org-left"><code>p</code></td>
<td class="org-left">Increase particle size</td>
</tr>

<tr>
<td class="org-left"><code>Shift + p</code></td>
<td class="org-left">Decrease particle size</td>
</tr>

<tr>
<td class="org-left"><code>v</code></td>
<td class="org-left">Increase particle velocity</td>
</tr>

<tr>
<td class="org-left"><code>Shift + v</code></td>
<td class="org-left">Decrease particle velocity</td>
</tr>

<tr>
<td class="org-left">Close window</td>
<td class="org-left">Quit</td>
</tr>
</tbody>
</table>


<a id="features"></a>

# Features

-   Fullscreen display at native resolution
-   50 particles with independent random walks
-   Color changes on boundary collision
-   Configurable particle size and velocity

