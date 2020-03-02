#include "PlayerEnemyInteraction.h"

void PlayerEnemyInteraction::loadonce()
{
    font.loadFromFile("Fonts/AD.ttf");
    targetlocation = start[2];

    playerrect.setSize(sf::Vector2f(360, 200));
    playerrect.setOrigin(sf::Vector2f(180, 140));
    enemyRect.setSize(sf::Vector2f(150, 150));
    enemyRect.setOrigin(sf::Vector2f(75, 125));
    
    playerHealth = TotalHealth;
    totalEnemyhealth = 5000;
    EnemyHealth = -1;
    Score = 0;
    playerDamage = 567;
    enemyDamage = 27;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 10; k++)
            {
                player[i][j][k].loadFromFile("Textures/Players/" + std::to_string(i+1) + "/" + std::to_string(j+1) + "/" + std::to_string(k) + ".png");
            }
        }
    }
    
    //EnemyTypes 0-9
    
    for (int i = 0; i < 10; i++)
    {
        //Two cases 1.Move , 2.Attack
        for (int j = 0; j < 2; j++)
        { 
            for(int k=0;k<10;k++)
                enemy[i][j][k].loadFromFile("Textures/Enemies/Enemy/Enemy" + std::to_string(i) +"/"+ std::to_string(j + 1) + "/" + std::to_string(k) + ".png");
        }
    }
    
    //PlayerHitBox And Player Damage Box
    
    hitbox.setSize(sf::Vector2f(50, 100));
    hitbox.setFillColor(sf::Color(0, 0, 0, 255));
    hitbox.setOrigin(25, 100);
    
    damagebox.setFillColor(sf::Color(255, 255, 255, 255));
    identifier.setOrigin(sf::Vector2f(25,25));
    identifier.setRadius(25);

}

bool PlayerEnemyInteraction::loadCharacters(int choice, int count, bool* forward, float deltaTime)
{
    
    if (playerHealth <= 0)
    {
        return false;
    }
    *forward = false;
    //PlayerHitBox
    hitbox.setPosition(start[choice]);
    
    //PlayerAttackbox
    switch (choice)
    {
    case 0:
        damagebox.setSize(sf::Vector2f(100, 100));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            damagebox.setOrigin(75, 100);
        else
            damagebox.setOrigin(25, 100);
        break;
    case 1:
        damagebox.setSize(sf::Vector2f(100, 100));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            damagebox.setOrigin(75, 100);
        else
            damagebox.setOrigin(25, 100);
        break;
    case 2:
        targettime += deltaTime;
        damagebox.setSize(sf::Vector2f(120, 100));
        if (targetlocation.x < start[2].x)
            damagebox.setOrigin(100, 100);
        else
            damagebox.setOrigin(25, 100);
        if (targettime > 0.1)
        {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                targetlocation = (sf::Vector2f)sf::Mouse::getPosition();
            targettime = 0;
        }
        
        break;
    }
    damagebox.setPosition(start[choice]);
    
    //-------------------------------------------------delete these lines
      //  m_window.draw(damagebox);
     //m_window.draw(hitbox);
    //--------------------------------------------------delete lines above
    //PlayerOrigin
    frameRate = deltaTime;

    
    walking = walkcheck(choice);
    if(attackframe==0)
        attackeff = attackcheck(choice);
    if (attackeff == true)
    {
        attackframe++;
        if (attackframe == imgChange*10)
        {
            attackframe = 0;
        }
            
    }
    *forward = false;
    playerrect.setPosition(start[choice]);
    
    playerrect.setScale(1, 1);

    if (walking == false && attackeff == false)
    {
        idleframe++;
        if (idleframe == imgChange*10)
            idleframe = 0;
        walkframe = 0;
        if (choice == 0)
            playerrect.setTexture(&player[0][0][idleframe / imgChange]);
        else if (choice == 1)
            playerrect.setTexture(&player[1][0][idleframe / imgChange]);
        else
            playerrect.setTexture(&player[2][0][idleframe / imgChange]);
    }
    if (walking == true)
    {
        speed = 400 * deltaTime;
        walkframe++;
        if (walkframe == imgChange*10) { walkframe = 0; }
        
        if (choice == 0)
            playerrect.setTexture(&player[0][1][walkframe / imgChange]);
        else if (choice == 1)
            playerrect.setTexture(&player[1][1][walkframe / imgChange]);
        else
            playerrect.setTexture(&player[2][1][walkframe / imgChange]);
        playermove(choice);
        limitplayer(choice, forward);
    }
    if (attackeff == true)
    {
        speed = 100 * deltaTime;
            if (choice == 0)
                playerrect.setTexture(&player[0][2][attackframe / imgChange]);
            else if (choice == 1)
                playerrect.setTexture(&player[1][2][attackframe / imgChange]);
            else
                playerrect.setTexture(&player[2][2][attackframe / imgChange]);
    }
    loadEnemy(choice);
    getCollision(choice);
    statbar(playerrect, playerHealth, TotalHealth);
    m_window.draw(playerrect); 

    if (enemyRect.getPosition().x < 1920 && enemyRect.getPosition().x>0 && enemyRect.getPosition().y < 1080)
        *forward = false;

    return true;
}

