#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cstdlib> 
#include <ctime>
#include <cstring>
//cstdlib and ctime both for rand() application
using namespace sf;
using namespace std;

// Initializing Dimensions.
// resolutionX and resolutionY determine the rendering resolution.
// Don't edit unless required. Use functions on lines 43, 44, 45 for resizing the game window.
const int resolutionX = 960;
const int resolutionY = 960;
const int boxPixelsX = 32;
const int boxPixelsY = 32;
const int gameRows = resolutionX / boxPixelsX; // Total rows on grid
const int gameColumns = resolutionY / boxPixelsY; // Total columns on grid

// Initializing GameGrid.
int gameGrid[gameRows][gameColumns] = {};

// The following exist purely for readability.
const int x = 0;
const int y = 1;
const int exists = 2;

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// Write your functions declarations here. Some have been written for you. //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

void drawCentip(sf::RenderWindow& window, float centip[12][3], sf::Sprite& centipSprite); //to draw centip

void drawPlayer(sf::RenderWindow& window, float player[], sf::Sprite& playerSprite); //to draw player

void moveBullet(float bullet[], sf::Clock& bulletClock); //movement of bullet

void drawBullet(sf::RenderWindow& window, float bullet[], sf::Sprite& bulletSprite); // drawing of bullet

void moveCentip(float centip[12][3], float centSpeed); //centipede ki movement

void movePlayerupdown(float player[], float moveSpeed);//player ki longitudinal movement

void movePlayerleftright(float player[], float moveSpeed);//player ki lateral movement

void drawMushroom(sf::RenderWindow& window,int maxmush, float mushroom[][3], sf::Sprite& mushroomSprite);//mushroom ka wujood

void shootBullet(float bullet[], bool& bulletExists, float player[]); //for shooting

void bulletHead(float bullet[], bool& bulletExists, float centip[][3], float gamerange, float xrange, float yrange, int& score );
			//bullet hit head
void bulletBody(float bullet[], bool& bulletExists, float centip[][3], float gamerange, float xrange, float yrange, int& score);
			//bullet hit any other segment of centipede
void bulletMushroom(float bullet[], bool& bulletExists, float mushroom[][3], int maxmush, float gamerange, float xrange, float yrange, int& score);
			//bullet hit mushroom
void centipMushroom(float centip[][3], float mushroom[][3], int maxmush, float gamerange, float xrange, float yrange );
			//centipede hits mushroom
void deathPlayer(float player[], float centip[][3], float gamerange, float xrange, float yrange ); //centipede eats player


