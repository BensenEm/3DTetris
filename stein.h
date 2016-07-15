#include "cube.h"
#include <glm\glm.hpp>
#include <vector>

using namespace irrklang;
using namespace glm;

ISoundEngine *SoundEngine;
bool gameover = false;



std::array< std::array< std::array<color, zLen >, yLen >, xLen > schacht_array;			//der hauptschacht der auch gezeichnet wird
std::array< std::array< std::array<color, zLen >, yLen >, xLen > uebergangs_schacht;	//der schacht in dem vollständige linien gelöscht sind (ohne nachrücken)
std::array< std::array< std::array<color, zLen >, yLen >, xLen > fresh_schacht;			//der schacht nach aktualisierung 
std::array< std::array< std::array<color, zLen >, yLen >, xLen > old_schacht;			//der schacht vor beginn einer löschaktion 


//dropShadow();
struct shadow {
	std::vector<cube> shadow_elems;
};

struct stein {
	std::vector<cube> cube_elems;
	shadow shadowstone;
	shadow helpstone;
	int diffToFloor;


	stein::stein(int type, color col) {
		switch (type) {
		case 1: //I - stein 
			cube_elems.push_back(cube((xLen / 2) - 1, yLen, zLen / 2, col));
			cube_elems.push_back(cube((xLen / 2), yLen, zLen / 2, col));
			cube_elems.push_back(cube((xLen / 2) + 1, yLen, zLen / 2, col));
			cube_elems.push_back(cube((xLen / 2) + 2, yLen, zLen / 2, col));
			break;

		case 2: //L - stein
			cube_elems.push_back(cube((xLen / 2) - 1, yLen - 1, zLen / 2, col));
			cube_elems.push_back(cube((xLen / 2) - 1, yLen, zLen / 2, col));
			cube_elems.push_back(cube((xLen / 2), yLen, zLen / 2, col));
			cube_elems.push_back(cube((xLen / 2) + 1, yLen, zLen / 2, col));
			break;

		case 3: //S - stein
			cube_elems.push_back(cube((xLen / 2) - 1, yLen - 1, zLen / 2, col));
			cube_elems.push_back(cube((xLen / 2), yLen - 1, zLen / 2, col));
			cube_elems.push_back(cube((xLen / 2), yLen, zLen / 2, col));
			cube_elems.push_back(cube((xLen / 2) + 1, yLen, zLen / 2, col));
			break;

		case 4: //O - stein
			cube_elems.push_back(cube((xLen / 2) - 1, yLen - 1, zLen / 2, col));
			cube_elems.push_back(cube((xLen / 2), yLen - 1, zLen / 2, col));
			cube_elems.push_back(cube((xLen / 2) - 1, yLen, zLen / 2, col));
			cube_elems.push_back(cube((xLen / 2), yLen, zLen / 2, col));
			break;

		case 5: //T-stein
			cube_elems.push_back(cube((xLen / 2), yLen - 1, zLen / 2, col));
			cube_elems.push_back(cube((xLen / 2) - 1, yLen, zLen / 2, col));
			cube_elems.push_back(cube((xLen / 2), yLen, zLen / 2, col));
			cube_elems.push_back(cube((xLen / 2) + 1, yLen, zLen / 2, col));

			break;
		};
		//printf("blub");
		updateShadow();

		for (std::vector<cube>::iterator it = cube_elems.begin(); it != cube_elems.end(); ++it) {
			if (schacht_array[it->koordinate.x - 1][it->koordinate.y - 1][it->koordinate.z - 1] != transparent) {
				if (!gameover) {
					gameover = true;
					SoundEngine->play2D("irrKlang-1.5.0/media/awww.wav", false, false, false);

				}
			}
		}
	}
	stein::stein() {
		cube_elems.push_back(cube());
		cube_elems.push_back(cube());
		cube_elems.push_back(cube());
		cube_elems.push_back(cube());
	}
	
	
	void stein::updateShadow() {
		shadowstone.shadow_elems.clear();
		helpstone.shadow_elems.clear();
		diffToFloor = yLen;
		if (gameover) {
			for (std::vector<cube>::iterator it = cube_elems.begin(); it != cube_elems.end(); ++it) {
				shadowstone.shadow_elems.push_back(cube(it->koordinate.x, 0, it->koordinate.z, color(1)));
			}
			for (std::vector<cube>::iterator it = cube_elems.begin(); it != cube_elems.end(); ++it) {
				helpstone.shadow_elems.push_back(cube(it->koordinate.x, 0, it->koordinate.z, color(1)));
			}
		}
		
		else{
			for (std::vector<cube>::iterator it = cube_elems.begin(); it != cube_elems.end(); ++it) {
				shadowstone.shadow_elems.push_back(cube(it->koordinate.x, 1, it->koordinate.z, color(1)));
				helpstone.shadow_elems.push_back(*it);
			}


			

			int stone_y = 0;
			int f = 0;
			for (std::vector<cube>::iterator it = shadowstone.shadow_elems.begin(); it != shadowstone.shadow_elems.end(); ++it) {
				int x = it->koordinate.x;
				int z = it->koordinate.z;
				int y = cube_elems.at(stone_y).koordinate.y;
				
				for (int i = y - 1; i > 0; i--) {
					if (schacht_array[x-1][i-1][z-1] != transparent) {
						it->koordinate.y = i + 1;
						break;
					}
					else f++;
				}
				stone_y++;
			}
			
			dropShadow();
			
		}
	}
	//
	void stein::bewegen(char achse, int richtung) {
		switch (achse) {
		case('x') :
			for (std::vector<cube>::iterator it = cube_elems.begin(); it != cube_elems.end(); ++it) {
				it->koordinate.x += richtung;
			}
				  if (inside_arena() == false || cube_free() == false) {
					  for (std::vector<cube>::iterator it = cube_elems.begin(); it != cube_elems.end(); ++it) {
						  it->koordinate.x -= richtung;
						  SoundEngine->play2D("irrKlang-1.5.0/media/boing.wav", false, false, false);
					  }
				  }
				  break;

		case ('z') :
			for (std::vector<cube>::iterator it = cube_elems.begin(); it != cube_elems.end(); ++it) {
				it->koordinate.z += richtung;
			}
				   if (inside_arena() == false || cube_free() == false) {
					   for (std::vector<cube>::iterator it = cube_elems.begin(); it != cube_elems.end(); ++it) {
						   it->koordinate.z -= richtung;
						   SoundEngine->play2D("irrKlang-1.5.0/media/boing.wav", false, false, false);
					   }
				   }
				   break;
		}
		updateShadow();
	}


