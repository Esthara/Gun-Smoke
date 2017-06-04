#include "Application.h"
#include "Enemy_MechBoss.h"
#include "ModuleCollision.h"
#include "ModuleEnemies.h"
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include "Moduleplayer.h"
#include "SDL/include/SDL_timer.h"
#include "ModuleParticles.h"

#define PATH_DURATION 500
#define BULLET_INT_MIN 2000
#define BULLET_INT_MAX 2500
#define BOMB_DELAY 12000


Enemy_MechBoss::Enemy_MechBoss(int x, int y) : Enemy(x, y)
{
	walk.PushBack({ 2, 154, 22, 32 });
	walk.PushBack({ 25, 154, 20, 32 });
	walk.PushBack({ 46, 154, 22, 32 });
	walk.PushBack({ 69, 154, 19, 32 });
	walk.speed = 0.1f;
	animation = &walk;

	/*path.PushBack({ -1.0f,-0.75f }, 120);
	path.PushBack({ -1.0f,0.75f }, 100);
	path.PushBack({ -1.0f,1.25f }, 100);*/

	collider = App->collision->AddCollider({ 0, 0, 20, 30 }, COLLIDER_TYPE::COLLIDER_ENEMY, (Module*)App->enemies);
	col = App->collision->AddCollider({ 0, 0, 20, 10 }, COLLIDER_TYPE::COLLIDER_ENEMY_BASE, (Module*)App->enemies);

	original_pos.x = x;
	original_pos.y = y;
	path_dest.y = position.y;
	path_dest.x = position.x;

	timer = SDL_GetTicks()+2000;

	hp = 1;
}

int Enemy_MechBoss::value_between(int min, int max)
{
	return rand() % (max - min + 1) + min;
}


void Enemy_MechBoss::Move()
{
	if (SDL_GetTicks() > timer)
	{
		if (moving == false)
		{
			bool path_valid = false;
			//select new path
			while (!path_valid)
			{
				path_dest.x = value_between(position.x - 25, position.x + 25);
				path_dest.y = value_between(position.y - 40, position.y + 20);

				if ((abs(path_dest.x - App->player->position.x) > 20)
					|| (abs(path_dest.y - App->player->position.y) > 20)) // not near player
				{
					if (path_dest.x > 0
						&& path_dest.x < SCREEN_WIDTH)// not out of screen(x)
					{
						path_valid = true;
					}
				}
			}
			path_from = position;
			moving = true;
			path_start = SDL_GetTicks();
		}
		//shoot next mine
		if (SDL_GetTicks() > next_shot)
		{
			bullet_angle = M_PI / 4 * trunc((M_PI / 8) + atan2(App->player->position.y - position.y, App->player->position.x - position.x) / (M_PI / 4));
			App->particles->AddParticle(App->particles->tnt, position.x, position.y, COLLIDER_NONE, 2, 0, 2 * sin(bullet_angle));
			bombdelay = SDL_GetTicks() + 800;

			next_shot = SDL_GetTicks() + value_between(BULLET_INT_MIN, BULLET_INT_MAX);
		}


		if (SDL_GetTicks() > bombdelay) {
			App->particles->AddParticle(App->particles->explosion, position.x, position.y + 70, COLLIDER_ENEMY_BASE, 0, 0, 0,true);
			bombdelay += 100000;

		}



		//check destination
		uint elapsed = SDL_GetTicks() - path_start;
		if (elapsed > PATH_DURATION)
		{
			elapsed = PATH_DURATION;
			moving = false;
		}

		//move towards path
		position.x = path_from.x + (path_dest.x - path_from.x) * (int)elapsed / PATH_DURATION;
		position.y = path_from.y + (path_dest.y - path_from.y) * (int)elapsed / PATH_DURATION;

		animation = &walk;

		//move collider
		col->SetPos(position.x, position.y + 20);
		if (collider != nullptr)
			collider->SetPos(position.x, position.y);
	}
	else
	{
		animation = &walk;

		col->SetPos(position.x, position.y + 20);
		if (collider != nullptr)
			collider->SetPos(position.x, position.y);

		position.y += 1;
	}

}

Enemy_MechBoss::~Enemy_MechBoss()
{
	if (App->player->destroyed == false && hit == true)
	{
		App->player->score += 300;
		App->enemies->Playsound(0);
	}

	App->particles->AddParticle(App->particles->mechdeath, position.x, position.y, COLLIDER_NONE);
}