int main()
{
	srand(time(0)); //for the application of random function

	// Declaring RenderWindow.
	sf::RenderWindow window(sf::VideoMode(resolutionX, resolutionY), "Centipede", sf::Style::Close | sf::Style::Titlebar);

	// Used to resize your window if it's too big or too small. Use according to your needs.
	window.setSize(sf::Vector2u(640, 640)); // Recommended for 1366x768 (768p) displays.
	// window.setSize(sf::Vector2u(1280, 1280)); // Recommended for 2560x1440 (1440p) displays.
	// window.setSize(sf::Vector2u(1920, 1920)); // Recommended for 3840x2160 (4k) displays.
	
	// Used to position your window on every launch. Use according to your needs.
	window.setPosition(sf::Vector2i(100, 0));

	// Initializing Background Music.
	sf::Music bgMusic;
	bgMusic.openFromFile("Music/Iron_Maiden_The_Trooper.ogg");	//game theme which i changed to IRON MAIDEN's song
	bgMusic.play();
	bgMusic.setVolume(50);
	
	//initialize shooting sound
	sf::Music shootMusic;
	shootMusic.openFromFile("Music/Ak-47.ogg");	//shooting sound
	shootMusic.setVolume(100);			//i added AK47 sound
	
	// Initializing Background.
	sf::Texture backgroundTexture;
	sf::Sprite backgroundSprite;
	backgroundTexture.loadFromFile("Textures/pngtree-game-volcano-advertising-background-picture-image_979719.png");
	//backgorund screen which i changed for myself
	backgroundSprite.setTexture(backgroundTexture);
	backgroundSprite.setColor(sf::Color(255, 255, 255, 255 * 0.20)); // Reduces Opacity to 25%
	
	//text to display score
	int score=0;
 	sf::Font font;
 //	font.loadFromFile("font-file/Freedom-10eM.ttf");
  //	font.loadFromFile("font-file/BodoniFLF-Bold.ttf");
  	font.loadFromFile("font-file/russian-dollmaker-font/RussianDollmaker-AzK2.ttf");
	sf::Text text;
        text.setFont(font);
        text.setCharacterSize(100);
    	text.setFillColor(sf::Color::White);
    	text.setStyle(sf::Text::Bold);
   	text.setPosition(320, 320);
    
   // window.draw(text);
    
	// Initializing Player and Player Sprites.
	float player[3] = {};
	player[x] = (gameColumns / 2) * boxPixelsX;
	player[y] = 800 ;//(gameColumns * 3 / 4) * boxPixelsY;
	player[exists]=true;
	sf::Texture playerTexture;
	sf::Sprite playerSprite;
	playerTexture.loadFromFile("Textures/player.png");
	playerSprite.setTexture(playerTexture);
	playerSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));

	// Initializing Bullet and Bullet Sprites.
	float bullet[3] = {};
	bullet[x] = player[x];
	bullet[y] = player[y] - boxPixelsY;
	bullet[exists] = true;
	sf::Clock bulletClock;
	sf::Texture bulletTexture;
	sf::Sprite bulletSprite;
	bulletTexture.loadFromFile("Textures/bullet.png");
	bulletSprite.setTexture(bulletTexture);
	bulletSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));
	
	//initialize mushrooms
	int maxmush= rand()%10 +20;   		//max number of mushrooms between 20 to 30
	float mushroom[maxmush][3];
	sf::Texture mushroomTexture;
	sf::Sprite mushroomSprite;
	mushroomTexture.loadFromFile("Textures/mushroom.png");
	mushroomSprite.setTexture(mushroomTexture);
	mushroomSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));	
		for(int i=0; i<maxmush; i++){	   		//spawns mushrooms at random locations but 
			mushroom[i][x] = rand()%928;		//out of the player area and within the screen
			mushroom[i][y] = rand()%800-32;	
			mushroom[i][exists] = true;
		while (mushroom[i][y] < 32) {			// and not in the first row
        		mushroom[i][y] = rand() % 800 - 32;
   				 }				
					}			
		
	//initialize half mushrooms
	float halfmush[3];				//mushrooms but with only half the image so as to show half mushies
	sf::Texture halfmushTexture;
	sf::Sprite halfmushSprite;
	halfmushTexture.loadFromFile("Textures/mushroom.png");
	halfmushSprite.setTexture(halfmushTexture);
	halfmushSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY*0.5));	//0.5 gives half the image
	
	//Initialize Centipede
	float centip[12][3]={};			//centipede has 12 segments including one head
	int centispawnvar=rand() % (gameColumns - 12);		//centipede spawns anywhere in the first row
	for(int i=0; i<12; i++){				//and within the screen so it can fit
	centip[i][x]= (centispawnvar+i)*boxPixelsX;
	centip[i][y]=0;
	centip[i][exists]= true;
	}
	sf::Clock centipClock;
	sf::Texture centipTexture;
	sf::Sprite centipSprite;
	sf::Texture headTexture;
	headTexture.loadFromFile("Textures/c_head_left_walk.png");
	centipTexture.loadFromFile("Textures/c_body_left_walk.png");
	centipSprite.setTexture(centipTexture);
	centipSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));
	
	
	
	float moveSpeed = 0.5f; //movement speed of player	
	float centSpeed =0.05f;//centispede
	bool bulletExists = false;
	
	//for collisions
	float gamerange = 16.0f; // 16 is half a pixel and is the radius so diameter is one whole pixel
	float xrange= 0.5f * boxPixelsX;	
	float yrange= 0.5f * boxPixelsY;	
	while(window.isOpen()) {

		///////////////////////////////////////////////////////////////
		//                                                           //
		// Call Your Functions Here. Some have been written for you. //
		// Be vary of the order you call them, SFML draws in order.  //
		//                                                           //
		///////////////////////////////////////////////////////////////

		window.draw(backgroundSprite);
		drawPlayer(window, player, playerSprite);	//player ka wujood(existence)
		drawCentip(window, centip, centipSprite);	//centipede ka wujood
		drawMushroom(window, maxmush, mushroom, mushroomSprite); //mushrooms ka wujood
		
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
    			shootBullet(bullet, bulletExists, player);	//shooting function called
    			shootMusic.play();		//shot soundeffect played everytime space pressed


		}
		
		
		if (bulletExists){		//bullet movement and collission functions with centipede
			moveBullet(bullet, bulletClock);
			bulletHead(bullet, bulletExists, centip, gamerange, xrange, yrange, score);
			bulletBody(bullet, bulletExists, centip, gamerange, xrange, yrange, score);
			drawBullet(window, bullet, bulletSprite);
		}
		
		
		for(int i=0; i<12; i++){
		  if(centip[i][y]<=928-32){		//centipede doesnt move if on the last row
		moveCentip(centip, centSpeed);}
		}
		
		
		bulletMushroom(bullet, bulletExists, mushroom, maxmush, gamerange, xrange, yrange, score); 
		//bullet collission with mushroom function
		centipMushroom(centip, mushroom, maxmush, gamerange, xrange, yrange);	//centipede and mushroom collision
		deathPlayer(player, centip, gamerange, xrange, yrange);		//player eaten by centipede function
		
		
		
		sf::Event e;
		while (window.pollEvent(e)) {
			if (e.type == sf::Event::Closed) {
				window.close();
				return 0;
			}
		}
		
    		movePlayerupdown(player, moveSpeed);		//player mvmnt funcs
    		movePlayerleftright(player, moveSpeed);
    		
    		std::string scoreString = "GAME OVER \n Score: " + std::to_string(score);
       		text.setString(scoreString);	//score has to be updated
       		
        if((player[exists]==false) || (centip[11][exists]==false) )	//game over if centipede or player dies
			    			window.draw(text);
		window.display();
		window.clear();
		
	}
	//while window open ends here
}
//int main ends here

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// Write your functions definitions here. Some have been written for you. //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