	bool stein::pruefe_ob_unten() {

	}
	//Prüft ob sich ein Stein innerhalb der erlaubten Spielfläche/Arena befindet
	bool stein::inside_arena() {
		for (std::vector<cube>::iterator it = cube_elems.begin(); it != cube_elems.end(); ++it) {
			int x = it->koordinate.x;
			int y = it->koordinate.y;
			int z = it->koordinate.z;
			if (!(x >= 1 && y >= 1 && z >= 1 && x <= xLen && y <= yLen && z <= zLen)) {
				return false;
			}
		}
		return true;
	}

	
	//Prüft ob ein Quadrant im Schacht/Arena frei ist. 
	//Prüft NICHT ob der Quadrant innerhalb der Arena ist!
	bool stein::cube_free() {
		for (std::vector<cube>::iterator it = cube_elems.begin(); it != cube_elems.end(); ++it) {
			int x = it->koordinate.x;
			int y = it->koordinate.y;
			int z = it->koordinate.z;
			if (schacht_array[x - 1][y - 1][z - 1] != transparent) {
				return false;
			}
		}
		return true;
	}
	
	//Kopiert Stein/ Farbe des Steins in die Arenakoordinaten hinein
	void stein::kopiere_in_schacht() {			// Methode 
		for (std::vector<cube>::iterator it = cube_elems.begin(); it != cube_elems.end(); ++it) {
			int x = it->koordinate.x;
			int y = it->koordinate.y;
			int z = it->koordinate.z;
			schacht_array[x - 1][y - 1][z - 1] = it->col;
		}
		
	}
	
	void stein::drehen(char achse, bool uhrzeigersinn, bool reverseT) {
		
		if (gameover) { return; }
		vec3 *diff = new vec3(); //reserviert Speicher für den Differenzvektor (Ankercube - thisCube)
		cube *anker = &this->cube_elems[0]; //
		for (std::vector<cube>::iterator it = cube_elems.begin(); it != cube_elems.end(); ++it) { //Iteriert durch alle Cubes des Steins
			it->transform(achse, uhrzeigersinn, anker, diff);// Verändert den Differenzvektor entsprechend der gewünschten Drehung
			it->koordinate = anker->koordinate + *diff; // Aktualisiert die Koordinaten des cubes 
		}
		delete diff;
		if (inside_arena() == false || cube_free() == false) {
			drehen(achse, !uhrzeigersinn, true);
			SoundEngine->play2D("irrKlang-1.5.0/media/boing.wav", false, false, false);
		}
		else {
			if (!gameover && !reverseT) {
				SoundEngine->play2D("irrKlang-1.5.0/media/drehen.wav", false, false, false);
			}
		}
		updateShadow();
	}
	void stein::difflToFloor() {

		int stone_y = 0;
		for (std::vector<cube>::iterator it = helpstone.shadow_elems.begin(); it != helpstone.shadow_elems.end(); ++it) {
		int f = 0;
			int x = it->koordinate.x;
			int z = it->koordinate.z;
			int y = it->koordinate.y;

			for (int i = y-1 ; i > 0; i--) {
				if (schacht_array[x - 1][i-1][z - 1] == transparent) {
					f++;
				}
				else break;
			}
			if (f < diffToFloor) {
				diffToFloor = f;
			}
				
		}
	
}

			
	void stein::dropShadow() {
		difflToFloor();
		for (std::vector<cube>::iterator it = helpstone.shadow_elems.begin(); it != helpstone.shadow_elems.end(); ++it) {
			int x = it->koordinate.x;
			int y = it->koordinate.y;
			int z = it->koordinate.z;
			it->setKoor(x, y - diffToFloor, z);


		}
	}
};

stein *falling = NULL;




void dropStein(int value) {

	for (std::vector<cube>::iterator it = falling->cube_elems.begin(); it != falling->cube_elems.end(); ++it) {
		it->koordinate.y += value;
	}

	if (falling->inside_arena() == false || falling->cube_free() == false) {
		for (std::vector<cube>::iterator it = falling->cube_elems.begin(); it != falling->cube_elems.end(); ++it) {
			it->koordinate.y -= value;
		}
		if (!gameover)SoundEngine->play2D("irrKlang-1.5.0/media/aufsetzen.wav", false, false, false);

		falling->kopiere_in_schacht();
		srand(time(NULL));		
		int	randStone = rand() % 5 + 1;
		int randColor = rand() % 5 + 1;
		falling = new stein(randStone, color(randColor));
	}
	falling->updateShadow();
}
void turboDrop() {
	for (int i = 0; i < yLen; i++) {
		//	while (falling->inside_arena() == true || falling->cube_free() == true) {
		dropStein(-1);
		//}
		break;
	}
}

