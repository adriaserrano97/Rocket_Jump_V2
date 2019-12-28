# Rocket_Jump_V2
## Game can be found on the following repository: https://github.com/adriaserrano97/Rocket_Jump_V2 <br>
Welcome to Rocket Jump, an original platformer game which focus on fast paced scrolling though a level. Combine your jump and your rocket jump to pull off amazing moves! Fly trough the map as quickly as possible and get to the PC at the end of the level: it will take yu to the following one.


## Credits
Platformer game made by 2 students at CITM, Terrassa, Spain: 

José Luís Redondo Tello: <https://github.com/jose-tello> <br>
Adrià Serrano López: <https://github.com/adriaserrano97> <br>

Credit given to "dklon" for the sound effects used. Find them at: https://opengameart.org/users/dklon. <br>
Credit given to "Robert Brooks" for the platforms and stickman art used in this game. Find his work at: https://www.gamedeveloperstudio.com/graphics/viewgraphic.php?item=1l4q737v077s1f8l3t
Credit given to "Omni_Theorem" for the walking alien art used in this game. Find his work at: https://opengameart.org/users/omnitheorem

## Installation

Currently there is no installation required, just open the .exe provided in the download file

## Usage

### General Controls :
	ESC: Close the game
	
### Debug Controls:<br>
	F1/F2 Start from the first/second level 
	F3 Start from the beginning of the current level
	F5 Save the current state 
	F6 Load the previous state 
	F9 To view colliders 
	F10 God Mode 
	F11 Toggle framerate cap at 30 fps
	6 Add coin
	7 Add Life

	Q: Toggle auto / manual control of camera
	TAB: If on manual control of camera, trigger side switch

### Player Controls: <br>
        W: Look up 
        A: Move right
		While jumping, slightly adjust jump reach
        S: Look down 
        D: Move left 
		While jumping, slightly adjust jump reach
	Left mouse click: Create explosion
				-> colliding with player it will lauch it across the map!
				-> colliding with enemies, it will kill them!


## Innovation
	1: Map collisions are declared in Tiled. Our code loads them at the sime time as the map and generates the colliders automatically (the tiles themselves hold that information, as well as a variable which states which type of collider they are). Therefore, we're free to change our maps at will without having to pay mind to the colliders.
	2: Our extra movement mechanic consist of a point-and-click explosion, which generates a collider that launches the player in 8 possible directions (depends on side of impact) 
	3: We designed and implemented a complex camera: Using lerp movement, it behaves as Super Mario World camera in the horizontal axis (camera-window + dual forward focus). On the vertical axis, it behaves as a manual-control, which the added bonus that it automatically follows the player. However, if the players wants to, it can look even further by keeping W/S pressed. Once released, the camera will lerp it's way back to an acceptable position. 


## Team roles:
José Tello: Rocket Jump implementation, state matrix managment, memory managment, player collisions, debug implementation, particles.<br>

Adrià Serrano: Jump implementation, art adjustment, Map design, map collisions, camera, player fall managment, sound music and sound sfx, player-map navegation. UI: Atlas creation, sound managment, intro menu configuration, ingame UI.<br>

DISCLAIMER: Mostly everything was done in cooperation. The statements above reflect who was the one to take responsability for the section, as well as supervision and final implementation.<br>


## Worthwhile considerations:
V2 was created because V1's poor repository managment caused unsolvable issues (gitignore was not implemented currently, causing certain files to corrupt if opened with certain versions of VisualStudio).


## License

SDL license:<br>
Simple DirectMedia Layer<br>
Copyright (C) 1997-2016 Sam Lantinga <slouken@libsdl.org><br>
SDL_image:  An example image loading library for use with SDL<br>
Copyright (C) 1997-2013 Sam Lantinga <slouken@libsdl.org><br>
SDL_mixer:  An audio mixer library based on the SDL library<br>
Copyright (C) 1997-2013 Sam Lantinga <slouken@libsdl.org><br>

  
This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:
  
1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required. 
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.