bool PlayerEnemyInteraction::walkcheck(int choice)
{
    if (choice == 0)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            return true;
        else
            return false;
    }
    if (choice == 1)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            return true;
        else
            return false;
    }
    if (choice == 2)
    {
        if (targetlocation != start[2])
            return true;
        else
            return false;
    }   
}

bool PlayerEnemyInteraction::attackcheck(int choice)
{
    if (choice == 0)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            return true;
        else
            return false;
    }
    if (choice == 1)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad0))
            return true;
        else
            return false;
    }
    if (choice == 2)
    {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
            return true;
        else
            return false;
    }
}

void PlayerEnemyInteraction::playermove(int choice)
{
    if (choice == 0)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)){start[0].y -= speed;}
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)){start[0].y += speed ;}
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { start[0].x -= speed; playerrect.setScale(-1, 1); damagebox.setOrigin(75, 100);}
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)){start[0].x += speed;}
    }
    if (choice == 1)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))  {start[1].y -= speed; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){start[1].y += speed; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) { start[1].x -= speed; playerrect.setScale(-1, 1); damagebox.setOrigin(75, 100); }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){start[1].x += speed; }
    }
    if (choice == 2)
    {
        xdiff = abs(targetlocation.x - start[2].x);
        ydiff = abs(targetlocation.y - start[2].y);
        distance = std::sqrt(xdiff * xdiff + ydiff * ydiff);
        if (targetlocation.y < start[2].y)
            start[2].y -= speed * ydiff/distance;
        if (targetlocation.y > start[2].y)
            start[2].y += speed * ydiff / distance;
        if (targetlocation.x < start[2].x)
        {
            playerrect.setScale(-1, 1);
            damagebox.setOrigin(100, 100);
            start[2].x -= speed * xdiff / distance;
        }
            
        if (targetlocation.x > start[2].x)
            start[2].x += speed * xdiff / distance;
        if (distance <= 5)
            start[2] = targetlocation;
    }
}

void PlayerEnemyInteraction::limitplayer(int choice,bool *forward)
{
    if (start[choice].y < 470){start[choice].y = 470;}
    if (start[choice].y >= 1080){start[choice].y = 1080;}
    if (start[choice].x <= 0){start[choice].x = 0;}
    if (start[choice].x >= 1520){start[choice].x = 1520;}
    if (start[choice].x >= 1520)
    {    
        if (walking == true)
            *forward = true;
        else
            *forward = false;
    }
    if (targetlocation.y <= 470) { targetlocation.y = 470; }
    if (targetlocation.y >= 1080) { targetlocation.y = 1080; }
    if (targetlocation.x <= 100) { targetlocation.x = 100; }
    if (targetlocation.x >= 1520) { targetlocation.x = 1521; }

}

