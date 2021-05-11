#pragma once

/*	
	-> Gameplay speed = 25 loop ticks/sec
*/

#define WIDTH 1024
#define HEIGHT 500

#define playerTexture "SF01.png"
#define playerHP 10
#define playerBulletSpeed 30

#define enemyTexture "resources/sprites/enemyShip.png"
#define enemyHP 5
#define enemySpeed -5
#define enemyBulletSpeed -15

#define bulletHP 1

#define normalBulletTexture "resources/sprites/blueLaser.png"
#define waveBulletTexture "resources/sprites/waveBullet.png"
#define enemyBulletTexture "resources/sprites/redLaser.png"


#define powerUpSPD 6
#define bonusHPTexture "resources/sprites/bonusHP.png"
#define enchanceAttackTexture "resources/sprites/enchanceATK.png"

#define explosionTexture "resources/sprites/explosion.png"

#define titleScreenTexture "resources/sprites/titleScreen.png"
#define endScreenTexture "resources/sprites/endScreen.png"
#define winScreenTexture "winScreen.png"
#define icon "resources/sprites/icon.png"
#define backgroundTexture "resources/sprites/background2.jpg"//background.jpg for shorter background

#define soundChannel 5
#define fireSound "resources/sound/laser.ogg"
#define explosionSound "resources/sound/explosion.ogg"
#define bgSound "resources/sound/background.mp3"
#define btSound "resources/sound/button.ogg"

enum{
	pPlane,
	ePlane,
	ePlane2,
	eBullet,
	shipDebris,
	bonusHP,
	enchanceATK,
};

enum{
	normalBullet,
	waveBullet
};

enum{
	CH_MUSIC,
	CH_MENU,
	CH_PLAYER,
	CH_ENEMY,
	CH_OTHER
};

enum{
	SOUND_FIRE,
	SOUND_EXPLOSION,
	SOUND_BUTTON
};
