#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <wayland-client.h>
#include <wlr-foreign-toplevel-management-unstable-v1.h>

// Types

typedef struct wl_registry wl_registry;
typedef struct wl_display wl_display;
typedef struct wl_registry_listener wl_registry_listener;

typedef struct zwlr_foreign_toplevel_manager_v1 zwlr_foreign_toplevel_manager_v1;
typedef struct zwlr_foreign_toplevel_handle_v1 zwlr_foreign_toplevel_handle_v1;

typedef struct
{
    zwlr_foreign_toplevel_handle_v1 *toplevel;
    const char *app_id;
} ToplevelInfo;

typedef struct
{
    zwlr_foreign_toplevel_manager_v1 *toplevel_manager;

    int toplevel_size;
    int toplevel_capacity;
    ToplevelInfo *toplevels;
} State;

// Forward declarations

void registry_global_handler(void *data, wl_registry *registry, uint32_t name, const char *interface, uint32_t version);
void registry_global_remove_handler(void *data, wl_registry *registry, uint32_t name);

void toplevel_manager_toplevel_handler(
    void *data,
    zwlr_foreign_toplevel_manager_v1 *zwlr_foreign_toplevel_manager_v1,
    zwlr_foreign_toplevel_handle_v1 *toplevel
);

void toplevel_handle_title_handler(
    void *data,
    zwlr_foreign_toplevel_handle_v1 *zwlr_foreign_toplevel_handle_v1,
    const char *title
);
void toplevel_handle_app_id_handler(
    void *data,
    zwlr_foreign_toplevel_handle_v1 *zwlr_foreign_toplevel_handle_v1,
    const char *app_id
);
void toplevel_handle_output_enter_handler(
    void *data,
    struct zwlr_foreign_toplevel_handle_v1 *zwlr_foreign_toplevel_handle_v1,
    struct wl_output *output
);
void toplevel_handle_output_leave_handler(
    void *data,
    struct zwlr_foreign_toplevel_handle_v1 *zwlr_foreign_toplevel_handle_v1,
    struct wl_output *output
);
void toplevel_handle_state_handler(
    void *data,
    struct zwlr_foreign_toplevel_handle_v1 *zwlr_foreign_toplevel_handle_v1,
    struct wl_array *state
);
void toplevel_handle_done_handler(void *data, struct zwlr_foreign_toplevel_handle_v1 *zwlr_foreign_toplevel_handle_v1);
void
toplevel_handle_closed_handler(void *data, struct zwlr_foreign_toplevel_handle_v1 *zwlr_foreign_toplevel_handle_v1);
void toplevel_handle_parent_handler(
    void *data,
    struct zwlr_foreign_toplevel_handle_v1 *zwlr_foreign_toplevel_handle_v1,
    struct zwlr_foreign_toplevel_handle_v1 *parent
);

// Globals

State state = {};

struct zwlr_foreign_toplevel_manager_v1_listener toplevel_manager_listener = {
    .toplevel = toplevel_manager_toplevel_handler,
};

struct zwlr_foreign_toplevel_handle_v1_listener toplevel_handle_listener = {
    .title        = toplevel_handle_title_handler,
    .app_id       = toplevel_handle_app_id_handler,
    .output_enter = toplevel_handle_output_enter_handler,
    .output_leave = toplevel_handle_output_leave_handler,
    .state        = toplevel_handle_state_handler,
    .done         = toplevel_handle_done_handler,
    .closed       = toplevel_handle_closed_handler,
    .parent       = toplevel_handle_parent_handler,
};

// Public implementations

int main()
{
    wl_display *display                    = wl_display_connect(nullptr);
    wl_registry *registry                  = wl_display_get_registry(display);
    wl_registry_listener registry_listener = {
        .global        = registry_global_handler,
        .global_remove = registry_global_remove_handler,
    };

    wl_registry_add_listener(registry, &registry_listener, nullptr);

    while (true)
    {
        wl_display_dispatch(display);
    }

    free(state.toplevels);

    // zwlr_foreign_toplevel_manager_v1_destroy(state.toplevel_manager);

    return 0;
}

// Private implementations

void registry_global_handler(void *data, wl_registry *registry, uint32_t name, const char *interface, uint32_t version)
{
    // printf("Interface: '%s', version: %u name: %u\n", interface, version, name);

    if (strcmp(interface, "zwlr_foreign_toplevel_manager_v1") == 0)
    {
        // clang-format off
        state.toplevel_manager = (zwlr_foreign_toplevel_manager_v1 *)wl_registry_bind(registry, name, &zwlr_foreign_toplevel_manager_v1_interface, 3);

        zwlr_foreign_toplevel_manager_v1_add_listener(
            state.toplevel_manager,
            &toplevel_manager_listener,
            nullptr
        );
        // clang-format on
    }
}

void registry_global_remove_handler(void *data, wl_registry *registry, uint32_t name)
{
    printf("Removed: %u\n", name);
}

void toplevel_manager_toplevel_handler(
    void *data,
    zwlr_foreign_toplevel_manager_v1 *zwlr_foreign_toplevel_manager_v1,
    zwlr_foreign_toplevel_handle_v1 *toplevel
)
{
    zwlr_foreign_toplevel_handle_v1_add_listener(toplevel, &toplevel_handle_listener, nullptr);

    if (state.toplevel_size >= state.toplevel_capacity)
    {
        state.toplevel_capacity = state.toplevel_capacity == 0 ? 1 : state.toplevel_capacity * 2;
        state.toplevels         = (ToplevelInfo *)realloc(state.toplevels, state.toplevel_capacity);
    }

    state.toplevels[state.toplevel_size] = (ToplevelInfo){
        .toplevel = toplevel,
    };
}

void toplevel_handle_title_handler(
    void *data,
    zwlr_foreign_toplevel_handle_v1 *zwlr_foreign_toplevel_handle_v1,
    const char *title
)
{
    printf("title: %s\n", title);
}

void toplevel_handle_app_id_handler(
    void *data,
    zwlr_foreign_toplevel_handle_v1 *zwlr_foreign_toplevel_handle_v1,
    const char *app_id
)
{
    printf("app_id: %s\n", app_id);
}

void toplevel_handle_output_enter_handler(
    void *data,
    struct zwlr_foreign_toplevel_handle_v1 *zwlr_foreign_toplevel_handle_v1,
    struct wl_output *output
)
{
}

void toplevel_handle_output_leave_handler(
    void *data,
    struct zwlr_foreign_toplevel_handle_v1 *zwlr_foreign_toplevel_handle_v1,
    struct wl_output *output
)
{
}

void toplevel_handle_state_handler(
    void *data,
    struct zwlr_foreign_toplevel_handle_v1 *zwlr_foreign_toplevel_handle_v1,
    struct wl_array *state
)
{
}

void toplevel_handle_done_handler(void *data, struct zwlr_foreign_toplevel_handle_v1 *zwlr_foreign_toplevel_handle_v1)
{
}

void toplevel_handle_closed_handler(void *data, struct zwlr_foreign_toplevel_handle_v1 *zwlr_foreign_toplevel_handle_v1)
{
}

void toplevel_handle_parent_handler(
    void *data,
    struct zwlr_foreign_toplevel_handle_v1 *zwlr_foreign_toplevel_handle_v1,
    struct zwlr_foreign_toplevel_handle_v1 *parent
)
{
}
