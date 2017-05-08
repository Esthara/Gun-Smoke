#ifndef __ModuleBarrel_H__
#define __ModuleBarrel_H__

#include "Module.h"
#include "Powerup.h"

#define MAX_BARRELS 20

class Barrel;

class ModuleBarrel : public Module
{
public:

	ModuleBarrel();
	~ModuleBarrel();

	bool Start();

	update_status Update();

	bool CleanUp();
	void OnCollision(Collider* c1, Collider* c2);
	SDL_Texture* sprites;
	void AddBarrel(int x, int y, POWERUP_TYPE type);

private:

	Barrel* barrels[MAX_BARRELS];

};

#endif // __ModuleEnemies_H__