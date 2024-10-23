#if STANDARD_PLATFORM_WINDOWS == 1

#include "os.h"

#include <windows.h>
#include <windowsx.h> // param input extraction
#include <intrin.h> // rdtsc

#include "opengl_loader_win32.h"

#include "platform/log.h"

struct os_internal_state_st {
    platform_state_st* platform_state;
    // core
    f64 clock_frequency;
    LARGE_INTEGER start_time;
    // windowing
    HINSTANCE h_instance;
    WNDCLASSA wndclass;
    // opengl
    HGLRC glrc;
};
os_internal_state_st os_internal_state;


void os_init(platform_state_st* platform_state)
{
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    os_internal_state.clock_frequency = 1.0 / (f64)frequency.QuadPart;
    QueryPerformanceCounter(&os_internal_state.start_time);

    os_internal_state.platform_state = platform_state;
}


void os_console_write(const char* message, u8 level)
{
    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    // levels[6] -- FATAL,ERROR,WARN,INFO,DEBUG,TRACE
    static u8 levels[6] = {64, 4, 6, 2, 1, 8};
    SetConsoleTextAttribute(console_handle, levels[level]);
    OutputDebugStringA(message);
    u64 length = strlen(message);
    LPDWORD number_written = 0;
    WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), message, (DWORD)length, number_written, 0);
    SetConsoleTextAttribute(console_handle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
}


f64 os_get_absolute_time(void)
{
    LARGE_INTEGER now_time;
    QueryPerformanceCounter(&now_time);
    return (f64)now_time.QuadPart * os_internal_state.clock_frequency;
}

void os_wait(f64 ms)
{
    f64 start = os_get_absolute_time();
    f64 end = 0.0;

    do {
        end = os_get_absolute_time();
    } while ((end - start) < ( ms / 1000 ));
}

u64 os_rdtsc(void)
{
    return __rdtsc();
}

