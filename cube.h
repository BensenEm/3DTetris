#pragma once
#include "color.h"
#include <glm/glm.hpp>

using namespace glm;

const int xLen = 6;
const int yLen = 12;
const int zLen = 6;

struct cube {
	vec3 koordinate;
	color col;

	cube::cube(int x, int y, int z, color c) {
		koordinate = vec3(x, y, z);
		col = c;

	}

	cube::cube() {
		koordinate = vec3(xLen, yLen, zLen);
		this->col = transparent;

	}

	vec3 getKoor() {
		return koordinate;
	};

	void setKoor(int x, int y, int z) {
		koordinate = vec3(x, y, z);
	};

	void cube::transform(char achse, bool uhrzeigersinn, cube* anker, vec3 *diff) {
		//vec3 *diff = new vec3();
		*diff = this->koordinate - anker->koordinate;
		vec3 temp = *diff;

		if (achse == 'x' && uhrzeigersinn == true) {
			diff->y = diff->z;
			diff->z = -1 * temp.y;

		}
		else if (achse == 'x' && uhrzeigersinn == false) {
			diff->y = -1 * diff->z;
			diff->z = temp.y;
		}
		else if (achse == 'y' && uhrzeigersinn == true) {
			diff->x = diff->z;
			diff->z = -1 * temp.x;

		}
		else if (achse == 'y' && uhrzeigersinn == false) {
			diff->x = -1 * diff->z;
			diff->z = temp.x;
		}
		else if (achse == 'z' && uhrzeigersinn == true) {
			diff->y = diff->x;
			diff->x = -1 * temp.y;

		}
		else {
			diff->y = -1 * diff->x;
			diff->x = temp.y;
		};
		//return *diff;
	};
};