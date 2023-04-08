#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TARGET_FRAME_RATE 60.0
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define CAMERA_SPEED 5

typedef struct
{
	int x;
	int y;
} Camera;

Camera init_camera(void);
void SDL_ExitWithError(const char *message);
void SDL_RenderDrawPixel(SDL_Renderer *renderer, const int x, const int y);
SDL_Texture *SDL_CreateTextureFromBMP(SDL_Renderer *renderer, const char *file);
int x_by_camera(const int x, const Camera camera);
int y_by_camera(const int y, const Camera camera);
SDL_Rect rectangle_copy(const SDL_Rect source);
SDL_Rect rectangle_by_camera(const SDL_Rect source, const Camera camera);

int main(int argc, char *argv[])
{
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
	Camera camera = init_camera();

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		SDL_ExitWithError("Erreur init SDL");

	if (SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer) != 0)
		SDL_ExitWithError("Erreur create window or renderer");
	SDL_SetWindowTitle(window, "Mon titre perso");

	SDL_Texture *slime = SDL_CreateTextureFromBMP(renderer, "slime.bmp");

	SDL_Rect slime_position;
	if (SDL_QueryTexture(slime, NULL, NULL, &slime_position.w, &slime_position.h) != 0)
	{
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_ExitWithError("Erreur query texture");
	}
	slime_position.x = -200;
	slime_position.y = -300;

	uint64_t last_update = SDL_GetTicks64();

	const float FRAME_RATE_TIME = 1000.0 / TARGET_FRAME_RATE;
	SDL_Log("avg %f fps\n", FRAME_RATE_TIME);
	unsigned int frames = 0;

	SDL_bool is_running = SDL_TRUE;
	while (is_running)
	{
		const uint64_t ticks_now = SDL_GetTicks64();
		const float fps = frames / (ticks_now / 1000.0);
		SDL_Log("%.2f fps\n", fps);
		const uint64_t delta = ticks_now - last_update;
		SDL_Log("delta is %d ms\n", delta);
		last_update = ticks_now;

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				is_running = SDL_FALSE;
				break;
			case SDL_KEYUP:
				if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					is_running = SDL_FALSE;
				}
				break;
			case SDL_KEYDOWN:
				const SDL_Keycode key = event.key.keysym.sym;
				switch (key)
				{
				case SDLK_LEFT:
					camera.x = camera.x - CAMERA_SPEED;
					break;
				case SDLK_RIGHT:
					camera.x = camera.x + CAMERA_SPEED;
					break;
				case SDLK_UP:
					camera.y = camera.y - CAMERA_SPEED;
					break;
				case SDLK_DOWN:
					camera.y = camera.y + CAMERA_SPEED;
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 1);
		SDL_RenderClear(renderer);

		SDL_Rect offset = rectangle_by_camera(slime_position, camera);
		SDL_RenderCopy(renderer, slime, NULL, &offset);
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 1);
		SDL_RenderDrawPixel(renderer, x_by_camera(50, camera), y_by_camera(100, camera));

		SDL_RenderPresent(renderer);

		const uint64_t waiting_time = fmaxf(FRAME_RATE_TIME - (SDL_GetTicks64() - last_update), 0);
		if (waiting_time > 0)
			SDL_Delay(waiting_time);
		frames++;
	}

	SDL_DestroyTexture(slime);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}

Camera init_camera(void)
{
	Camera camera = {.x = 0, .y = 0};
	return camera;
}

void SDL_ExitWithError(const char *message)
{
	SDL_LogError(0, "%s > %s\n", message, SDL_GetError());
	SDL_Quit();
	exit(EXIT_FAILURE);
}

void SDL_RenderDrawPixel(SDL_Renderer *renderer, const int x, const int y)
{
	SDL_Rect pixel;
	pixel.x = x;
	pixel.y = y;
	pixel.w = 8;
	pixel.h = 8;
	SDL_RenderFillRect(renderer, &pixel);
}

SDL_Texture *SDL_CreateTextureFromBMP(SDL_Renderer *renderer, const char *file)
{
	SDL_Surface *surface = NULL;
	SDL_Texture *texture = NULL;
	surface = SDL_LoadBMP("slime.bmp");
	if (surface == NULL)
	{
		SDL_ExitWithError("Erreur init image");
	}

	texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	if (texture == NULL)
	{
		SDL_ExitWithError("Erreur init texture");
	}
	return texture;
}

int x_by_camera(const int x, const Camera camera)
{
	return x - camera.x;
}

int y_by_camera(const int y, const Camera camera)
{
	return y - camera.y;
}

SDL_Rect rectangle_copy(const SDL_Rect source)
{
	SDL_Rect result = {.x = source.x, .y = source.y, .w = source.w, .h = source.h};
	return result;
}

SDL_Rect rectangle_by_camera(const SDL_Rect source, const Camera camera)
{
	SDL_Rect result = rectangle_copy(source);
	result.x = result.x - camera.x;
	result.y = result.y - camera.y;
	return result;
}
