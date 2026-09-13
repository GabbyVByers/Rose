
/*
 *   Source File [rose.c]
 */

#include "rose.h"

typedef struct ROSE_Image {
	size_t w, h;
	uint8_t* pixels;
} ROSE_Image;

typedef struct ROSE_Sprite {
	size_t w, h;
	SDL_GPUBuffer* buffer;
	SDL_GPUTexture* texture;
} ROSE_Sprite;

typedef struct ROSE_Text {
	size_t w, h;
	size_t num_vertices;
	SDL_GPUBuffer* buffer;
} ROSE_Text;

static bool rose = false;
static size_t screen_width = 0;
static size_t screen_height = 0;
static SDL_Window* window = NULL;
static SDL_GPUDevice* device = NULL;
static SDL_GPUSampler* sampler = NULL;
static SDL_GPUTexture* depth_texture = NULL;
static SDL_GPUGraphicsPipeline* graphics_pipeline = NULL;
static SDL_GPUTexture* ascii_texture = NULL;

static bool frame = false;
static bool minimized = false;
static SDL_GPURenderPass* render_pass = NULL;
static SDL_GPUTexture* swapchain_texture = NULL;
static SDL_GPUCommandBuffer* command_buffer = NULL;

static float mouse_px = 0.0f;
static float mouse_py = 0.0f;
static float mouse_vx = 0.0f;
static float mouse_vy = 0.0f;
static float saved_mouse_px = 0.0f;
static float saved_mouse_py = 0.0f;
static int curr_mouse_state = 0;
static int prev_mouse_state = 0;
static float mouse_scroll = 0.0f;

static bool curr_keyboard_state[SDL_SCANCODE_COUNT] = { 0 };
static bool prev_keyboard_state[SDL_SCANCODE_COUNT] = { 0 };

/*
 *   INIT / QUIT
 */

