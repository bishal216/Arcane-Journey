#pragma once
#include<SFML/Graphics.hpp>
#include<iostream>
class PlayerEnemyInteraction
{
private:
	//Render Window
	sf::RenderWindow& m_window;
	sf::Font font;
	
	//Use to preserve attack animation
	int idleframe = 0,attackframe=0,walkframe=0;
	//3 players,3 animations , 10 images
	sf::Texture player[3][3][10];
	//10 enemies, 2 animations , 10 images
	sf::Texture enemy[10][2][10];
	//Spawn Position
	sf::Vector2f  start[3] = { sf::Vector2f(400, 750),sf::Vector2f(200, 600),sf::Vector2f(200, 900) };
	//Player Health function
	int TotalHealth=5000;
	int playerHealth;
	//Destination of player[2]
	sf::Vector2f targetlocation;
	float targettime = 0;
	//speed of players and deltaTime
	float speed,frameRate;
	//for mouse controlled player
	float xdiff, ydiff, distance;
	//Whether walk and attack is true or not
	bool walking,attackeff;
	//Draws player and enemy
	sf::RectangleShape playerrect,enemyRect;
	//Enemy Health Functions
	int EnemyHealth = 0, totalEnemyhealth;
	//Checks if enemy is attacking or not
	bool enemyAttack;
	//Enemy Position
	sf::Vector2f EnemyPosition;
	//Animation Preservation
	int EnemyMoveFrame=0;
	//10 types of enemies
	int enemyType=0;
	//Destination of enemy
	sf::Vector2f EnemyDestination;
	//Speed of Enemy
	float EnemySpeed;
	//Score
	 int Score;
	 //For collision
	 //Player
	 sf::RectangleShape hitbox,damagebox; 
	 sf::RectangleShape enemyHitBox, enemyDamageBox;
	 sf::CircleShape identifier;
	 int playerDamage = 0, enemyDamage = 0;

	 int imgChange=5;
public:
	
	//Constructor
  	PlayerEnemyInteraction(sf::RenderWindow& window) : m_window(window) {};
	//Loads the sprites
		void loadonce();
	//Loads the Player and Enemy Interaction Logic
	bool loadCharacters(int choice, int count, bool* forward,float et) ;
	//PlayerControl
	bool walkcheck(int choice);
	bool attackcheck(int choice);
	void playermove(int choice);
	void limitplayer(int choice, bool* forward);
	//EnemyControl
	void loadEnemy(int choice);
	void moveEnemy(int choice);
	void spawnEnemy(int loaction);
	//Collision Control
	void getCollision(int choice);
	void statbar(sf::RectangleShape rec, float health, float totalhealth);
};

