#pragma once

struct Config
{
	enum cubeColor {
		NONE = -1,
		WHITE,
		RAINBOW
	};

	static const int NUMBER_OF_RANDOMIZED_CUBES = 50;
	static const bool RENDER_HOUSE_OF_CARDS = true;
	static const bool RENDER_CUBES_WITH_PLANE = false;
	static const bool RANDOMIZE_OBJECT_POSITIONS = false;
	//static const bool OBJECT_POSITIONS_PREDEFINED = true;
	static const bool ENABLE_CUBE_MOVEMENT = false; //Enables the movement of the Cube via WASD (For Item #1)
	static const bool ENABLE_WARPING_ANIM = false; 
	static const bool ENABLE_INTERPOLATING_SCALE = false;
	static const cubeColor CUBE_COLOR = WHITE; //Selects the color of the cube to be rendered (USE THE GIVEN ENUM, 'cubeColor')
	static const bool ENABLE_FPS_CAMERA = true; //Enables or Disables the mouse movement for the camera

	static const bool ROTATE_AROUND_X = false;
	static const bool ROTATE_AROUND_Y = false;
	static const bool ROTATE_AROUND_Z = false;
};