// os memory
STANDARD_RESULT os_memory_allocate(u64 size, u64 debug_base_address_in_TiB, void** out_allocation, u64* out_aligned_size)
{
    // retrieve system page size
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    u64 page_size = sys_info.dwPageSize;

    // align to page size, also enforces minimum size = page_size
    u64 aligned_size = ((size + page_size - 1) / page_size) * page_size;

#if STANDARD_DEBUG_BASE_ADDRESS_FOR_ALLOCATIONS == 1
    LPVOID base_address = (LPVOID)TiB(debug_base_address_in_TiB);
#else // STANDARD_DEBUG_BASE_ADDRESS_FOR_ALLOCATIONS == 1
    LPVOID base_address = NULL;
#endif // STANDARD_DEBUG_BASE_ADDRESS_FOR_ALLOCATIONS == 1

    // allocate
    *out_allocation = VirtualAlloc(base_address, aligned_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (*out_allocation == NULL) {
        return STANDARD_RESULT_OS_ALLOCATION_FAILURE;
    }

    // already zeroed but touch the memory anyways because overcommit even though it's supposedly not an issue on windows
    memset(*out_allocation, 0, aligned_size);

    *out_aligned_size = aligned_size;
    return STANDARD_RESULT_SUCCESS;
}

void os_memory_free(void* block, u64 length)
{
    (void)length;
    if (block != NULL) {
        VirtualFree(block, 0, MEM_RELEASE);
    }
}

u64 os_page_size_get(void)
{
    // retrieve system page size
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    return sys_info.dwPageSize;
}


// os windowing
struct os_thread_st {
    HANDLE handle;
    DWORD id;
};

struct os_window_st {
    HWND hwnd;
    os_thread_st thread;
    HANDLE initialized_event;

    // ImGuiContext* imgui_context;
    // GLuint imgui_opengl_vao;
};

LRESULT CALLBACK win32_process_message(HWND hwnd, u32 msg, WPARAM wParam, LPARAM lParam)
{
    platform_window_st* platform_window = (platform_window_st*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg) {
        case WM_CLOSE: {
            LOG_INFO("WM_CLOSE: %p\n", platform_window);
            platform_window->is_closing = true;
            return 0; // handled
        } break;

        default: {
            LOG_INFO("MSG: %lu\n", msg);
        } break;
    }
    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

u32 _threaded_window_init(platform_window_st* platform_window)
{
    // return 0 - OK
    // return 1 - ERROR
    u32 window_style = WS_OVERLAPPEDWINDOW;
    u32 window_ex_style = WS_EX_APPWINDOW;

    RECT rect = {0, 0, (u16)platform_window->current_width, (u16)platform_window->current_height};
    AdjustWindowRectEx(&rect, window_style, 0, window_ex_style);

    int window_width = rect.right - rect.left;
    int window_height = rect.bottom - rect.top;

    platform_window->os_window->hwnd = CreateWindowExA(
        WS_EX_APPWINDOW,
        "lostling_windowing_class",
        platform_window->title,
        window_style,
        CW_USEDEFAULT, CW_USEDEFAULT, window_width, window_height,
        NULL, NULL, os_internal_state.h_instance, NULL
    );
    if(platform_window->os_window->hwnd == 0) { return 1; }
    SetWindowLongPtrA(platform_window->os_window->hwnd, GWLP_USERDATA, (LONG_PTR)platform_window);
    ShowWindow(platform_window->os_window->hwnd, SW_SHOW);

    return 0;
}

DWORD WINAPI _window_thread_function(LPVOID lpParam)
{
    // return 0 - OK
    // return 1 - ERROR
    platform_window_st* window = (platform_window_st*)lpParam;
    u32 res = _threaded_window_init(window);
    if(res) { return res; }
    SetEvent(window->os_window->initialized_event);

    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    return 0;
}

STANDARD_RESULT os_windowing_init(void)
{
    os_internal_state.h_instance = GetModuleHandleA(0);
    os_internal_state.wndclass.lpfnWndProc = win32_process_message;
    os_internal_state.wndclass.cbClsExtra = 0;
    os_internal_state.wndclass.cbWndExtra = 0;
    os_internal_state.wndclass.hInstance = os_internal_state.h_instance;
    HICON icon = LoadIconA(os_internal_state.h_instance, IDI_WARNING);
    os_internal_state.wndclass.hIcon = icon;
    os_internal_state.wndclass.hCursor = LoadCursorA(NULL, IDC_ARROW); // Manage the cursor manually
    os_internal_state.wndclass.hbrBackground = NULL; // Transparent
    os_internal_state.wndclass.lpszClassName = "lostling_windowing_class";
    s32 res = RegisterClassA(&os_internal_state.wndclass);
    if (res == 0) { LOG_FATAL("Failed to register window class - os_windowing_system_init"); return STANDARD_RESULT_OS_WINDOWING_FAILURE; } // MSDN: If the function fails, the return value is zero

    return STANDARD_RESULT_SUCCESS;
}

STANDARD_RESULT os_window_create(platform_window_st* platform_window)
{
    platform_window->os_window = (os_window_st*)pool_allocate(sizeof(os_window_st), os_internal_state.platform_state->platform_pool, MEMORY_TAG_WINDOWING, true);

    platform_window->os_window->initialized_event = CreateEventA(NULL, TRUE, FALSE, NULL); // 0, ManualReset(true), InitialState(false/unsignaled), 0
    if(platform_window->os_window->initialized_event == 0) { return STANDARD_RESULT_OS_WINDOWING_FAILURE; };
    platform_window->os_window->thread.handle = CreateThread(NULL, 0, _window_thread_function, platform_window, 0, &platform_window->os_window->thread.id);
    if(platform_window->os_window->thread.handle == 0) { return STANDARD_RESULT_OS_WINDOWING_FAILURE; }
    WaitForSingleObject(platform_window->os_window->initialized_event, INFINITE);
    CloseHandle(platform_window->os_window->initialized_event);

    os_gl_window_setup(platform_window);

    return STANDARD_RESULT_SUCCESS;
}

void os_window_destroy(platform_window_st* platform_window)
{
    TerminateThread(platform_window->os_window->thread.handle, 0);
}


// os opengl
STANDARD_RESULT os_gl_init()
{
    WNDCLASSA window_class = {
        .style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
        .lpfnWndProc = DefWindowProcA,
        .hInstance = GetModuleHandle(0),
        .lpszClassName = "dummy",
    };

    if (!RegisterClassA(&window_class)) { return STANDARD_RESULT_OS_WINDOWING_FAILURE; }

    HWND dummy_window = CreateWindowExA(
        0,
        window_class.lpszClassName,
        "dummy",
        0,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        0,
        0,
        window_class.hInstance,
        0);

    if (!dummy_window) { return STANDARD_RESULT_OS_WINDOWING_FAILURE; }

    HDC dummy_dc = GetDC(dummy_window);

        PIXELFORMATDESCRIPTOR pfd = {
        .nSize = sizeof(pfd),
        .nVersion = 1,
        .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        .iPixelType = PFD_TYPE_RGBA,
        .cColorBits = 32,
        .cAlphaBits = 8,
        .cDepthBits = 24,
        .cStencilBits = 8,
        .iLayerType = PFD_MAIN_PLANE,
    };

    int pixel_format = ChoosePixelFormat(dummy_dc, &pfd);
    if (!pixel_format) { return STANDARD_RESULT_OS_WINDOWING_FAILURE; }
    if (!SetPixelFormat(dummy_dc, pixel_format, &pfd)) { return STANDARD_RESULT_OS_WINDOWING_FAILURE; }

    HGLRC dummy_context = wglCreateContext(dummy_dc);
    if (!dummy_context) { return STANDARD_RESULT_OS_WINDOWING_FAILURE; }
    if (!wglMakeCurrent(dummy_dc, dummy_context)) { return STANDARD_RESULT_OS_WINDOWING_FAILURE; }

    gl_loader_init();

    // cleanup
    wglMakeCurrent(dummy_dc, 0);
    wglDeleteContext(dummy_context);
    ReleaseDC(dummy_window, dummy_dc);
    DestroyWindow(dummy_window);
    return STANDARD_RESULT_SUCCESS;
}

void os_gl_render_context_create(platform_window_st* window)
{
    // this needs the DC to already have it's pixel format set up
    // all future windows/DCs need to share this same pixel format

    HDC gldc = GetDC(window->os_window->hwnd);

    // Specify that we want to create an OpenGL 3.3 core profile context
    int gl33_attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0,
    };

    os_internal_state.glrc = wglCreateContextAttribsARB(gldc, 0, gl33_attribs);
        if (!os_internal_state.glrc) {
            LOG_FATAL("Failed to create OpenGL 3.3 context.");
        }

        if (!wglMakeCurrent(gldc, os_internal_state.glrc)) {
            LOG_FATAL("Failed to activate OpenGL 3.3 rendering context.");
        }

    ReleaseDC(window->os_window->hwnd, gldc);
}

void os_gl_window_setup(platform_window_st* window)
{
    HDC hdc = GetDC(window->os_window->hwnd);

    // choose pixel format using wglChoosePixelFormatARB
    int pixel_format_attribs[] = {
        WGL_DRAW_TO_WINDOW_ARB,     GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB,     GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB,      GL_TRUE,
        WGL_ACCELERATION_ARB,       WGL_FULL_ACCELERATION_ARB,
        WGL_PIXEL_TYPE_ARB,         WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB,         32,
        WGL_DEPTH_BITS_ARB,         24,
        WGL_STENCIL_BITS_ARB,       8,
        0
    };

    s32 pixel_format;
    UINT num_formats;
    if (!wglChoosePixelFormatARB(hdc, pixel_format_attribs, 0, 1, &pixel_format, &num_formats)) {
        LOG_FATAL("wglChoosePixelFormatARB failed.");
    }
    if (num_formats == 0) {
        LOG_FATAL("Failed to choose a pixel format.");
    }

    PIXELFORMATDESCRIPTOR pfd;
    DescribePixelFormat(hdc, pixel_format, sizeof(pfd), &pfd);
    if (!SetPixelFormat(hdc, pixel_format, &pfd)) {
        LOG_FATAL("Failed to set the pixel format.2");
    }

    if (os_internal_state.glrc == NULL) { os_gl_render_context_create(window); }

    ReleaseDC(window->os_window->hwnd, hdc);
}

void os_gl_context_make_current(platform_window_st* window)
{
    if (!window) { return; }
    if (!window->os_window) { return; }
    HDC gldc = GetDC(window->os_window->hwnd);
    wglMakeCurrent(gldc, os_internal_state.glrc);
    ReleaseDC(window->os_window->hwnd, gldc);
}

void os_gl_clear(platform_window_st* window, f32 r, f32 g, f32 b)
{
    if (!window) { return; }
    if (!window->os_window) { return; }
    HDC gldc = GetDC(window->os_window->hwnd);
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ReleaseDC(window->os_window->hwnd, gldc);
}

void os_gl_swap_buffers(platform_window_st* window)
{
    if (!window) { return; }
    if (!window->os_window) { return; }
    HDC gldc = GetDC(window->os_window->hwnd);
    SwapBuffers(gldc);
    ReleaseDC(window->os_window->hwnd, gldc);
}




#endif // STANDARD_PLATFORM_WINDOWS == 1