void drawPlayer(sf::RenderWindow& window, float player[], sf::Sprite& playerSprite) {
	playerSprite.setPosition(player[x], player[y]);		//player has position given in original code
	window.draw(playerSprite);
}


void moveBullet(float bullet[], sf::Clock& bulletClock) {
	if (bulletClock.getElapsedTime().asMilliseconds() < 5)		//5 basically determines the speed
		return;							//increasing this value decreases bullet speed

	bulletClock.restart();
	bullet[y] -= 10;		
	if (bullet[y] < -32)				//bullet destroyed at the top of the screen
		bullet[exists] = false;
}


void drawBullet(sf::RenderWindow& window, float bullet[], sf::Sprite& bulletSprite) {
	bulletSprite.setPosition(bullet[x], bullet[y]);		//bullet is shot from player postion
	window.draw(bulletSprite);
}


void shootBullet(float bullet[], bool& bulletExists, float player[]) {
	
    if (!bulletExists || bullet[y]<0) {				//shoots if bulletexists is false which we did just before window open 
        bullet[x] = player[x];					
        bullet[y] = player[y] - boxPixelsY;
        bulletExists = true;
        
    }
}


void drawCentip(sf::RenderWindow& window, float centip[12][3], sf::Sprite& centipSprite) {
	sf::Texture centipTexture;
	sf::Texture headTexture;
	headTexture.loadFromFile("Textures/c_head_left_walk.png");
	centipTexture.loadFromFile("Textures/c_body_left_walk.png");
   		for(int i=0;i<12;i++){
    			if(centip[i][exists]==true){		//cannot draw if existence is false
      				if (i < 11) {
            				centipSprite.setTexture(centipTexture);			//normal centipede body 
        					}
        			else {
            				centipSprite.setTexture(headTexture);			//centipede head
        				}   	
	centipSprite.setPosition(centip[i][x], centip[i][y]);
	window.draw(centipSprite);
				}
		}
}


void moveCentip(float centip[12][3], float centSpeed){
	for(int i=0; i<12; i++){			//all 12 segments must move
		if(centip[i][x]>=29*32 || centip[i][x] <= 0)		//centipede descends if hits boundary
		centip[i][y] += 32;
		if(int(centip[i][y])%64==0)				//rows of even numbers (e.g. 0,2,8,26...)
		centip[i][x] += centSpeed;
		else 					//centipede moves in opposite direction when rows of odd numbers
		centip[i][x] -= centSpeed;
	}
}


void movePlayerupdown(float player[], float moveSpeed) { 	//player vertical mvmnt
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {		//Press W to move up
        		if(player[1] -1>=800){			//player cannot cross a certain boundary in the y axis
        		player[1] -= moveSpeed;// Move up
        		}
    }								//player cannot leave the screen either
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {		//Press S to move down
    			if(player[1] +1<=896+32){
        		player[1] += moveSpeed;// Move down
        		}
    }
}


void movePlayerleftright(float player[], float moveSpeed) {		//player horizontal mvmnt
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {   	 //Press A to move left
        		if(player[0] -1>=0){
        		player[0] -= moveSpeed;// Move left
        		}						//player cannot go right or left and leave the screen
    		}
    		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {	//Press D to move right
    			if(player[0] +1<=29*32){
        		player[0] += moveSpeed; // Move right
    			}	
    		}       
}


void drawMushroom(sf::RenderWindow& window,int maxmush, float mushroom[][3], sf::Sprite& mushroomSprite) {
	for(int i=0; i<maxmush; i++){			//more than max mushies cant be created
	 if(mushroom[i][exists]==true){			//cannot be created if its existence is false
	mushroomSprite.setPosition(mushroom[i][x], mushroom[i][y]);
	window.draw(mushroomSprite);}
	}
}


