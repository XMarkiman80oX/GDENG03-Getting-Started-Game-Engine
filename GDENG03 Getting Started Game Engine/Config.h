#pragma once

struct Config
{
	enum cubeColor {
		NONE = -1,
		WHITE,
		RAINBOW
	};

	static const int NUMBER_OF_CUBES = 1;
	static const bool RANDOMIZE_OBJECT_POSITIONS = true;
	static const bool ENABLE_CUBE_MOVEMENT = false; //Enables the movement of the Cube via WASD (For Item #1)
	static const cubeColor CUBE_COLOR = WHITE; //Selects the color of the cube to be rendered (USE THE GIVEN ENUM, 'cubeColor')
	static const bool ENABLE_FPS_CAMERA = true; //Enables or Disables the mouse movement for the camera

	static const bool ROTATE_AROUND_X = true;
	static const bool ROTATE_AROUND_Y = true;
	static const bool ROTATE_AROUND_Z = true;
};