#include "../header/controls.h"
#include "../assets/playerSprite.h"
#include <math.h>

float speed = 300;
Vector2 getMovementVector() {
	Vector2 movement = { 0.0f, 0.0f };

	if (getControls().left) {
		movement.x -= 1.0f;
	}

	if (getControls().right) {
		movement.x += 1.0f;
	}

	if (getControls().up) {
		movement.y -= 1.0f;
	}

	if (getControls().down) {
		movement.y += 1.0f;
	}

	if (movement.x != 0 || movement.y != 0) {
		float length = sqrtf(movement.x * movement.x + movement.y * movement.y);
		movement.x /= length;
		movement.y /= length;
	}

	return movement;
}

float normalX = 0;
float normalY = 0;
void handleMovement(Rectangle* player, float deltaTime) {
	Vector2 movement = getMovementVector();

	normalX = movement.x;
	normalY = movement.y;

	player->x += movement.x * speed * deltaTime;
	player->y += movement.y * speed * deltaTime;
}

void ResolveCollision(Rectangle* player, Rectangle* wall) {
	// Calculate the intersection rectangle
	float overlapX = fminf(player->x + player->width, wall->x + wall->width) - fmaxf(player->x, wall->x);
	float overlapY = fminf(player->y + player->height, wall->y + wall->height) - fmaxf(player->y, wall->y);

	// Determine which direction has the smallest overlap
	if (overlapX < overlapY) {
		// Resolve collision along the X axis
		if (player->x < wall->x) {
			// Player is to the left of the wall
			player->x = wall->x - player->width;
		}
		if (player->x > wall->x) {
			// Player is to the right of the wall
			player->x = wall->x + wall->width;
		}
	}
	if (overlapX > overlapY) {
		// Resolve collision along the Y axis
		if (player->y < wall->y) {
			// Player is above the wall
			player->y = wall->y - player->height;
		}
		if (player->y > wall->y) {
			// Player is below the wall
			player->y = wall->y + wall->height;
		}
	}
}

void generateRoom(int room[], Rectangle rectangles[], Vector2 positions[], Color colors[], Vector2 axis, Rectangle solids[]) {
	int count = 0;
	int wallCount = 0;

	for (int x = 0; x < axis.x; x++) {
		for (int y = 0; y < axis.y; y++) {
			rectangles[count].x = 50 * x;
			rectangles[count].y = 50 * y;
			rectangles[count].width = 50;
			rectangles[count].height = 50;
			
			// floor
			if (room[x + y * (int)axis.x] == 625) {
				colors[count] = (Color){ 0x35, 0x34, 0x36, 0xFF };
			}

			// wall
			if (room[x + y * (int)axis.x] == 415) {
				solids[wallCount] = rectangles[count];
				wallCount++;
				colors[count] = (Color){ 0x5c, 0x5c, 0x5c, 0xFF };
			}
			count++;
		}
	}
}