void bulletHead(float bullet[], bool& bulletExists, float centip[][3], float gamerange, float xrange, float yrange, int& score ) {
    	
    //centip[11] is the head wala segment
    if ((bullet[x] + xrange)>= (centip[11][x] + xrange) - gamerange &&
        (bullet[x] + xrange) <=(centip[11][x] + xrange) + gamerange &&
        (bullet[y] + yrange)>= (centip[11][y] + yrange) - gamerange &&
        (bullet[y] + yrange)<= (centip[11][y] + yrange) + gamerange) {
      
        bulletExists = false;  		//  if bullet is not destroyed, it will pierce through
    
        	for (int i=0; i<12; i++){
        centip[i][x] =-1000;			//the whole centipede is not only destroyed but also
        centip[i][y] =-1000 ;			//moved wayyy out of the screen
        centip[i][exists]=false;		//otherwise it will stop the bullet from passing in that area
        	}
        	score=score+20;				//game should end
    	}
}


void bulletBody(float bullet[], bool& bulletExists, float centip[][3], float gamerange, float xrange, float yrange, int& score) {
     
 // all segments are included except the head  
	for (int i=0; i<11; i++){
	  if 	((bullet[x]+xrange) >= (centip[i][x] + xrange) - gamerange &&
         	(bullet[x]+xrange) <= (centip[i][x] + xrange) + gamerange &&
         	(bullet[y]+yrange) >= (centip[i][y] + yrange) - gamerange &&
         	(bullet[y]+yrange) <= (centip[i][y] + yrange)  + gamerange){
         	
         	bulletExists=false;		//  if bullet is not destroyed, it will pierce through
         	
         	centip[i][exists] = false;		//specific segment is destroyed
         	centip[i][x]=-1000;			//and moved out of the screen to 
         	centip[i][y]=-1000;			//let the bullet pass
         	score=score+10;
         	}	
	}
	
}


void bulletMushroom(float bullet[], bool& bulletExists, float mushroom[][3], int maxmush, float gamerange, float xrange, float yrange, int& score) {
    

    // all mushrooms are checked
    for (int i = 0; i < maxmush; ++i) {
        if ((bullet[x]+xrange) >= (mushroom[i][x] + xrange) - gamerange &&
            (bullet[x]+xrange) <= (mushroom[i][x] + xrange) + gamerange &&
            (bullet[y]+yrange) >= (mushroom[i][y] + yrange) - gamerange &&
            (bullet[y]+yrange) <= (mushroom[i][y] + yrange) + gamerange) {
            
            bulletExists=false;			//removing this condition makes the bullet pierce through
            
            mushroom[i][exists]=false;		//that mushroom is not only destroyed
            mushroom[i][x]=-1000;		//but also kicked out of the screen 
            mushroom[i][y]=-1000;		//so as not to stop the bullet
            score=score+1;
        }
    }
    
}


void centipMushroom(float centip[][3], float mushroom[][3], int maxmush, float gamerange, float xrange, float yrange ) {
   
    for (int i = 0; i < maxmush; ++i) {			//each mushroom is checked
        if (mushroom[i][exists] == true) {		//mushroom must exist
            for (int j = 0; j < 12; ++j) {		//all centipede segments are checked
                if (centip[j][exists] == true) {
                    if ((centip[j][x]+xrange) >= (mushroom[i][x] + xrange) - gamerange &&
                        (centip[j][x]+xrange) <= (mushroom[i][x] + xrange) + gamerange &&
                        (centip[j][y]+yrange) >= (mushroom[i][y] + yrange) - gamerange &&
                        (centip[j][y]+yrange) <= (mushroom[i][y] + yrange) + gamerange) {
                        
                        centip[j][y] += boxPixelsY; 		//centipede changes direction as if collided with wall
                    }
                }
            }
        }
    }
}


void deathPlayer(float player[], float centip[][3], float gamerange, float xrange, float yrange ) {
    
    
    for (int i = 0; i < 12; ++i) {		//all segments are checked        
       	if (player[0] + xrange >= (centip[i][0] + xrange) - gamerange &&
            	player[0] + xrange <= (centip[i][0] + xrange) + gamerange &&
            	player[1] + yrange >= (centip[i][1] + yrange) - gamerange &&
            	player[1] + yrange <= (centip[i][1] + yrange) + gamerange) {
            		player[exists]=false;
            		player[0] = -1000; 			//player is kicked wayyy out of the screen
            		player[1] = -1000; 			//this is to show elimination
            					//that centipede ate you up, game should end
        }
    }
}
