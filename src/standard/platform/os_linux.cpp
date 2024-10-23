#if STANDARD_PLATFORM_LINUX == 1

#include "os.h"

#include <unistd.h> // posix
#include <time.h> // time
#include <sys/mman.h> // mmap
#include <xcb/xcb.h> // xcb
#include <xcb/xcb_util.h> // sudo apt-get install libxcb-util-dev
#include <xcb/xcb_icccm.h> // sudo apt-get install libxcb-icccm4-dev
#include <xcb/xcb_keysyms.h> // sudo apt-get install libxcb-keysyms1-dev
#include <GL/gl.h> // gl
#include <GL/glx.h> // gl

#include <stdio.h>
#include <string.h> // memset
#include <stdlib.h> // free

#include "platform.h"
#include "log.h"


struct os_internal_state_st {
    platform_state_st* platform_state;
    // core
    f64 clock_frequency;
    struct timespec start_time;
    // windowing
    xcb_connection_t* xcb_connection;
    s32 xcb_screen_number;
    xcb_screen_t* xcb_screen;
    Display* display;
    // opengl
    // HGLRC glrc;
    GLXContext glx_context;
};
os_internal_state_st os_internal_state;


void os_init(platform_state_st* platform_state)
{
    // On Linux, clock resolution is fixed and can be obtained with clock_getres, but for
    // CLOCK_MONOTONIC it's usually unnecessary as it provides nanosecond precision.
    os_internal_state.clock_frequency = 1.0e9; // CLOCK_MONOTONIC provides time in nanoseconds.

    // Get the current time
    if (clock_gettime(CLOCK_MONOTONIC, &os_internal_state.start_time) == -1)
    {
        // Handle error
        perror("clock_gettime");
    }
    os_internal_state.platform_state = platform_state;
}