void PlayerEnemyInteraction::loadEnemy(int choice)
{
    if (EnemyHealth <= 0)
    {
        EnemyHealth--;
        enemyRect.setFillColor(sf::Color(255,255,255,255+EnemyHealth));
    }
    if (EnemyHealth <= -500)
    {
        spawnEnemy(std::rand() % 2);
        enemyType = std::rand() % 10;
    }
    if (EnemyHealth > 0)
    {
        EnemyMoveFrame++;
        if (EnemyMoveFrame == imgChange * 10) { EnemyMoveFrame = 0; }
        enemyRect.setFillColor(sf::Color(255, 255, 255, 255));
        EnemyDestination = sf::Vector2f(start[choice].x + 80, start[choice].y);
        enemyRect.setPosition(EnemyPosition);
        enemyRect.setScale(-1, 1);
        if (EnemyPosition.x < start[choice].x)
            enemyRect.setScale(1, 1);
        if (enemyAttack == false)
            enemyRect.setTexture(&enemy[enemyType][0][EnemyMoveFrame / (imgChange)]);
        else
            enemyRect.setTexture(&enemy[enemyType][1][EnemyMoveFrame / (imgChange)]);
        moveEnemy(choice);
        //Enemy Identifier
        identifier.setPosition(EnemyPosition);
        if (choice == 0)
            identifier.setFillColor(sf::Color(255, 239, 46, 255));
        if (choice == 1)
            identifier.setFillColor(sf::Color(191, 181, 108, 255));
        if (choice == 2)
            identifier.setFillColor(sf::Color(128, 127, 127, 255));
        statbar(enemyRect, EnemyHealth, totalEnemyhealth);
        m_window.draw(identifier);
        m_window.draw(enemyRect);
    }
    
}

void PlayerEnemyInteraction::moveEnemy(int choice)
{
    EnemySpeed = 100 * frameRate;
    xdiff = abs(EnemyPosition.x- EnemyDestination.x);
    ydiff = abs(EnemyPosition.y - EnemyDestination.y);
    distance = std::sqrt(xdiff * xdiff + ydiff * ydiff);
    if (EnemyPosition.y < EnemyDestination.y)
        EnemyPosition.y += EnemySpeed * ydiff / distance;
    if (EnemyPosition.y > EnemyDestination.y)
        EnemyPosition.y -= EnemySpeed * ydiff / distance;
    if (EnemyPosition.x < EnemyDestination.x)
        EnemyPosition.x += EnemySpeed * xdiff / distance;
    if (EnemyPosition.x > EnemyDestination.x)
        EnemyPosition.x -= EnemySpeed * xdiff / distance;
   if (distance <= 1)
       EnemyPosition = EnemyDestination;
   if (EnemyDestination.y == EnemyPosition.y)
       enemyAttack = true;
   else
       enemyAttack = false;
}

void PlayerEnemyInteraction::spawnEnemy(int loaction)
{
    enemyDamage =(int)(enemyDamage* 1.1);
    playerDamage = (int)(playerDamage * 1.1);
    Score += totalEnemyhealth / 100;
    if (loaction == 0)
        EnemyPosition = sf::Vector2f(2500, 1500);
        
    if(loaction==1)
        EnemyPosition = sf::Vector2f(-500, 1500);
    EnemyHealth = totalEnemyhealth;
    totalEnemyhealth += 500;
}

void PlayerEnemyInteraction::getCollision(int choice)
{
    if (std::abs(start[choice].y - EnemyPosition.y)<=5)
    {
        if (std::abs(start[choice].x - EnemyPosition.x) <= 100)
        {
            if (attackeff == true && attackframe == 42)
                EnemyHealth -= playerDamage;
            if (enemyAttack == true && EnemyMoveFrame == 42)
                playerHealth -= enemyDamage;
        }
        
    }       
}

void PlayerEnemyInteraction::statbar(sf::RectangleShape rec, float health, float totalhealth)
{
    if (health <= 0)
        health = 0;
    //Totalhealth
    sf::RectangleShape total_hth(sf::Vector2f(70, 15));
    total_hth.setFillColor(sf::Color(255, 255, 255, 200));
    total_hth.setPosition(rec.getPosition().x-50,rec.getPosition().y-150);
    //HealthString
    sf::Text strHealth(std::to_string((int)health) + "/" + std::to_string((int)totalhealth),font,10);
    strHealth.setFillColor(sf::Color(0,0,0,255));
    strHealth.setPosition(rec.getPosition().x - 45, rec.getPosition().y - 150);

    //CurrentHealth
    sf::RectangleShape current_hth(sf::Vector2f(health / totalhealth * 70, 15));

    if (health / totalhealth> .66)
        current_hth.setFillColor(sf::Color(0, 255, 0, 200));
    else if (health / totalhealth < .33)
        current_hth.setFillColor(sf::Color(255, 0, 0, 200));
    else
        current_hth.setFillColor(sf::Color(255, 255, 0, 200));
    current_hth.setPosition(rec.getPosition().x - 50, rec.getPosition().y - 150);

    m_window.draw(total_hth);
    m_window.draw(current_hth);
    m_window.draw(strHealth);



}
