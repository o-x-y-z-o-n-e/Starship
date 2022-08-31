#ifdef SYS_WINDOWS
#include "platform/window.h"
#include "core/core.h"
#include "video/renderer.h"
#include "util/debug.h"

#include <windows.h>

#define CLASS_NAME L"CLASS_NAME"
#define WINDOW_NAME L"Game Title"

static uint16_t window_width = 640;
static uint16_t window_height = 400;

static WNDCLASS config;
static HWND handle;
static HDC device_context;

struct {
	BITMAPINFO bitmap_info;
	HBITMAP bitmap;
	viewport_t viewport;
	uint16_t x_offset;
	uint16_t y_offset;
} static frame;


//------------------------------------------------------------------------------


void create_frame_buffer() {
	uint16_t w_scale = window_width / frame.viewport.width;
	uint16_t h_scale = window_height / frame.viewport.height;

	frame.viewport.scale = w_scale;
	if(h_scale < w_scale)
		frame.viewport.scale = h_scale;

	if(frame.viewport.scale == 0)
		frame.viewport.scale = 1;

	uint16_t frame_width = frame.viewport.width * frame.viewport.scale;
	uint16_t frame_height = frame.viewport.height * frame.viewport.scale;

	frame.x_offset = (window_width - frame_width) / 2;
	frame.y_offset = (window_height - frame_height) / 2;

	frame.bitmap_info.bmiHeader.biWidth = frame_width;
	frame.bitmap_info.bmiHeader.biHeight = frame_height;

	if(frame.bitmap)
		DeleteObject(frame.bitmap);

	frame.bitmap = CreateDIBSection(NULL, &frame.bitmap_info, DIB_RGB_COLORS, &(frame.viewport.buffer), 0, 0);
	if(frame.bitmap == NULL)
		return;

	SelectObject(device_context, frame.bitmap);
}


//------------------------------------------------------------------------------


LRESULT CALLBACK on_window_event(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch(msg) {
		case WM_CLOSE: {
			close_core();
			break;
		}

		case WM_DESTROY: {
			PostQuitMessage(0);
			break;
		}
			
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);

			if(wParam == 1) {
				// Clears screen.
				BitBlt(hdc, 0, 0, window_width, window_height, 0, 0, 0, BLACKNESS);
			} else {
				// Copies frame buffer to GDI.
				int w = frame.viewport.width * frame.viewport.scale;
				int h = frame.viewport.height * frame.viewport.scale;

				StretchBlt(
					hdc,
					frame.x_offset,
					frame.y_offset+h,
					w,
					-h,
					device_context,
					0,
					0,
					w,
					h,
					SRCCOPY
				);
			}

			EndPaint(hwnd, &ps);
			break;
		}

		case WM_SIZE: {
			window_width = LOWORD(lParam);
			window_height = HIWORD(lParam);

			log_info("New window dimensions [%u, %u]", window_width, window_height);
			create_frame_buffer();

			InvalidateRect(handle, NULL, FALSE);
			SendMessage(handle, WM_PAINT, 1, 0);
			break;
		}
			
		default: {
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
	}

	return 0;
}


//------------------------------------------------------------------------------


int create_window() {
	if(handle != NULL) {
		log_error("Window already exists!");
		return 0;
	}

	config.lpfnWndProc = on_window_event;
	config.hInstance = GetModuleHandle(NULL);
	config.lpszClassName = CLASS_NAME;
	//config.style = 0;
	//config.cbClsExtra = 0;
	//config.cbWndExtra = 0;
	//config.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	config.hCursor = LoadCursor(NULL, IDC_ARROW);
	//config.hbrBackground = (HBRUSH)(GRAY_BRUSH);
	//config.lpszMenuName = NULL;
	

	if(!RegisterClass(&config)) {
		log_error("Window registration failed!");
		return 0;
	}

	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	device_context = CreateCompatibleDC(0);

	frame.bitmap_info.bmiHeader.biSize = sizeof(frame.bitmap_info.bmiHeader);
	frame.bitmap_info.bmiHeader.biPlanes = 1;
	frame.bitmap_info.bmiHeader.biBitCount = 32;
	frame.bitmap_info.bmiHeader.biCompression = BI_RGB;
	frame.viewport = (viewport_t){NULL, 320, 200, 1};
	
	handle = CreateWindow(
		CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		window_width,
		window_height,
		NULL,
		NULL,
		config.hInstance,
		NULL
	);

	if(handle == NULL) {
		log_error("Window creation failed!");
		return 0;
	}

	SendMessage(handle, WM_SIZE, 0, (window_width) + (window_height << 16));

	return 1;
}


//------------------------------------------------------------------------------


void poll_window_events() {
	MSG msg;

	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


//------------------------------------------------------------------------------


void close_window() {
	if(handle != NULL) {
		DestroyWindow(handle);
	}
}


//------------------------------------------------------------------------------


void refresh_window() {
	InvalidateRect(handle, NULL, FALSE);
	UpdateWindow(handle);
}


//------------------------------------------------------------------------------


void set_viewport(uint16_t width, uint16_t height) {
	if(handle == NULL) {
		log_error("Window not created yet");
		return;
	}

	frame.viewport.width = width;
	frame.viewport.height = height;

	create_frame_buffer();
}


//------------------------------------------------------------------------------


viewport_t* get_viewport() {
	return &frame.viewport;
}


#endif