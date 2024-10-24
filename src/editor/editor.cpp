#include "platform/os.h"
#include "platform/platform.h"
#include "platform/log.h"


void editor_run(void);

platform_state_st* platform_state;
platform_window_st* win1;
platform_window_st* win2;
platform_window_st* win3;

int main(void)
{
    STANDARD_RESULT res;

    res = platform_init(&platform_state);
    if (res) { return -1; }

    os_init(platform_state);
    if (res) { return -1; }

    res = platform_windowing_init();
    if (res) { return -1; }

    os_gl_init();

    res = platform_window_create(800, 600, &win1);
    if (res) { return STANDARD_RESULT_OS_WINDOWING_FAILURE; }
    os_gl_window_setup(win1);

    res = platform_window_create(800, 600, &win2);
    if (res) { return STANDARD_RESULT_OS_WINDOWING_FAILURE; }
    os_gl_window_setup(win2);

    res = platform_window_create(800, 600, &win3);
    if (res) { return STANDARD_RESULT_OS_WINDOWING_FAILURE; }
    os_gl_window_setup(win3);

    platform_state->is_running = true;
    editor_run();

    return 0;
}

void editor_run(void)
{
static s32 test = 0;

    const f64 UPDATE_TPS = 60.0;
    const f64 UPDATE_DELTA = (1.0 / UPDATE_TPS);
    f64 accumulator = UPDATE_DELTA;

    const f64 FRAME_TIME_MAX = 0.25f;

    f64 last_time = os_get_absolute_time();

    s64 FRAME_CYCLES_LastCycleCount = os_rdtsc();
    s64 FRAME_CYCLES_EndCycleCount = 0;
    u64 FRAME_CYCLES_CyclesElapsed = 0u;
    LOG_TRACE("UNUSED PARAM editor_run\n", FRAME_CYCLES_CyclesElapsed);

    while(platform_state->is_running)
    {
        f64 current_time = os_get_absolute_time();
        f64 delta_time = current_time - last_time;
        last_time = current_time;

        if (delta_time > FRAME_TIME_MAX) { delta_time = FRAME_TIME_MAX; }

        accumulator += delta_time;
        f64 alpha_time = accumulator / UPDATE_DELTA;
        (void)alpha_time; // UNUSED VARIABLE
        while (accumulator >= UPDATE_DELTA)
        {
            // update();
            os_wait(1);
            accumulator -= UPDATE_DELTA;
        }
        platform_windowing_pre_render();
        // render();
        os_wait(10);

        // opengl testing
        if (test == 0)
        {
            test = 1;
            os_gl_context_make_current(win1);
            os_gl_clear(win1, 0.0f, 0.0f, 1.0f);
            os_gl_swap_buffers(win1);

            os_gl_context_make_current(win2);
            os_gl_clear(win2, 1.0f, 0.0f, 1.0f);
            os_gl_swap_buffers(win2);

            os_gl_context_make_current(win3);
            os_gl_clear(win3, 0.0f, 0.0f, 1.0f);
            os_gl_swap_buffers(win3);
        }
        else if (test == 1)
        {
            test = 0;
            os_gl_context_make_current(win1);
            os_gl_clear(win1, 1.0f, 0.0f, 1.0f);
            os_gl_swap_buffers(win1);

            os_gl_context_make_current(win2);
            os_gl_clear(win2, 0.0f, 0.0f, 1.0f);
            os_gl_swap_buffers(win2);

            os_gl_context_make_current(win3);
            os_gl_clear(win3, 1.0f, 0.0f, 0.0f);
            os_gl_swap_buffers(win3);
        }

        // platform_ui_new_frame(&win1->ui_context);
        // platform_ui_window_begin(&win1->ui_context, "window1");
        // static b32 button_state = 0;
        // button_state = platform_ui_widget_button(&win1->ui_context);
        // platform_ui_window_end(&win1->ui_context);
        // platform_ui_render_frame(&win1->ui_context);

        platform_ui_next_window_initial_size(&win1->ui_context, 540, 320);
        platform_ui_window_begin(&win1->ui_context, "window1");
        platform_ui_window_begin(&win2->ui_context, "window2");
        platform_ui_window_begin(&win3->ui_context, "window3");
        platform_ui_window_begin(&win1->ui_context, "window1");

        // LOG_DEBUG("WINDOW COUNT: %llu\n", darray_length_get(platform->windows_darray));
        FRAME_CYCLES_EndCycleCount = os_rdtsc();
        FRAME_CYCLES_CyclesElapsed = FRAME_CYCLES_EndCycleCount - FRAME_CYCLES_LastCycleCount;
        FRAME_CYCLES_LastCycleCount = FRAME_CYCLES_EndCycleCount;
        // LOG_INFO("FRAME DELTA %fms - FPS %f - MC %f\n", deltaTime * 1000, 1 / deltaTime, (f64)FRAME_CYCLES_CyclesElapsed / (1000.0f * 1000.0f));
        // LOG_INFO("FRAME DELTA %fms - FPS %f\n", deltaTime * 1000, 1 / deltaTime);
    }
}
