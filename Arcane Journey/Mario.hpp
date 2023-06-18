#pragma once

class Mario
{
private:
	// In Game variables
	//bool crouching; //Checks if player is crouching
	bool dead;		//Checks if player is dead
	bool onGround; //Checks if player is grounded
	bool moving; //Checks if player is moving
	void initIngameVars();

	// Player variables
	sf::Vector2f speed;
	sf::Vector2f position;
	void initPlayerVars();

	// QOL variable
	float enemyBounceSpeed; //speed off which player bounces off enemies
	unsigned char PowerUpState; //powered up or not
	void initQOLVars();

	//Timer Variables
	unsigned char jumpTimer; // longer jump if we press space longer
	unsigned short deathTimer; // time taken for player to die
	unsigned short growthTimer; //time taken for player to grow
	unsigned short invulnerableTimer; // time for which player doesn't take damage
	void initTimerVars();

	// Sprites
	sf::Sprite sprite;
	sf::Texture smallTexture;
	sf::Texture powerUpTexture;
	void initSprites();

	// powerups
	std::vector<Mushroom> mushrooms;

	

public:
	Mario();

	bool get_dead() const;

	float get_vertical_speed() const;
	float get_x() const;

	void die(const bool i_instant_death);
	void draw(sf::RenderWindow& i_window);
	void draw_mushrooms(const unsigned i_view_x, sf::RenderWindow& i_window);
	void reset();
	void set_position(const float i_x, const float i_y);
	void set_vertical_speed(const float i_value);

	void updateEnemyBounce();
	void updateMushrooms(const unsigned i_view_x, MapManager& i_map_manager);

	void updateMovement(MapManager& i_map_manager);
	void updateJump(MapManager& i_map_manager);
	void updateCollisions(MapManager& i_map_manager);
	void updatePowerUps(MapManager& i_map_manager);
	void updateMapCollisions(MapManager& i_map_manager);
	void updateCoinCollection(MapManager& i_map_manager);
	void updateTimers();
	void updateDeath();
	void updateDeathAnimation();
	void updateDeadMushrooms();


	void update(const unsigned i_view_x, MapManager& i_map_manager);

	sf::FloatRect get_hit_box() const;
};