#include "stdafx.h"

#include "Animation.hpp"
#include "Global.hpp"
#include "MapManager.hpp"
#include "Mushroom.hpp"
#include "Mario.hpp"

void Mario::initIngameVars()
{
	//this->crouching = 0;
	this->dead = 0;
	this->onGround = 0;
	this->moving = 0;
}
void Mario::initPlayerVars()
{
	this->speed = sf::Vector2f(0.f, 0.f);
	this->position = sf::Vector2f(0.0f, 0.0f);
}
void Mario::initQOLVars()
{
	this->enemyBounceSpeed = 0;
	this->PowerUpState = 0;
}
void Mario::initTimerVars()
{
	this->jumpTimer = 0;
	this->deathTimer = MARIO_DEATH_DURATION;
	this->invulnerableTimer = 0;
	this->growthTimer = 0;
}
void Mario::initSprites()
{
	smallTexture.loadFromFile("../assets/textures/player.png");
	sprite.setTexture(smallTexture);

	powerUpTexture.loadFromFile("../assets/textures/playerTall.png");
}
Mario::Mario(){
	initIngameVars();
	initPlayerVars();
	initQOLVars();
	initTimerVars();
	initSprites();
}

bool Mario::get_dead() const
{
	return dead;
}

float Mario::get_vertical_speed() const
{
	return this->speed.y;
}

float Mario::get_x() const
{
	return this->position.x;
}

void Mario::die(const bool i_instant_death)
{
	//Player instantly dies and it doesn't matter if he's big or small.
	if (i_instant_death){dead = 1;}
	//Player dies, unless he's big.
	else if (0 == growthTimer && 0 == invulnerableTimer)
	{
		//small Player dies
		if (0 == PowerUpState){dead = 1;}
		//powered Up player loses power
		else{
			PowerUpState = 0;
			invulnerableTimer = MARIO_INVINCIBILITY_DURATION;
			//if (0 == crouching){this->position.y += CELL_SIZE;}
			//else{crouching = 0;}
		}
	}
}

void Mario::draw(sf::RenderWindow& i_window)
{
	//When player is invincible, his sprite will blink.
	if (invulnerableTimer / MARIO_BLINKING % 2 == 0)
	{
		bool draw_sprite = 1;
		bool draw_big = 0 == growthTimer / MARIO_BLINKING % 2;
		sprite.setPosition(round(this->position.x), round(this->position.y));

		if (!dead)
		{
			if ( PowerUpState > 0 /*and crouching == 0*/)
			{
				this->sprite.setTexture(powerUpTexture);
			}
			else{
				this->sprite.setTexture(smallTexture);
			}
		}
		i_window.draw(sprite);
	}
}

void Mario::draw_mushrooms(const unsigned i_view_x, sf::RenderWindow& i_window)
{
	for (Mushroom& mushroom : mushrooms)
	{
		mushroom.draw(i_view_x, i_window);
	}
}

void Mario::reset()
{
	initIngameVars();
	initPlayerVars();
	initQOLVars();
	initTimerVars();
	initSprites();
	
	mushrooms.clear();
}

void Mario::set_position(const float i_x, const float i_y)
{
	this->position.x = i_x;
	this->position.y = i_y;
}

void Mario::set_vertical_speed(const float i_value)
{
	enemyBounceSpeed = i_value;
}

void Mario::updateEnemyBounce()
{
	if (enemyBounceSpeed != 0)
	{
		this->speed.y = enemyBounceSpeed;
		enemyBounceSpeed = 0;
	}
}

void Mario::updateMushrooms(const unsigned i_view_x, MapManager& i_map_manager)
{
	for (Mushroom& mushroom : mushrooms)
	{
		mushroom.update(i_view_x, i_map_manager);
	}
}

void Mario::updateMovement(MapManager& i_map_manager)
{
	this->moving = 0;

	if (1/*0 == this->crouching*/)
	{
		if (0 == sf::Keyboard::isKeyPressed(sf::Keyboard::Right) &&
			1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			moving = 1;

			this->speed.x = std::max(this->speed.x - MARIO_ACCELERATION, -MARIO_WALK_SPEED);
		}

		if (0 == sf::Keyboard::isKeyPressed(sf::Keyboard::Left) &&
			1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			moving = 1;

			this->speed.x = std::min(MARIO_ACCELERATION + this->speed.x, MARIO_WALK_SPEED);
		}
	}
	if (moving == 0)
	{
		if (0 < this->speed.x)
		{
			this->speed.x = std::max<float>(0, this->speed.x - MARIO_ACCELERATION);
		}
		else if (0 > this->speed.x)
		{
			this->speed.x = std::min<float>(0, MARIO_ACCELERATION + this->speed.x);
		}
	}
}

