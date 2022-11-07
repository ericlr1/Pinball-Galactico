#include "Player.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "Map.h"
#include "Map.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");

	//Pushback animation
	baseAnimation.PushBack({ 0, 0, 35, 30 });
	baseAnimation.PushBack({ 40, 0, 35, 30 });

	baseAnimation.loop;
	baseAnimation.speed = 0.05f;
}

Player::~Player() {

}

bool Player::Awake() {

	//L02: DONE 1: Initialize Player parameters
	//pos = position;
	//texturePath = "Assets/Textures/player/idle1.png";

	//L02: DONE 5: Get Player parameters from XML
	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	speed = parameters.attribute("speed").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool Player::Start() {

	//initilize textures
	playerTexture = app->tex->Load(texturePath);
	vidaTexture = app->tex->Load("Assets/Textures/vida.png");

	// L07 DONE 5: Add physics to the player - initialize physics body
	pbody = app->physics->CreateCircle(position.x+16, position.y+16, 16, bodyType::DYNAMIC);

	// L07 DONE 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this; 

	// L07 DONE 7: Assign collider type
	pbody->ctype = ColliderType::PLAYER;
	pbody->body->SetLinearVelocity(b2Vec2(0, -GRAVITY_Y));

	//initialize audio effect - !! Path is hardcoded, should be loaded from config.xml
	pickCoinFxId = app->audio->LoadFx("Assets/Audio/Fx/coinPickup.ogg");
	jumpFxId = app->audio->LoadFx("Assets/Audio/Fx/jump.wav");

	numJumps = 0;
	vidas = 3;
	godMode = false;

	currentAnimation = &baseAnimation;
	
	return true;
}

bool Player::Update()
{
	currentAnimation->Update();

	if (vidas == 0)
	{
		//Acabar la partida
		LOG("VIDAS = 0");
		//FadeToBlack -> Game Over 
		
	}

	// L07 DONE 5: Add physics to the player - updated player position using physics
	
	b2Vec2 vel = b2Vec2(0, -GRAVITY_Y);

	//Controles de debug
	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
		godMode = !godMode;
		numJumps = 0;

	}
	
	//Provisional para bajar las vidas
	if (app->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN) {
		vidas--;
	}
	
	 
	 
	if (godMode == false)
	{


		//L02: DONE 4: modify the position of the player using arrow keys and render the texture
		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			//
		}
		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			//
		}

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			vel = b2Vec2(-speed, -GRAVITY_Y);
			currentAnimation = &baseAnimation;
		}

		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			vel = b2Vec2(speed, -GRAVITY_Y);
		}

		if (position.x > 400 && position.x < 3382)
			app->render->camera.x = -position.x + 400;

		if (numJumps < 2)
		{
			//Salto
			if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {

				//Fuerza de salto
				salto = -30.0;

				on_floor = false;
				numJumps++;
				app->audio->PlayFx(jumpFxId);
			}
		}

		if (salto < 0.0)
		{
			vel.y = salto;
			salto+=3.5;
		}

		if (on_floor)
		{
			numJumps = 0;
		}
	}

	if (godMode == true)
	{
		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			//
		}
		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			//
		}

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			vel = b2Vec2(-speed, -GRAVITY_Y);
			currentAnimation = &baseAnimation;
		}

		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			vel = b2Vec2(speed, -GRAVITY_Y);
		}

		if (position.x > 400 && position.x < 3382)
			app->render->camera.x = -position.x + 400;

		//Salto
		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {

			//Fuerza de salto
			salto = -22.0;

			on_floor = false;
			numJumps++;
			app->audio->PlayFx(jumpFxId);
		}

		if (salto < 0.0)
		{
			vel.y = salto;
			salto++;
		}
	}
	


	//Set the velocity of the pbody of the player
	pbody->body->SetLinearVelocity(vel);

	//Update player position in pixels
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;

	//app->render->DrawTexture(texture, position.x, position.y);

	SDL_Rect rect = currentAnimation->GetCurrentFrame();

	app->render->DrawTexture(playerTexture, position.x, position.y, &rect);

	for (int i = 0; i < vidas; i++)
	{

		//app->render->Blit(App->UI->iconoVida, App->render->GetCameraCenterX() - 100 + (9 * i), App->render->GetCameraCenterY() + 120, NULL, 1.0, false);
		app->render->DrawTexture(vidaTexture, -app->render->camera.x + 50 + ( 35*i), -app->render->camera.y + 50);
	}


	return true;
}

bool Player::CleanUp()
{
	return true;
}



// L07 DONE 6: Define OnCollision function for the player. Check the virtual function on Entity class
void Player::OnCollision(PhysBody* physA, PhysBody* physB) {

	// L07 DONE 7: Detect the type of collision
	
	switch (physB->ctype)
	{
		case ColliderType::ITEM:
			LOG("Collision ITEM");
			app->audio->PlayFx(pickCoinFxId);
			break;
		case ColliderType::PLATFORM:
			LOG("Collision PLATFORM");
			if (physB->body->GetWorldCenter().y + 32 < position.y ) //Comprobación de que el collider está por debajo, es decir es el suelo y no el techo
			{
				on_floor = true;
			}
			

			break;
		case ColliderType::UNKNOWN:
			LOG("Collision UNKNOWN");
			break;
			
	}
	
}