void os_console_write(const char* message, u8 level) {
    //                              FATAL,  ERROR,  WARN,   INFO,   DEBUG,  TRACE
    const char* level_strings[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    printf("\033[%sm%s\033[0m", level_strings[level], message);
}

f64 os_get_absolute_time(void)
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec + now.tv_nsec * 0.000000001;
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


STANDARD_RESULT os_memory_allocate(u64 size, u64 debug_base_address_in_TiB, void** out_allocation, u64* out_aligned_size)
{
    s32 page_size;
    page_size = sysconf(_SC_PAGESIZE);

    if (page_size == -1) {
        perror("sysconf");
        return STANDARD_RESULT_OS_ALLOCATION_FAILURE;
    }

    // align to page size, also enforces minimum size = page_size
    u64 aligned_size = ((size + page_size - 1) / page_size) * page_size;

#if STANDARD_DEBUG_BASE_ADDRESS_FOR_ALLOCATIONS == 1
    void* BaseAddress = (void*)TiB(debug_base_address_in_TiB);
#else // STANDARD_DEBUG_BASE_ADDRESS_FOR_ALLOCATIONS == 1
    void* BaseAddress = NULL;
#endif // STANDARD_DEBUG_BASE_ADDRESS_FOR_ALLOCATIONS == 1

    // allocate
    *out_allocation = mmap(BaseAddress, aligned_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (*out_allocation == MAP_FAILED) {
        return STANDARD_RESULT_OS_ALLOCATION_FAILURE;
    }

    // already zeroed but touch the memory anyways because overcommit even though it's supposedly not an issue on windows
    memset(*out_allocation, 0, aligned_size);

    *out_aligned_size = aligned_size;
    return STANDARD_RESULT_SUCCESS;
}

void os_memory_free(void* block, u64 length)
{
    if (block != NULL) {
        munmap(block, length);
    }
}

u64 os_page_size_get(void)
{
    return (s32)sysconf(_SC_PAGESIZE);
}


// os windowing
struct os_thread_st {
    // HANDLE handle;
    // DWORD id;
};

struct os_window_st {
    // HWND hwnd;
    // os_thread_st thread;
    // HANDLE initialized_event;
    xcb_window_t xcb_window;
    xcb_atom_t wm_delete_win;
    xcb_atom_t wm_protocols;
    pthread_t thread;
    xcb_connection_t* local_xcb_connection;
    xcb_screen_t* local_xcb_screen;

    // ImGuiContext* imgui_context;
    // GLuint imgui_opengl_vao;
};


void* _window_message_loop_threaded(void* window)
{
    platform_window_st* window_struct = (platform_window_st*)window;

    xcb_generic_event_t* event;
    xcb_client_message_event_t* cm;
    LOG_TRACE("UNUSED VARIABLE\n", event = 0, cm = 0);
    xcb_key_symbols_t* keysyms = xcb_key_symbols_alloc(window_struct->os_window->local_xcb_connection);
    while(true)
    {
        xcb_generic_event_t* event = xcb_poll_for_event(window_struct->os_window->local_xcb_connection);
        if (event == nullptr) {
            continue; // No event to process, continue the loop
        }
        switch(XCB_EVENT_RESPONSE_TYPE(event))
        {
            case XCB_EXPOSE:
            {
                xcb_expose_event_t* expose_event = (xcb_expose_event_t*)event;
                LOG_INFO("Expose Event: %i %i %i %i\n", expose_event->x, expose_event->y, expose_event->width, expose_event->height);
            } break;
            // case XCB_KEY_PRESS:
            // case XCB_KEY_RELEASE: {
            //     // Key press event - xcb_key_press_event_t and xcb_key_release_event_t are the same
            //     xcb_key_press_event_t* kb_event = (xcb_key_press_event_t*)event;
            //     bool pressed = event->response_type == XCB_KEY_PRESS;
            //     xcb_keycode_t code = kb_event->detail;

            //     xcb_keysym_t key_sym = xcb_key_press_lookup_keysym(keysyms, kb_event, 0);

            //     keys key = translate_keycode(key_sym);

            //     // Pass to the input subsystem for processing.
            //     input_process_key(key, pressed);
            // } break;
            // case XCB_BUTTON_PRESS:
            // case XCB_BUTTON_RELEASE: {
            //     bool pressed = event->response_type == XCB_BUTTON_PRESS;
            //     buttons mouse_button = STANDARD_MOUSE_BUTTON_MAX_BUTTONS;

            //     switch (((xcb_button_press_event_t *)event)->detail) {
            //         case 1: mouse_button = STANDARD_MOUSE_BUTTON_LEFT; break;
            //         case 2: mouse_button = STANDARD_MOUSE_BUTTON_MIDDLE; break;
            //         case 3: mouse_button = STANDARD_MOUSE_BUTTON_RIGHT; break;
            //         case 4: input_process_mouse_wheel(-1); break;  // Scroll Up
            //         case 5: input_process_mouse_wheel(1); break; // Scroll Down
            //     }
            //     if (mouse_button != STANDARD_MOUSE_BUTTON_MAX_BUTTONS) {
            //         input_process_button(mouse_button, pressed);
            //     }
            // } break;
            // case XCB_MOTION_NOTIFY:
            // {
            //     xcb_motion_notify_event_t* motion = (xcb_motion_notify_event_t*) event;
            //     // LOG_INFO("Mouse Movement: (%d, %d)", motion->event_x, motion->event_y);
            //     input_process_mouse_move(motion->event_x, motion->event_y);
            // } break;
            // case XCB_CONFIGURE_NOTIFY:
            // {
            //     // TODO: Resizing
            //     xcb_configure_notify_event_t *cn_event = (xcb_configure_notify_event_t *)event;
            //     LOG_ERROR("Window resized to %dx%d\n", cn_event->width, cn_event->height);
            //     window_struct->width = cn_event->width;
            //     window_struct->height = cn_event->height;
            //     window_struct->resized = true;
            // } break;
            // case XCB_CLIENT_MESSAGE:
            // {
            //     cm = (xcb_client_message_event_t*)event;

            //     // Window close
            //     if (cm->data.data32[0] == window_struct->wm_delete_win) {
            //         LOG_INFO("message loop close window: %s", window_struct->title);
            //         window_struct->closing = true;

            //         return nullptr;
            //     }
            // } break;
            // case XCB_FOCUS_IN:
            // {
            //     LOG_DEBUG("WIN FOCUS IN");
            //     window_struct->active = true;
            // } break;
            // case XCB_FOCUS_OUT:
            // {
            //     LOG_DEBUG("WIN FOCUS OUT");
            //     window_struct->active = false;
            // } break;
            default:
                // Something else
                break;
        }

        free(event);
    }
    xcb_key_symbols_free(keysyms);

    return nullptr;
}

STANDARD_RESULT os_windowing_init(void)
{
    os_internal_state.xcb_connection = xcb_connect(nullptr, &os_internal_state.xcb_screen_number);
    STANDARD_ASSERT(!xcb_connection_has_error(os_internal_state.xcb_connection));
    os_internal_state.xcb_screen = xcb_aux_get_screen(os_internal_state.xcb_connection, os_internal_state.xcb_screen_number);

    os_internal_state.display = XOpenDisplay(NULL);
    if (!os_internal_state.display) {
        LOG_ERROR("Failed to open display for GLX.\n");
        return STANDARD_RESULT_OS_WINDOWING_FAILURE;
    }

    return STANDARD_RESULT_SUCCESS;
}

STANDARD_RESULT os_window_create(struct platform_window_st* platform_window)
{
    platform_window->os_window = (os_window_st*)pool_allocate(sizeof(os_window_st), os_internal_state.platform_state->platform_pool, MEMORY_TAG_WINDOWING, true);


    platform_window->os_window->local_xcb_connection = xcb_connect(nullptr, &os_internal_state.xcb_screen_number);
    STANDARD_ASSERT(!xcb_connection_has_error(platform_window->os_window->local_xcb_connection));
    platform_window->os_window->local_xcb_screen = xcb_aux_get_screen(platform_window->os_window->local_xcb_connection, os_internal_state.xcb_screen_number);

    // Generate ID for the window
    platform_window->os_window->xcb_window = xcb_generate_id(platform_window->os_window->local_xcb_connection);

    // Event mask and values
    u32 event_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    u32 event_values = XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_FOCUS_CHANGE;
    u32 value_list[] = {platform_window->os_window->local_xcb_screen->black_pixel, event_values};

    // Create the window
    xcb_create_window(platform_window->os_window->local_xcb_connection, XCB_COPY_FROM_PARENT, platform_window->os_window->xcb_window, platform_window->os_window->local_xcb_screen->root, 0, 0, platform_window->current_width, platform_window->current_height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, platform_window->os_window->local_xcb_screen->root_visual, event_mask, value_list);

    // Set the title
    xcb_change_property(platform_window->os_window->local_xcb_connection, XCB_PROP_MODE_REPLACE, platform_window->os_window->xcb_window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, strlen(platform_window->title), platform_window->title);

    // Handle WM_DELETE_WINDOW
    xcb_intern_atom_cookie_t wm_delete_cookie = xcb_intern_atom(platform_window->os_window->local_xcb_connection, 0, strlen("WM_DELETE_WINDOW"), "WM_DELETE_WINDOW");
    xcb_intern_atom_cookie_t wm_protocols_cookie = xcb_intern_atom(platform_window->os_window->local_xcb_connection, 0, strlen("WM_PROTOCOLS"), "WM_PROTOCOLS");
    xcb_intern_atom_reply_t* wm_delete_reply = xcb_intern_atom_reply(platform_window->os_window->local_xcb_connection, wm_delete_cookie, NULL);
    xcb_intern_atom_reply_t* wm_protocols_reply = xcb_intern_atom_reply(platform_window->os_window->local_xcb_connection, wm_protocols_cookie, NULL);
    platform_window->os_window->wm_delete_win = wm_delete_reply->atom;
    platform_window->os_window->wm_protocols = wm_protocols_reply->atom;

    xcb_change_property(platform_window->os_window->local_xcb_connection, XCB_PROP_MODE_REPLACE, platform_window->os_window->xcb_window, wm_protocols_reply->atom, 4, 32, 1, &wm_delete_reply->atom);

    // Free the replies
    free(wm_delete_reply);
    free(wm_protocols_reply);

    // Map the window
    xcb_map_window(platform_window->os_window->local_xcb_connection, platform_window->os_window->xcb_window);

    // Flush the stream
    s32 stream_result = xcb_flush(platform_window->os_window->local_xcb_connection);
    if (stream_result <= 0) {
        LOG_ERROR("An error occurred when flusing the stream: %d", stream_result);
        return STANDARD_RESULT_OS_WINDOWING_FAILURE;
    }

    pthread_create(&platform_window->os_window->thread, nullptr, _window_message_loop_threaded, platform_window);

    // darray_push(os_internal_state.windows, platform_window);

    os_gl_window_setup(platform_window);

    return STANDARD_RESULT_SUCCESS;
}

void os_window_destroy(struct platform_window_st* platform_window)
{
    pthread_cancel(platform_window->os_window->thread);
}


STANDARD_RESULT os_gl_init()
{
    // Choose visual info for GLX
    int visual_attribs[] = {
        GLX_RGBA,
        GLX_DEPTH_SIZE, 24,
        GLX_DOUBLEBUFFER,
        None
    };

    XVisualInfo *vi = glXChooseVisual(os_internal_state.display, 0, visual_attribs);
    if (vi == NULL) {
        printf("No appropriate visual found.\n");
        return STANDARD_RESULT_OS_WINDOWING_FAILURE;
    }

    // Create GLX context
    os_internal_state.glx_context = glXCreateContext(os_internal_state.display, vi, NULL, GL_TRUE);
    if (os_internal_state.glx_context == NULL) {
        printf("Failed to create GLX context.\n");
        return STANDARD_RESULT_OS_WINDOWING_FAILURE;
    }

    return STANDARD_RESULT_SUCCESS;
}

void os_gl_window_setup(struct platform_window_st* window)
{
    LOG_TRACE("UNUSED PARAM os_gl_window_setup\n", window);
}

void os_gl_context_make_current(struct platform_window_st* window)
{
    glXMakeCurrent(os_internal_state.display, window->os_window->xcb_window, os_internal_state.glx_context);
}

void os_gl_clear(struct platform_window_st* window, f32 r, f32 g, f32 b)
{
    (void)window;
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void os_gl_swap_buffers(struct platform_window_st* window)
{
    glXSwapBuffers(os_internal_state.display, window->os_window->xcb_window);
}
















#endif // STANDARD_PLATFORM_LINUX == 1