void Mario::updateJump(MapManager& i_map_manager)
{

}

void Mario::update(const unsigned i_view_x, MapManager& i_map_manager)
{

	this->updateEnemyBounce();

	this->updateMushrooms(i_view_x, i_map_manager);

	if (dead == 0)
	{
		updateMovement(i_map_manager);
		updateJump(i_map_manager);
		//Each number is a binary representation of the collisions in a single row. And we're storing that vector in this vector.
		std::vector<unsigned char> collision;

		//map_collision function can also return the coordinates of cells intersecting the hitbox. We need that too.
		std::vector<sf::Vector2i> cells;

		sf::FloatRect hit_box = get_hit_box();

		onGround = 0;

		

		//if (0 < PowerUpState)
		//{
		//	/*if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::C) || 1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		//	{
		//		if (0 == crouching)
		//		{
		//			crouching = 1;

		//			this->position.y += CELL_SIZE;
		//		}
		//	}*/
		//	/*else */if (1 /*== crouching*/)
		//	{
		//		hit_box.height += CELL_SIZE;
		//		hit_box.top -= CELL_SIZE;

		//		//Making sure we can stand up without hitting anything.
		//		collision = i_map_manager.map_collision({Cell::ActivatedQuestionBlock, Cell::Brick, Cell::Pipe, Cell::QuestionBlock, Cell::Wall}, hit_box);

		//		if (1 == std::all_of(collision.begin(), collision.end(), [](const unsigned char i_value)
		//		{
		//			return 0 == i_value;
		//		}))
		//		{
		//			crouching = 0;

		//			this->position.y -= CELL_SIZE;
		//		}
		//		else
		//		{
		//			collision = i_map_manager.map_collision({Cell::ActivatedQuestionBlock, Cell::Pipe, Cell::QuestionBlock, Cell::Wall}, hit_box);

		//			//But if it happens to be bricks, we'll destroy them.
		//			if (1 == std::all_of(collision.begin(), collision.end(), [](const unsigned char i_value)
		//			{
		//				return 0 == i_value;
		//			}))
		//			{
		//				crouching = 0;

		//				this->position.y -= CELL_SIZE;

		//				i_map_manager.map_collision({Cell::Brick}, cells, hit_box);

		//				for (const sf::Vector2i& cell : cells)
		//				{
		//					i_map_manager.set_map_cell(cell.x, cell.y, Cell::Empty);
		//					i_map_manager.add_brick_particles(CELL_SIZE * cell.x, CELL_SIZE * cell.y);
		//				}
		//			}
		//		}
		//	}
		//}

		hit_box = get_hit_box();
		hit_box.left += this->speed.x;
		
		collision = i_map_manager.map_collision({Cell::ActivatedQuestionBlock, Cell::Brick, Cell::Pipe, Cell::QuestionBlock, Cell::Wall}, hit_box);

		if (0 == std::all_of(collision.begin(), collision.end(), [](const unsigned char i_value)
		{
			return 0 == i_value;
		}))
		{
			moving = 0;

			if (0 < this->speed.x)
			{
				this->position.x = CELL_SIZE * (ceil((this->speed.x + this->position.x) / CELL_SIZE) - 1);
			}
			else if (0 > this->speed.x)
			{
				this->position.x = CELL_SIZE * (1 + floor((this->speed.x + this->position.x) / CELL_SIZE));
			}

			this->speed.x = 0;
		}
		else
		{
			this->position.x += this->speed.x;
		}

		hit_box = get_hit_box();
		hit_box.top++;

		collision = i_map_manager.map_collision({Cell::ActivatedQuestionBlock, Cell::Brick, Cell::Pipe, Cell::QuestionBlock, Cell::Wall}, hit_box);
		
		if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || 1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
		{
			if (0 == this->speed.y && 0 == std::all_of(collision.begin(), collision.end(), [](const unsigned char i_value)
			{
				return 0 == i_value;
			}))
			{
				this->speed.y = MARIO_JUMP_SPEED;

				jumpTimer = MARIO_JUMP_TIMER;
			}
			else if (0 < jumpTimer) //The longer we press the jump button, the higher Mario jumps.
			{
				this->speed.y = MARIO_JUMP_SPEED;

				jumpTimer--;
			}
			else
			{
				this->speed.y = std::min(GRAVITY + this->speed.y, MAX_VERTICAL_SPEED);
			}
		}
		else
		{
			this->speed.y = std::min(GRAVITY + this->speed.y, MAX_VERTICAL_SPEED);

			jumpTimer = 0;
		}

		hit_box = get_hit_box();
		hit_box.top += this->speed.y;

		collision = i_map_manager.map_collision({Cell::ActivatedQuestionBlock, Cell::Brick, Cell::Pipe, Cell::QuestionBlock, Cell::Wall}, hit_box);
		
		if (0 == std::all_of(collision.begin(), collision.end(), [](const unsigned char i_value)
		{
			return 0 == i_value;
		}))
		{
			if (0 > this->speed.y)
			{
				//Destroying bricks!!!!
				if (/*0 == crouching && */0 < PowerUpState)
				{
					i_map_manager.map_collision({Cell::Brick}, cells, hit_box);

					for (const sf::Vector2i& cell : cells)
					{
						i_map_manager.set_map_cell(cell.x, cell.y, Cell::Empty);
						i_map_manager.add_brick_particles(CELL_SIZE * cell.x, CELL_SIZE * cell.y);
					}
				}

				i_map_manager.map_collision({Cell::QuestionBlock}, cells, hit_box);

				//Activating question blocks!!!!
				for (const sf::Vector2i& cell : cells)
				{
					i_map_manager.set_map_cell(cell.x, cell.y, Cell::ActivatedQuestionBlock);

					//It can be either a mushroom or a coin, depending on the color of the pixel in the sketch.
					if (sf::Color(255, 73, 85) == i_map_manager.get_map_sketch_pixel(cell.x, cell.y))
					{
						mushrooms.push_back(Mushroom(CELL_SIZE * cell.x, CELL_SIZE * cell.y));
					}
					else
					{
						i_map_manager.add_question_block_coin(CELL_SIZE * cell.x, CELL_SIZE * cell.y);
					}
				}

				this->position.y = CELL_SIZE * (1 + floor((this->speed.y + this->position.y) / CELL_SIZE));
			}
			else if (0 < this->speed.y)
			{
				this->position.y = CELL_SIZE * (ceil((this->speed.y + this->position.y) / CELL_SIZE) - 1);
			}

			jumpTimer = 0;

			this->speed.y = 0;
		}
		else
		{
			this->position.y += this->speed.y;
		}

		

		hit_box = get_hit_box();
		hit_box.top++;

		collision = i_map_manager.map_collision({Cell::ActivatedQuestionBlock, Cell::Brick, Cell::Pipe, Cell::QuestionBlock, Cell::Wall}, hit_box);

		if (0 == std::all_of(collision.begin(), collision.end(), [](const unsigned char i_value)
		{
			return 0 == i_value;
		}))
		{
			onGround = 1;
		}

		for (Mushroom& mushroom : mushrooms)
		{
			//Mushroom eating and becoming BIG, STRONG, MASCULINE!!!!
			if (1 == get_hit_box().intersects(mushroom.get_hit_box()))
			{
				mushroom.set_dead(1);

				if (0 == PowerUpState)
				{
					PowerUpState = 1;

					growthTimer = MARIO_GROWTH_DURATION;

					this->position.y -= CELL_SIZE;
				}
			}
		}

		if (0 < invulnerableTimer)
		{
			invulnerableTimer--;
		}

		hit_box = get_hit_box();

		i_map_manager.map_collision({Cell::Coin}, cells, hit_box);

		//Collecting coins.
		for (const sf::Vector2i& cell : cells)
		{
			i_map_manager.set_map_cell(cell.x, cell.y, Cell::Empty);
		}

		if (0 < growthTimer)
		{
			growthTimer--;
		}

		if (this->position.y >= SCREEN_HEIGHT - get_hit_box().height)
		{
			die(1);
		}

	}
	else
	{
		if (0 == deathTimer)
		{
			this->speed.y = std::min(GRAVITY + this->speed.y, MAX_VERTICAL_SPEED);
			this->position.y += this->speed.y;
		}
		else if (1 == deathTimer)
		{
			this->speed.y = MARIO_JUMP_SPEED;
		}

		deathTimer = std::max(0, deathTimer - 1);
	}

	//Deleting mushrooms from the vector.
	mushrooms.erase(remove_if(mushrooms.begin(), mushrooms.end(), [](const Mushroom& i_mushroom)
	{
		return 1 == i_mushroom.get_dead();
	}), mushrooms.end());
}

sf::FloatRect Mario::get_hit_box() const
{
	//The hitbox will be small if Mario is small or crouching.
	return sf::FloatRect(this->position.x, this->position.y, CELL_SIZE, CELL_SIZE);
	
}