int main() {
	int screenWidth = 800;
	int screenHeight = 700;
	int targetFPS = 60;

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(screenWidth, screenHeight, "Room Transition");
	SetTargetFPS(targetFPS);
	
	Rectangle player = { 200, 200, 14, 15 };

	Camera2D camera = { 0 };
	camera.zoom = 2.f;
	camera.target = (Vector2){player.x, player.y};
	camera.offset = (Vector2){ GetScreenWidth() / 2, GetScreenHeight() / 2 };

	int room[] = {
		415,415,415,415,415,415,415,
		415,625,625,625,625,625,415,
		415,625,625,625,625,625,415,
		415,625,625,625,625,625,415,
		415,625,625,625,625,625,415,
		625,625,625,625,625,625,415,
		415,415,415,415,415,415,415
	};
	int totalTiles = sizeof(room) / sizeof(room[0]);
	Rectangle rectangles[49];
	Rectangle solids[24];
	Vector2 positions[49];
	Color colors[49];
	generateRoom(room, rectangles, positions, colors, (Vector2) { 7, 7 }, solids);


	int room2[] = {
		415,415,415,415,415,415,415,
		415,625,625,625,625,625,415,
		415,625,625,625,625,625,625,
		415,415,415,415,415,415,415
	};
	int totalTiles2 = sizeof(room2) / sizeof(room2[0]);
	Rectangle rectangles2[49];
	Rectangle solids2[18];
	Vector2 positions2[49];
	Color colors2[49];
	generateRoom(room2, rectangles2, positions2, colors2, (Vector2) { 7, 4 }, solids2);

	float deltaTime = GetFrameTime();

	int currentRoom = 0;
	Rectangle door = { 0, 250, 50, 50 };

	Image image = {
		.width = PLAYERSPRITE_WIDTH,
		.height = PLAYERSPRITE_HEIGHT,
		.data = PLAYERSPRITE_DATA,
		.format = PLAYERSPRITE_FORMAT,
		.mipmaps = 1
	};

	Texture2D playerSprite = LoadTextureFromImage(image);
	Rectangle playerFrame = { 0, 0, 24, 24 };
	int firstFrame = 0;
	int lastFrame = 1;

	double currentTime = GetFrameTime();
	double accumulator = 0.0;
	const double timeStep = 1.0 / 60.0;
	int frameCounter = 0;
	int frameSpeed = 4;


	float currentFrame = 0;
	int dir = 0;

	while (!WindowShouldClose()) {
		BeginDrawing();
			ClearBackground((Color) { 0x17, 0x17, 0x17, 0xFF });
			deltaTime = GetFrameTime();
			double newTime = GetTime();
			double frameTime = newTime - currentTime;
			currentTime = newTime;
			accumulator += frameTime;

			if (accumulator >= timeStep) {
				frameCounter++;
				accumulator -= timeStep;
			}

			if (frameCounter >= (60 / frameSpeed)) {
				currentFrame++;

				if (currentFrame > lastFrame) {
					currentFrame = firstFrame;
				}

				playerFrame.x = currentFrame * 24;
				frameCounter = 0;
			}

			if (IsKeyPressed(KEY_D)) {
				playerFrame.y = 24 * 2;
				playerFrame.x = 0;
				firstFrame = 0;
				lastFrame = 7;
				currentFrame = 0;
				frameSpeed = 8;
				dir = 1;
			}

			if (IsKeyPressed(KEY_A)) {
				playerFrame.y = 24;
				playerFrame.x = 0;
				firstFrame = 0;
				lastFrame = 7;
				currentFrame = 0;
				frameSpeed = 8;
				dir = 0;
			}

			if (IsKeyPressed(KEY_W) && dir == 0 || IsKeyPressed(KEY_S) && dir == 0) {
				playerFrame.y = 24;
				playerFrame.x = 0;
				firstFrame = 0;
				lastFrame = 7;
				currentFrame = 0;
				frameSpeed = 8;
			}

			if (IsKeyPressed(KEY_W) && dir == 1 || IsKeyPressed(KEY_S) && dir == 1) {
				playerFrame.y = 24 * 2;
				playerFrame.x = 0;
				firstFrame = 0;
				lastFrame = 7;
				currentFrame = 0;
				frameSpeed = 8;
				dir = 1;
			}

			if (IsKeyReleased(KEY_W) && dir == 0 && !IsKeyDown(KEY_D) && !IsKeyDown(KEY_A) || IsKeyReleased(KEY_S) && dir == 0 && !IsKeyDown(KEY_D) && !IsKeyDown(KEY_A)) {
				currentFrame = 1;
				firstFrame = 0;
				lastFrame = 1;
				playerFrame.x = currentFrame * 24;
				playerFrame.y = 0;
				frameSpeed = 4;
			}

			if (IsKeyReleased(KEY_W) && dir == 1 && !IsKeyDown(KEY_D) && !IsKeyDown(KEY_A) || IsKeyReleased(KEY_S) && dir == 1 && !IsKeyDown(KEY_D) && !IsKeyDown(KEY_A)) {
				currentFrame = 3;
				firstFrame = 2;
				lastFrame = 3;
				playerFrame.x = currentFrame * 24;
				playerFrame.y = 0;
				frameSpeed = 4;
			}


			if (IsKeyReleased(KEY_D) && !IsKeyDown(KEY_D) && !IsKeyDown(KEY_A)) {
				// idle right
				currentFrame = 3;
				firstFrame = 2;
				lastFrame = 3;
				playerFrame.x = currentFrame * 24;
				playerFrame.y = 0;
				frameSpeed = 4;
			}

			if (IsKeyReleased(KEY_A) && !IsKeyDown(KEY_D) && !IsKeyDown(KEY_A)) {
				// idle left
				currentFrame = 1;
				firstFrame = 0;
				lastFrame = 1;
				playerFrame.x = currentFrame * 24;
				playerFrame.y = 0;
				frameSpeed = 4;
			}

			if (GetScreenWidth() != screenWidth || GetScreenHeight() != screenHeight) {
				screenWidth = GetScreenWidth();
				screenHeight = GetScreenHeight();

				camera.offset = (Vector2){ GetScreenWidth() / 2, GetScreenHeight() / 2 };
			}
		
			handleMovement(&player, deltaTime);

			// here
			if (currentRoom == 0) {
				for (int i = 0; i < 24; i++) {
					if (CheckCollisionRecs(player, solids[i])) {
						ResolveCollision(&player, &solids[i]);
					}
				}
			}
			
			if (currentRoom == 1) {
				for (int i = 0; i < 18; i++) {
					if (CheckCollisionRecs(player, solids2[i])) {
						ResolveCollision(&player, &solids2[i]);
					}
				}
			}
			
			if (CheckCollisionRecs(player, door)) {
				if (currentRoom == 0) {
					currentRoom = 1;
					door.x = 300;
					door.y = 100;
					player.x = door.x - 50;
					player.y = door.y + door.height / 2;
				}
				else if (currentRoom == 1) {
					door.x = 0;
					door.y = 250;
					player.x = door.x + 50;
					player.y = door.y + door.height / 2;
					currentRoom = 0;
				}
			}

			camera.target = (Vector2){ player.x, player.y };
			BeginMode2D(camera);
				if (currentRoom == 0) {
					for (int i = 0; i < totalTiles; i++) {
						DrawRectangleRec(rectangles[i], colors[i]);
					}
					
				}

				if (currentRoom == 1) {
					for (int i = 0; i < totalTiles2; i++) {
						DrawRectangleRec(rectangles2[i], colors2[i]);
					}
				}
				DrawRectangleRec(door, BLUE);
				DrawText("Door", door.x, door.y, 10, WHITE);
				// DrawRectangleRec(player, WHITE);
				DrawTextureRec(playerSprite, playerFrame, (Vector2) { player.x - 7, player.y - 7 }, WHITE);

			EndMode2D();
		EndDrawing();
	}


	CloseWindow();
	return 0;
}