void ROSE_Init(const char* title, size_t w, size_t h, bool vkdebug) {
	if (rose) {
		const char* message = "ROSE has Already been Initialized!";
		fprintf(stderr, "ROSE_Init() Failed: %s", message);
		exit(EXIT_FAILURE);
	} rose = true;

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "SDL_Init() Failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	if (w < ROSE_MIN_WIDTH) {
		w = ROSE_MIN_WIDTH;
	} screen_width = w;
	
	if (h < ROSE_MIN_HEIGHT) {
		h = ROSE_MIN_HEIGHT;
	} screen_height = h;

	window = SDL_CreateWindow(title, screen_width, screen_height, SDL_WINDOW_RESIZABLE);
	if (!window) {
		SDL_Log("SDL_CreateWindow() Failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, vkdebug, NULL);
	if (!device) {
		SDL_Log("SDL_CreateGPUDevice() Failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	if (!SDL_ClaimWindowForGPUDevice(device, window)) {
		SDL_Log("SDL_ClaimWindowForGPUDevice() Failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	if (!SDL_SetGPUSwapchainParameters(device, window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_VSYNC)) {
		SDL_Log("SDL_SetGPUSwapchainParameters() Failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	if (!SDL_SetWindowMinimumSize(window, ROSE_MIN_WIDTH, ROSE_MIN_HEIGHT)) {
		SDL_Log("SDL_SetWindowMinimumSize() Failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	const char* vertex_path = "Shaders/vertex.spv";
	const char* fragment_path = "Shaders/fragment.spv";

	FILE* vertex_file = fopen(vertex_path, "rb");
	FILE* fragment_file = fopen(fragment_path, "rb");

	if (!vertex_file) {
		fprintf(stderr, "Couldn't Open: %s\n", vertex_path);
		exit(EXIT_FAILURE);
	}

	if (!fragment_file) {
		fprintf(stderr, "Couldn't Open: %s\n", fragment_path);
		exit(EXIT_FAILURE);
	}

	fseek(vertex_file, 0, SEEK_END);
	fseek(fragment_file, 0, SEEK_END);
	size_t vertex_code_size = ftell(vertex_file);
	size_t fragment_code_size = ftell(fragment_file);
	rewind(vertex_file);
	rewind(fragment_file);

	unsigned char* vertex_code = malloc(vertex_code_size);
	unsigned char* fragment_code = malloc(fragment_code_size);

	if (!vertex_code) {
		const char* message = "OOM!";
		fprintf(stderr, "ROSE_Init() Failed: %s\n", message);
		exit(EXIT_FAILURE);
	}

	if (!fragment_code) {
		const char* message = "OOM!";
		fprintf(stderr, "ROSE_Init() Failed: %s\n", message);
		exit(EXIT_FAILURE);
	}

	fread(vertex_code, 1, vertex_code_size, vertex_file);
	fread(fragment_code, 1, fragment_code_size, fragment_file);

	SDL_GPUShaderCreateInfo vertex_shader_create_info = {
		.code_size = vertex_code_size,
		.code = vertex_code,
		.entrypoint = "main",
		.format = SDL_GPU_SHADERFORMAT_SPIRV,
		.stage = SDL_GPU_SHADERSTAGE_VERTEX,
		.num_uniform_buffers = 1,
	};

	SDL_GPUShaderCreateInfo fragment_shader_create_info = {
		.code_size = fragment_code_size,
		.code = fragment_code,
		.entrypoint = "main",
		.format = SDL_GPU_SHADERFORMAT_SPIRV,
		.stage = SDL_GPU_SHADERSTAGE_FRAGMENT,
		.num_samplers = 1,
	};

	SDL_GPUShader* vertex_shader_program = SDL_CreateGPUShader(device, &vertex_shader_create_info);
	if (!vertex_shader_program) {
		fprintf(stderr, "SDL_CreateGPUShader() Failed: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	SDL_GPUShader* fragment_shader_program = SDL_CreateGPUShader(device, &fragment_shader_create_info);
	if (!fragment_shader_program) {
		fprintf(stderr, "SDL_CreateGPUShader() Failed: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	SDL_GPUVertexAttribute position_attribute = {
		.location = 0,
		.buffer_slot = 0,
		.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
		.offset = offsetof(ROSE_Vertex, pos),
	};

	SDL_GPUVertexAttribute texcoords_attribute = {
		.location = 1,
		.buffer_slot = 0,
		.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
		.offset = offsetof(ROSE_Vertex, uv),
	};

	SDL_GPUVertexAttribute vertex_attributes[2] = {
		position_attribute,
		texcoords_attribute,
	};

	SDL_GPUVertexBufferDescription vertex_buffer_description = {
		.pitch = sizeof(ROSE_Vertex),
		.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
	};

	SDL_GPUColorTargetDescription color_target_description = {
		.format = SDL_GetGPUSwapchainTextureFormat(device, window),
		.blend_state = {
			.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
			.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
			.color_blend_op = SDL_GPU_BLENDOP_ADD,
			.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
			.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
			.alpha_blend_op = SDL_GPU_BLENDOP_ADD,
			.enable_blend = true,
		},
	};

	SDL_GPUSamplerCreateInfo sampler_create_info = {
		.min_filter = SDL_GPU_FILTER_NEAREST,
		.mag_filter = SDL_GPU_FILTER_NEAREST,
		.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
		.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
		.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
		.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
	};

	SDL_GPUTextureCreateInfo depth_texture_create_info = {
		.type = SDL_GPU_TEXTURETYPE_2D,
		.format = SDL_GPU_TEXTUREFORMAT_D16_UNORM,
		.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET,
		.width = screen_width,
		.height = screen_height,
		.layer_count_or_depth = 1,
		.num_levels = 1,
		.sample_count = SDL_GPU_SAMPLECOUNT_1
	};

	SDL_GPUGraphicsPipelineCreateInfo graphics_pipeline_create_info = {
		.vertex_shader = vertex_shader_program,
		.fragment_shader = fragment_shader_program,
		.vertex_input_state = {
			.vertex_buffer_descriptions = &vertex_buffer_description,
			.num_vertex_buffers = 1,
			.vertex_attributes = vertex_attributes,
			.num_vertex_attributes = sizeof(vertex_attributes) / sizeof(vertex_attributes[0]),
		},
		.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
		.depth_stencil_state = {
			.compare_op = SDL_GPU_COMPAREOP_LESS_OR_EQUAL,
			.enable_depth_test = true,
			.enable_depth_write = true,
		},
		.target_info = {
			.color_target_descriptions = &color_target_description,
			.num_color_targets = 1,
			.depth_stencil_format = SDL_GPU_TEXTUREFORMAT_D16_UNORM,
			.has_depth_stencil_target = true,
		},
	};

	sampler = SDL_CreateGPUSampler(device, &sampler_create_info);
	if (!sampler) {
		fprintf(stderr, "SDL_CreateGPUSampler() Failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	depth_texture = SDL_CreateGPUTexture(device, &depth_texture_create_info);
	if (!depth_texture) {
		fprintf(stderr, "SDL_CreateGPUTexture() Failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	graphics_pipeline = SDL_CreateGPUGraphicsPipeline(device, &graphics_pipeline_create_info);
	if (!graphics_pipeline) {
		fprintf(stderr, "SDL_CreateGPUGraphicsPipeline() Failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	fclose(vertex_file);
	fclose(fragment_file);
	free(vertex_code);
	free(fragment_code);

	SDL_ReleaseGPUShader(device, vertex_shader_program);
	SDL_ReleaseGPUShader(device, fragment_shader_program);

	const char* ascii_path = "Resources/ascii.png";
	int ascii_w, ascii_h, n;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* ascii_image = stbi_load(ascii_path, &ascii_w, &ascii_h, &n, 4);
	size_t ascii_image_size = (size_t)ascii_w * (size_t)ascii_h * (size_t)4;

	if (!ascii_image) {
		fprintf(stderr, "Couldn't Open: %s\n", ascii_path);
		exit(EXIT_FAILURE);
	}

	ascii_texture = ROSE_INTERNAL_CreateRenderTexture(ascii_w, ascii_h);
	ROSE_INTERNAL_UploadImageToRenderTexture(ascii_image, ascii_w, ascii_h, ascii_texture);
	stbi_image_free(ascii_image);
}

void ROSE_Quit(void) {
	if (!rose) {
		const char* message = "ROSE has not been Initialized!";
		fprintf(stderr, "ROSE_Quit() Failed: %s", message);
		exit(EXIT_FAILURE);
	} rose = true;

	SDL_ReleaseGPUTexture(device, ascii_texture);
	SDL_ReleaseGPUSampler(device, sampler);
	SDL_ReleaseGPUTexture(device, depth_texture);
	SDL_ReleaseGPUGraphicsPipeline(device, graphics_pipeline);
	SDL_DestroyGPUDevice(device);
	SDL_DestroyWindow(window);
	SDL_Quit();

	screen_width = 0;
	screen_height = 0;
	window = NULL;
	device = NULL;
	sampler = NULL;
	depth_texture = NULL;
	graphics_pipeline = NULL;
	ascii_texture = NULL;

	frame = false;
	minimized = false;
	render_pass = NULL;
	swapchain_texture = NULL;
	command_buffer = NULL;

	mouse_px = 0.0f;
	mouse_py = 0.0f;
	mouse_vx = 0.0f;
	mouse_vy = 0.0f;
	saved_mouse_px = 0.0f;
	saved_mouse_py = 0.0f;
	curr_mouse_state = 0;
	prev_mouse_state = 0;

	memset(curr_keyboard_state, false, sizeof(bool) * SDL_SCANCODE_COUNT);
	memset(prev_keyboard_state, false, sizeof(bool) * SDL_SCANCODE_COUNT);
}

/*
 *   IMAGE HANDLE
 */

ROSE_Image* ROSE_LoadPNGImage(const char* path) {
	int w, h, n;
	stbi_set_flip_vertically_on_load(true);
	uint8_t* stb_image = stbi_load(path, &w, &h, &n, 4);
	if (!stb_image) {
		fprintf(stderr, "Couldn't Open: %s\n", path);
		exit(EXIT_FAILURE);
	}

	ROSE_Image* image = (ROSE_Image*)malloc(sizeof(ROSE_Image));
	if (!image) {
		const char* message = "OOM!";
		fprintf(stderr, "ROSE_LoadPNGImage() Failed: %s\n", message);
		exit(EXIT_FAILURE);
	}

	image->w = (size_t)w;
	image->h = (size_t)h;
	size_t buffer_size = image->w * image->h * (size_t)4;
	image->pixels = (uint8_t*)malloc(buffer_size);
	if (!image->pixels) {
		const char* message = "OOM!";
		fprintf(stderr, "ROSE_LoadPNGImage() Failed: %s\n", message);
		exit(EXIT_FAILURE);
	}

	memcpy(image->pixels, stb_image, buffer_size);
	stbi_image_free(stb_image);
	return image;
}

ROSE_Image* ROSE_CreateImage(size_t w, size_t h) {
	ROSE_Image* image = (ROSE_Image*)malloc(sizeof(ROSE_Image));
	if (!image) {
		const char* message = "OOM!";
		fprintf(stderr, "ROSE_LoadPNGImage() Failed: %s\n", message);
		exit(EXIT_FAILURE);
	}

	image->w = (size_t)w;
	image->h = (size_t)h;
	size_t buffer_size = image->w * image->h * (size_t)4;
	image->pixels = (uint8_t*)malloc(buffer_size);
	if (!image->pixels) {
		const char* message = "OOM!";
		fprintf(stderr, "ROSE_LoadPNGImage() Failed: %s\n", message);
		exit(EXIT_FAILURE);
	}

	memset(image->pixels, (uint8_t)255, buffer_size);
	return image;
}

ROSE_Color ROSE_GetImagePixel(ROSE_Image* image, size_t x, size_t y) {
	if (!image) {
		const char* message = "Image is NULL!";
		fprintf(stderr, "ROSE_GetImagePixel() Failed: %s", message);
		exit(EXIT_FAILURE);
	}

	bool bad_bounds = (x < (size_t)0) || (x >= image->w);
	bad_bounds = bad_bounds || ((y < (size_t)0) || (y >= image->h));
	if (bad_bounds) {
		const char* message = "Out of Bounds!";
		fprintf(stderr, "PutPixel() Failed: %s\n", message);
		exit(EXIT_FAILURE);
	}

	size_t index = ((((image->h - 1) - y) * image->w) + x) * (size_t)4;
	uint8_t r = image->pixels[index + 0];
	uint8_t g = image->pixels[index + 1];
	uint8_t b = image->pixels[index + 2];
	uint8_t a = image->pixels[index + 3];

	return (ROSE_Color) {
		.r = ((float)r / 255.0f),
		.g = ((float)g / 255.0f),
		.b = ((float)b / 255.0f),
		.a = ((float)a / 255.0f),
	};
}

void ROSE_SetImagePixel(ROSE_Image* image, size_t x, size_t y, ROSE_Color color) {
	if (!image) {
		const char* message = "Image is NULL!";
		fprintf(stderr, "ROSE_SetImagePixel() Failed: %s", message);
		exit(EXIT_FAILURE);
	}

	bool bad_bounds = (x < (size_t)0) || (x >= image->w);
	bad_bounds = bad_bounds || ((y < (size_t)0) || (y >= image->h));
	if (bad_bounds) {
		const char* message = "Out of Bounds!";
		fprintf(stderr, "PutPixel() Failed: %s\n", message);
		exit(EXIT_FAILURE);
	}

	size_t index = ((((image->h - 1) - y) * image->w) + x) * (size_t)4;
	image->pixels[index + 0] = (uint8_t)(color.r * 255.0f);
	image->pixels[index + 1] = (uint8_t)(color.g * 255.0f);
	image->pixels[index + 2] = (uint8_t)(color.b * 255.0f);
	image->pixels[index + 3] = (uint8_t)(color.a * 255.0f);
}

void ROSE_ImageSize(ROSE_Image* image, size_t* w, size_t* h) {
	if (!image) {
		const char* message = "Image is NULL!";
		fprintf(stderr, "ROSE_ImageSize() Failed: %s", message);
		exit(EXIT_FAILURE);
	}

	*w = image->w;
	*h = image->h;
}

void ROSE_DestroyImage(ROSE_Image* image) {
	if (!image) {
		const char* message = "Image is NULL!";
		fprintf(stderr, "ROSE_DestroyImage() Failed: %s", message);
		exit(EXIT_FAILURE);
	}

	free(image->pixels);
	free(image);
}

/*
 *   SPRITE
 */

ROSE_Sprite* ROSE_CreateSprite(ROSE_Image* image) {
	if (!rose) {
		const char* message = "ROSE has not been Initialized!";
		fprintf(stderr, "ROSE_CreateSprite() Failed: %s", message);
		exit(EXIT_FAILURE);
	}

	ROSE_Sprite* sprite = (ROSE_Sprite*)malloc(sizeof(ROSE_Sprite));
	if (!sprite) {
		const char* message = "OOM!";
		fprintf(stderr, "ROSE_CreateSprite() Failed: %s\n", message);
		exit(EXIT_FAILURE);
	}

	sprite->w = image->w;
	sprite->h = image->h;
	ROSE_Vertex vertices[6] = {
		{ { 0, 0 }, { 0, 1 } },
		{ { 1, 0 }, { 1, 1 } },
		{ { 1,-1 }, { 1, 0 } },
		{ { 0, 0 }, { 0, 1 } },
		{ { 1,-1 }, { 1, 0 } },
		{ { 0,-1 }, { 0, 0 } },
	};

	sprite->buffer = ROSE_INTERNAL_CreateVertexBuffer(vertices, sizeof(vertices) / sizeof(ROSE_Vertex));
	sprite->texture = ROSE_INTERNAL_CreateRenderTexture(image->w, image->h);
	ROSE_INTERNAL_UploadImageToRenderTexture(image->pixels, image->w, image->h, sprite->texture);
	return sprite;
}

void ROSE_UploadSpriteTexture(ROSE_Sprite* sprite, ROSE_Image* image) {
	if (!rose) {
		const char* message = "ROSE has not been Initialized!";
		fprintf(stderr, "ROSE_UploadSpriteTexture() Failed: %s", message);
		exit(EXIT_FAILURE);
	}

	if (!sprite) {
		const char* message = "Sprite is NULL!";
		fprintf(stderr, "ROSE_UploadSpriteTexture() Failed: %s", message);
		exit(EXIT_FAILURE);
	}

	if (!image) {
		const char* message = "Image is NULL!";
		fprintf(stderr, "ROSE_UploadSpriteTexture() Failed: %s", message);
		exit(EXIT_FAILURE);
	}

	if ((sprite->w == image->w) && (sprite->h == image->h)) {
		ROSE_INTERNAL_UploadImageToRenderTexture(image->pixels, image->w, image->h, sprite->texture);
		return;
	}

	sprite->w = image->w;
	sprite->h = image->h;
	SDL_ReleaseGPUTexture(device, sprite->texture);
	sprite->texture = ROSE_INTERNAL_CreateRenderTexture(image->w, image->h);
	ROSE_INTERNAL_UploadImageToRenderTexture(image->pixels, image->w, image->h, sprite->texture);
}

void ROSE_DestroySprite(ROSE_Sprite* sprite) {
	if (!rose) {
		const char* message = "ROSE has not been Initialized!";
		fprintf(stderr, "ROSE_DestroySprite() Failed: %s", message);
		exit(EXIT_FAILURE);
	}

	if (!sprite) {
		const char* message = "Sprite is NULL!";
		fprintf(stderr, "ROSE_DestroySprite() Failed: %s", message);
		exit(EXIT_FAILURE);
	}

	SDL_ReleaseGPUBuffer(device, sprite->buffer);
	SDL_ReleaseGPUTexture(device, sprite->texture);
	free(sprite);
}

/*
 *   TEXT
 */

ROSE_Text* ROSE_CreateText(const char* string) {
	if (!rose) {
		const char* message = "ROSE has not been Initialized!";
		fprintf(stderr, "ROSE_CreateText() Failed: %s", message);
		exit(EXIT_FAILURE);
	}

	ROSE_Text* text = (ROSE_Text*)malloc(sizeof(ROSE_Text));
	if (!text) {
		const char* message = "OOM!";
		fprintf(stderr, "ROSE_CreateText() Failed: %s\n", message);
		exit(EXIT_FAILURE);
	}
	
	size_t num_characters = 0;
	while (string[num_characters] != '\0') {
		num_characters++;
	}

	const size_t BUFFER_SIZE = 4096;
	if (num_characters >= BUFFER_SIZE) {
		static const char* message = "String Too Large!";
		fprintf(stderr, "CreateTextBox() Failed: %s\n", message);
		exit(EXIT_FAILURE);
	}

	size_t num_vertices = num_characters * (size_t)6;
	ROSE_Vertex* vertices = (ROSE_Vertex*)malloc(sizeof(ROSE_Vertex) * num_vertices);
	if (!vertices) {
		const char* message = "Returned NULL!";
		fprintf(stderr, "malloc() Failed: %s\n", message);
		exit(EXIT_FAILURE);
	}

	for (size_t i = 0; i < num_characters; i++) {
		size_t index = i * (size_t)6;
		char c = (string[i] - (uint8_t)32);
		float w = 1.0f / 95.0f;
		float x = w * ((float)c);
		vertices[index + 0] = (ROSE_Vertex){ { i + 0, 0 }, { x,     1 } };
		vertices[index + 1] = (ROSE_Vertex){ { i + 1, 0 }, { x + w, 1 } };
		vertices[index + 2] = (ROSE_Vertex){ { i + 1,-1 }, { x + w, 0 } };
		vertices[index + 3] = (ROSE_Vertex){ { i + 0, 0 }, { x,     1 } };
		vertices[index + 4] = (ROSE_Vertex){ { i + 1,-1 }, { x + w, 0 } };
		vertices[index + 5] = (ROSE_Vertex){ { i + 0,-1 }, { x,     0 } };
	}

	text->w = (size_t)7,
	text->h = (size_t)11,
	text->buffer = ROSE_INTERNAL_CreateVertexBuffer(vertices, num_vertices),
	text->num_vertices = num_vertices,
	free(vertices);
	return text;
}

void ROSE_DestroyText(ROSE_Text* text) {
	if (!rose) {
		const char* message = "ROSE has not been Initialized!";
		fprintf(stderr, "ROSE_DestroyText() Failed: %s", message);
		exit(EXIT_FAILURE);
	}

	if (!text) {
		const char* message = "Text is NULL!";
		fprintf(stderr, "ROSE_DestroyText() Failed: %s", message);
		exit(EXIT_FAILURE);
	}

	SDL_ReleaseGPUBuffer(device, text->buffer);
	free(text);
}

/*
 *   CORE RENDERING 
 */

void ROSE_ToggleVSync(bool vsync) {
	if (!rose) {
		const char* message = "ROSE has not been Initialized!";
		fprintf(stderr, "ROSE_ToggleVSync() Failed: %s", message);
		exit(EXIT_FAILURE);
	}

	if (vsync) {
		if (!SDL_SetGPUSwapchainParameters(device, window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_VSYNC)) {
			fprintf(stderr, "SDL_SetGPUSwapchainParameters() Failed: %s\n", SDL_GetError());
			exit(EXIT_FAILURE);
		} return;
	}

	const bool supports_immediate = SDL_WindowSupportsGPUPresentMode(device, window, SDL_GPU_PRESENTMODE_IMMEDIATE);
	if (supports_immediate) {
		if (!SDL_SetGPUSwapchainParameters(device, window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_IMMEDIATE)) {
			fprintf(stderr, "SDL_SetGPUSwapchainParameters() Failed: %s\n", SDL_GetError());
			exit(EXIT_FAILURE);
		} return;
	}

	const bool supports_mailbox = SDL_WindowSupportsGPUPresentMode(device, window, SDL_GPU_PRESENTMODE_MAILBOX);
	if (supports_mailbox) {
		if (!SDL_SetGPUSwapchainParameters(device, window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_MAILBOX)) {
			fprintf(stderr, "SDL_SetGPUSwapchainParameters() Failed: %s\n", SDL_GetError());
			exit(EXIT_FAILURE);
		} return;
	}

	if (!SDL_SetGPUSwapchainParameters(device, window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_VSYNC)) {
		fprintf(stderr, "SDL_SetGPUSwapchainParameters() Failed: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	} return;
}

void ROSE_GetScreenSize(size_t* w, size_t* h) {
	if (!rose) {
		const char* message = "ROSE has not been Initialized!";
		fprintf(stderr, "ROSE_GetScreenSize() Failed: %s", message);
		exit(EXIT_FAILURE);
	}

	*w = screen_width;
	*h = screen_height;
}

bool ROSE_PollEvents(void) {
	if (!rose) {
		const char* message = "ROSE has not been Initialized!";
		fprintf(stderr, "ROSE_PollEvents() Failed: %s", message);
		exit(EXIT_FAILURE);
	}

	mouse_scroll = 0.0f;
	mouse_vx = 0.0f;
	mouse_vy = 0.0f;
	
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
			return false;
		}
		if (event.type == SDL_EVENT_MOUSE_WHEEL) {
			mouse_scroll += event.wheel.y;
		}
		if (event.type == SDL_EVENT_MOUSE_MOTION) {
			mouse_vx += event.motion.xrel;
			mouse_vy += event.motion.yrel;
		}
	}
	
	memcpy(prev_keyboard_state, curr_keyboard_state, sizeof(bool) * SDL_SCANCODE_COUNT);
	memcpy(curr_keyboard_state, SDL_GetKeyboardState(NULL), sizeof(bool) * SDL_SCANCODE_COUNT);
	prev_mouse_state = curr_mouse_state;
	curr_mouse_state = SDL_GetMouseState(&mouse_px, &mouse_py);
	return true;
}

void ROSE_ClearScreen(ROSE_Color color) {
	if (!rose) {
		const char* message = "ROSE has not been Initialized!";
		fprintf(stderr, "ROSE_ClearScreen() Failed: %s", message);
		exit(EXIT_FAILURE);
	}

	if (frame) {
		const char* message = "Current Frame is Already Active!";
		fprintf(stderr, "ClearScreen() Failed: %s\n", message);
		exit(EXIT_FAILURE);
	} frame = true;

	command_buffer = SDL_AcquireGPUCommandBuffer(device);
	if (!command_buffer) {
		fprintf(stderr, "SDL_AcquireGPUCommandBuffer() Failed: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	unsigned int new_w, new_h;
	if (!SDL_WaitAndAcquireGPUSwapchainTexture(command_buffer, window, &swapchain_texture, &new_w, &new_h)) {
		fprintf(stderr, "SDL_WaitAndAcquireGPUSwapchainTexture() Failed: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	minimized = false;
	if ((new_w == 0) || (new_h == 0)) {
		minimized = true;
		if (!SDL_SubmitGPUCommandBuffer(command_buffer)) {
			fprintf(stderr, "SDL_SubmitGPUCommandBuffer() Failed: %s\n", SDL_GetError());
			exit(EXIT_FAILURE);
		}
		return;
	}

	const bool reconstruct_textures = ((size_t)new_w != screen_width) || ((size_t)new_h != screen_height);
	screen_width = (size_t)new_w;
	screen_height = (size_t)new_h;

	if (reconstruct_textures) {
		SDL_ReleaseGPUTexture(device, depth_texture);
		depth_texture = ROSE_INTERNAL_CreateDepthTexture();
	}

	SDL_GPUColorTargetInfo color_target_info = {
		.texture = swapchain_texture,
		.clear_color = {
			.r = color.r,
			.g = color.g,
			.b = color.b,
			.a = color.a,
		},
		.load_op = SDL_GPU_LOADOP_CLEAR,
		.store_op = SDL_GPU_STOREOP_STORE,
	};

	SDL_GPUDepthStencilTargetInfo depth_stencil_target_info = {
		.texture = depth_texture,
		.clear_depth = 1,
		.load_op = SDL_GPU_LOADOP_CLEAR,
		.store_op = SDL_GPU_STOREOP_DONT_CARE,
		.stencil_load_op = SDL_GPU_LOADOP_DONT_CARE,
		.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE,
		.cycle = true
	};

	render_pass = SDL_BeginGPURenderPass(command_buffer, &color_target_info, 1, &depth_stencil_target_info);
	if (!render_pass) {
		fprintf(stderr, "SDL_BeginGPURenderPass() Failed: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	} SDL_BindGPUGraphicsPipeline(render_pass, graphics_pipeline);
}

void ROSE_DrawSprite(ROSE_Sprite* sprite, size_t xpos, size_t ypos, double scale, ROSE_Color color) {
	if (!rose) {
		const char* message = "ROSE has not been Initialized!";
		fprintf(stderr, "ROSE_DrawSprite() Failed: %s", message);
		exit(EXIT_FAILURE);
	}

	if (!frame) {
		const char* message = "Cannot Draw on an Inactive Frame!";
		fprintf(stderr, "DrawSprite() Failed: %s\n", message);
		exit(EXIT_FAILURE);
	} if (minimized) { return; }

	if (!sprite) {
		const char* message = "Sprite is NULL!";
		fprintf(stderr, "ROSE_DrawSprite() Failed: %s", message);
		exit(EXIT_FAILURE);
	}

	SDL_GPUBufferBinding buffer_binding = {
		.buffer = sprite->buffer,
	};

	SDL_GPUTextureSamplerBinding texture_binding = {
		.texture = sprite->texture,
		.sampler = sampler,
	};

	double scaled_width = (double)sprite->w * scale;
	double scaled_height = (double)sprite->h * scale;

	double w = (scaled_width / (double)screen_width) * 2.0;
	double h = (scaled_height / (double)screen_height) * 2.0;
	double x = -1.0 + (((double)xpos / (double)screen_width) * 2.0);
	double y = 1.0 - (((double)ypos / (double)screen_height) * 2.0);

	typedef struct Uniform {
		float TransformMatrix[16];
		float TintColor[4];
	} Uniform;

	Uniform uniform = {
		.TransformMatrix = {
			w, 0, 0, 0,
			0, h, 0, 0,
			0, 0, 1, 0,
			x, y, 0, 1,
		},
		.TintColor = {
			color.r,
			color.g,
			color.b,
			color.a,
		}
	};

	SDL_PushGPUVertexUniformData(command_buffer, 0, ((void*)&uniform), sizeof(uniform));
	SDL_BindGPUVertexBuffers(render_pass, 0, &buffer_binding, 1);
	SDL_BindGPUFragmentSamplers(render_pass, 0, &texture_binding, 1);
	SDL_DrawGPUPrimitives(render_pass, 6, 1, 0, 0);
}

void ROSE_DrawText(ROSE_Text* text, size_t xpos, size_t ypos, double scale, ROSE_Color color) {
	if (!rose) {
		const char* message = "ROSE has not been Initialized!";
		fprintf(stderr, "ROSE_DrawText() Failed: %s", message);
		exit(EXIT_FAILURE);
	}

	if (!frame) {
		const char* message = "Cannot Draw on an Inactive Frame!";
		fprintf(stderr, "DrawTextBox() Failed: %s\n", message);
		exit(EXIT_FAILURE);
	} if (minimized) { return; }

	if (!text) {
		const char* message = "Text is NULL!";
		fprintf(stderr, "ROSE_DrawText() Failed: %s", message);
		exit(EXIT_FAILURE);
	}

	SDL_GPUBufferBinding buffer_binding = {
		.buffer = text->buffer,
	};

	SDL_GPUTextureSamplerBinding texture_binding = {
		.texture = ascii_texture,
		.sampler = sampler,
	};

	double scaled_width = (double)text->w * scale;
	double scaled_height = (double)text->h * scale;

	double w = (scaled_width / (double)screen_width) * 2.0;
	double h = (scaled_height / (double)screen_height) * 2.0;
	double x = -1.0 + (((double)xpos / (double)screen_width) * 2.0);
	double y = 1.0 - (((double)ypos / (double)screen_height) * 2.0);

	typedef struct Uniform {
		float TransformMatrix[16];
		float TintColor[4];
	} Uniform;

	Uniform uniform = {
		.TransformMatrix = {
			w, 0, 0, 0,
			0, h, 0, 0,
			0, 0, 1, 0,
			x, y, 0, 1,
		},
		.TintColor = {
			color.r,
			color.g,
			color.b,
			color.a,
		}
	};

	SDL_PushGPUVertexUniformData(command_buffer, 0, ((void*)&uniform), sizeof(uniform));
	SDL_BindGPUVertexBuffers(render_pass, 0, &buffer_binding, 1);
	SDL_BindGPUFragmentSamplers(render_pass, 0, &texture_binding, 1);
	SDL_DrawGPUPrimitives(render_pass, text->num_vertices, 1, 0, 0);
}

void ROSE_SwapBuffers(void) {
	if (!rose) {
		const char* message = "ROSE has not been Initialized!";
		fprintf(stderr, "ROSE_SwapBuffers() Failed: %s", message);
		exit(EXIT_FAILURE);
	}

	if (!frame) {
		const char* message = "Cannot SwapBuffers an Inactive Frame!";
		fprintf(stderr, "SwapBuffers() Failed: %s\n", message);
		exit(EXIT_FAILURE);
	} frame = false;
	if (minimized) { return; }

	SDL_EndGPURenderPass(render_pass);
	if (!SDL_SubmitGPUCommandBuffer(command_buffer)) {
		fprintf(stderr, "SDL_SubmitGPUCommandBuffer() Failed: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
}

/*
 *   MOUSE
 */

bool ROSE_HideMouseCursor(void) {
	if (!rose) {
		const char* message = "ROSE has not been Initialized!";
		fprintf(stderr, "ROSE_HideMouseCursor() Failed: %s", message);
		exit(EXIT_FAILURE);
	}

	saved_mouse_px = mouse_px;
	saved_mouse_py = mouse_py;
	SDL_SetWindowRelativeMouseMode(window, true);
}

bool ROSE_RevealMouseCursor(void) {
	if (!rose) {
		const char* message = "ROSE has not been Initialized!";
		fprintf(stderr, "ROSE_RevealMouseCursor() Failed: %s", message);
		exit(EXIT_FAILURE);
	}

	mouse_px = saved_mouse_px;
	mouse_py = saved_mouse_py;
	SDL_WarpMouseInWindow(window, mouse_px, mouse_py);
	SDL_SetWindowRelativeMouseMode(window, false);
}

bool ROSE_PressingMouseButton(int button) {
	if (!rose) {
		const char* message = "ROSE has not been Initialized!";
		fprintf(stderr, "ROSE_PressingMouseButton() Failed: %s", message);
		exit(EXIT_FAILURE);
	} return (curr_mouse_state & SDL_BUTTON_MASK(button)) != 0;
}

bool ROSE_PressedMouseButton(int button) {
	if (!rose) {
		const char* message = "ROSE has not been Initialized!";
		fprintf(stderr, "ROSE_PressedMouseButton() Failed: %s", message);
		exit(EXIT_FAILURE);
	} return ((prev_mouse_state & SDL_BUTTON_MASK(button)) == 0) && ((curr_mouse_state & SDL_BUTTON_MASK(button)) != 0);
}

bool ROSE_ReleasedMouseButton(int button) {
	if (!rose) {
		const char* message = "ROSE has not been Initialized!";
		fprintf(stderr, "ROSE_ReleasedMouseButton() Failed: %s", message);
		exit(EXIT_FAILURE);
	} return ((prev_mouse_state & SDL_BUTTON_MASK(button)) != 0) && ((curr_mouse_state & SDL_BUTTON_MASK(button)) == 0);
}

void ROSE_GetMousePosition(float* px, float* py) {
	if (!rose) {
		const char* message = "ROSE has not been Initialized!";
		fprintf(stderr, "ROSE_GetMousePosition() Failed: %s", message);
		exit(EXIT_FAILURE);
	}

	*px = mouse_px;
	*py = mouse_py;
}

void ROSE_GetMouseVelocity(float* vx, float* vy) {
	if (!rose) {
		const char* message = "ROSE has not been Initialized!";
		fprintf(stderr, "ROSE_GetMouseVelocity() Failed: %s", message);
		exit(EXIT_FAILURE);
	}

	*vx = mouse_vx;
	*vy = mouse_vy;
}

float ROSE_GetMouseScroll(void) {
	if (!rose) {
		const char* message = "ROSE has not been Initialized!";
		fprintf(stderr, "ROSE_GetMouseScroll() Failed: %s", message);
		exit(EXIT_FAILURE);
	} return mouse_scroll;
}

/*
 *   KEYBOARD
 */

bool ROSE_PressingKeyboardButton(int button) {
	if (!rose) {
		const char* message = "ROSE has not been Initialized!";
		fprintf(stderr, "ROSE_PressingKeyboardButton() Failed: %s", message);
		exit(EXIT_FAILURE);
	}

	if (button >= SDL_SCANCODE_COUNT) {
		const char* message = "Invalid Keyboard Button!";
		fprintf(stderr, "ROSE_PressingKeyboardButton() Failed: %s", message);
		exit(EXIT_FAILURE);
	} return curr_keyboard_state[button];
}

bool ROSE_PressedKeyboardButton(int button) {
	if (!rose) {
		const char* message = "ROSE has not been Initialized!";
		fprintf(stderr, "ROSE_PressedKeyboardButton() Failed: %s", message);
		exit(EXIT_FAILURE);
	}
	
	if (button >= SDL_SCANCODE_COUNT) {
		const char* message = "Invalid Keyboard Button!";
		fprintf(stderr, "ROSE_PressedKeyboardButton() Failed: %s", message);
		exit(EXIT_FAILURE);
	} return (!prev_keyboard_state[button]) && (curr_keyboard_state[button]);
}

bool ROSE_ReleasedKeyboardButton(int button) {
	if (!rose) {
		const char* message = "ROSE has not been Initialized!";
		fprintf(stderr, "ROSE_ReleasedKeyboardButton() Failed: %s", message);
		exit(EXIT_FAILURE);
	}
	
	if (button >= SDL_SCANCODE_COUNT) {
		const char* message = "Invalid Keyboard Button!";
		fprintf(stderr, "ROSE_ReleasedKeyboardButton() Failed: %s", message);
		exit(EXIT_FAILURE);
	} return (prev_keyboard_state[button]) && (!curr_keyboard_state[button]);
}

/*
 *   INTERNAL UTILITIES
 */

SDL_GPUTexture* ROSE_INTERNAL_CreateDepthTexture(void) {
	if (!rose) {
		const char* message = "ROSE has not been Initialized!";
		fprintf(stderr, "ROSE_INTERNAL_CreateDepthTexture() Failed: %s", message);
		exit(EXIT_FAILURE);
	}

	SDL_GPUTextureCreateInfo depth_texture_create_info = {
		.type = SDL_GPU_TEXTURETYPE_2D,
		.format = SDL_GPU_TEXTUREFORMAT_D16_UNORM,
		.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET,
		.width = screen_width,
		.height = screen_height,
		.layer_count_or_depth = 1,
		.num_levels = 1,
		.sample_count = SDL_GPU_SAMPLECOUNT_1
	};

	SDL_GPUTexture* texture = SDL_CreateGPUTexture(device, &depth_texture_create_info);
	if (!texture) {
		fprintf(stderr, "SDL_CreateGPUTexture() Failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	} return texture;
}

SDL_GPUTexture* ROSE_INTERNAL_CreateRenderTexture(size_t w, size_t h) {
	SDL_GPUTextureCreateInfo texture_create_info = {
		.type = SDL_GPU_TEXTURETYPE_2D,
		.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
		.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER,
		.width = w,
		.height = h,
		.layer_count_or_depth = 1,
		.num_levels = 1,
		.sample_count = SDL_GPU_SAMPLECOUNT_1,
	};

	SDL_GPUTexture* texture = SDL_CreateGPUTexture(device, &texture_create_info);
	if (!texture) {
		fprintf(stderr, "SDL_CreateGPUTexture() Failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	} return texture;
}

void ROSE_INTERNAL_UploadImageToRenderTexture(uint8_t* image, size_t w, size_t h, SDL_GPUTexture* texture) {
	if (!rose) {
		const char* message = "ROSE has not been Initialized!";
		fprintf(stderr, "ROSE_INTERNAL_UploadImageToRenderTexture() Failed: %s", message);
		exit(EXIT_FAILURE);
	}

	size_t image_size = w * h * (size_t)4;
	SDL_GPUTransferBufferCreateInfo transfer_buffer_create_info = {
		.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
		.size = image_size,
	};

	SDL_GPUTransferBuffer* transfer_buffer = SDL_CreateGPUTransferBuffer(device, &transfer_buffer_create_info);
	if (!transfer_buffer) {
		fprintf(stderr, "SDL_CreateGPUTransferBuffer() Failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	void* transfer_buffer_beginning = SDL_MapGPUTransferBuffer(device, transfer_buffer, false);
	if (!transfer_buffer_beginning) {
		fprintf(stderr, "SDL_MapGPUTransferBuffer() Failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	memcpy(transfer_buffer_beginning, image, image_size);

	SDL_GPUCommandBuffer* command_buffer = SDL_AcquireGPUCommandBuffer(device);
	if (!command_buffer) {
		fprintf(stderr, "SDL_AcquireGPUCommandBuffer() Failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(command_buffer);
	if (!copy_pass) {
		fprintf(stderr, "SDL_BeginGPUCopyPass() Failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	SDL_GPUTextureTransferInfo texture_transfer_info = {
		.transfer_buffer = transfer_buffer,
		.pixels_per_row = w,
		.rows_per_layer = h,
	};

	SDL_GPUTextureRegion destination_texture_region = {
		.texture = texture,
		.w = w,
		.h = h,
		.d = 1,
	};

	SDL_UploadToGPUTexture(copy_pass, &texture_transfer_info, &destination_texture_region, false);
	SDL_EndGPUCopyPass(copy_pass);

	if (!SDL_SubmitGPUCommandBuffer(command_buffer)) {
		fprintf(stderr, "SDL_SubmitGPUCommandBuffer() Failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	SDL_UnmapGPUTransferBuffer(device, transfer_buffer);
	SDL_ReleaseGPUTransferBuffer(device, transfer_buffer);
}

SDL_GPUBuffer* ROSE_INTERNAL_CreateVertexBuffer(ROSE_Vertex* vertices, size_t num_vertices) {
	if (!rose) {
		const char* message = "ROSE has not been Initialized!";
		fprintf(stderr, "ROSE_INTERNAL_CreateVertexBuffer() Failed: %s", message);
		exit(EXIT_FAILURE);
	}

	size_t buffer_size = num_vertices * sizeof(ROSE_Vertex);
	SDL_GPUBufferCreateInfo buffer_create_info = {
		.usage = SDL_GPU_BUFFERUSAGE_VERTEX,
		.size = buffer_size,
	};

	SDL_GPUBuffer* buffer = SDL_CreateGPUBuffer(device, &buffer_create_info);
	if (!buffer) {
		fprintf(stderr, "SDL_CreateGPUBuffer() Failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	SDL_GPUTransferBufferCreateInfo transfer_buffer_create_info = {
		.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
		.size = buffer_size,
	};

	SDL_GPUTransferBuffer* transfer_buffer = SDL_CreateGPUTransferBuffer(device, &transfer_buffer_create_info);
	if (!transfer_buffer) {
		fprintf(stderr, "SDL_CreateGPUTransferBuffer() Failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	void* transfer_buffer_beginning = SDL_MapGPUTransferBuffer(device, transfer_buffer, false);
	if (!transfer_buffer_beginning) {
		fprintf(stderr, "SDL_MapGPUTransferBuffer() Failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	memcpy(transfer_buffer_beginning, vertices, buffer_size);

	SDL_GPUCommandBuffer* command_buffer = SDL_AcquireGPUCommandBuffer(device);
	if (!command_buffer) {
		fprintf(stderr, "SDL_AcquireGPUCommandBuffer() Failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(command_buffer);
	if (!copy_pass) {
		fprintf(stderr, "SDL_BeginGPUCopyPass() Failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	SDL_GPUTransferBufferLocation source_buffer_location = {
		.transfer_buffer = transfer_buffer,
	};

	SDL_GPUBufferRegion destination_buffer_region = {
		.buffer = buffer,
		.size = buffer_size,
	};

	SDL_UploadToGPUBuffer(copy_pass, &source_buffer_location, &destination_buffer_region, true);
	SDL_EndGPUCopyPass(copy_pass);

	if (!SDL_SubmitGPUCommandBuffer(command_buffer)) {
		fprintf(stderr, "SDL_SubmitGPUCommandBuffer() Failed: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	SDL_UnmapGPUTransferBuffer(device, transfer_buffer);
	SDL_ReleaseGPUTransferBuffer(device, transfer_buffer);
	return buffer;
}

