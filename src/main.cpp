// C stdlib
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// C++ stdlib
#include <map>
#include <string>
#include <vector>

// Third-party
#include <wayland-client.h>
#include <wlr-foreign-toplevel-management-unstable-v1.h>

// Types

struct ToplevelInfo
{
    zwlr_foreign_toplevel_handle_v1 *toplevel;
    std::string title;
    std::string app_id;
    bool done;
    bool activated;
};

struct State
{
    wl_seat *seat;

    zwlr_foreign_toplevel_manager_v1 *toplevel_manager;

    bool is_running;

    std::vector<ToplevelInfo> toplevel_infos;
    std::map<std::string, std::vector<size_t>> toplevel_infos_by_app_id;
};

// Forward declarations

void registry_global_handler(void *data, wl_registry *registry, uint32_t name, const char *interface, uint32_t version);
void registry_global_remove_handler(void *data, wl_registry *registry, uint32_t name);

void toplevel_manager_toplevel_handler(
    void *data,
    zwlr_foreign_toplevel_manager_v1 *zwlr_foreign_toplevel_manager_v1,
    zwlr_foreign_toplevel_handle_v1 *toplevel
);
void toplevel_manager_finished_handler(void *data, zwlr_foreign_toplevel_manager_v1 *zwlr_foreign_toplevel_manager_v1);

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
    struct wl_array *state_array
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

State state = {
    .is_running = true,
};

struct zwlr_foreign_toplevel_manager_v1_listener toplevel_manager_listener = {
    .toplevel = toplevel_manager_toplevel_handler,
    .finished = toplevel_manager_finished_handler,
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

    while (state.is_running)
    {
        wl_display_dispatch(display);

        if (!state.toplevel_infos.empty())
        {
            bool are_all_done = true;
            for (ToplevelInfo &info : state.toplevel_infos)
            {
                if (!info.done)
                {
                    are_all_done = false;
                }
            }

            if (are_all_done)
            {
                break;
            }
        }
    }

    ToplevelInfo *last_activated_info = nullptr;
    size_t activated_index            = 0;
    for (; activated_index < state.toplevel_infos.size(); ++activated_index)
    {
        ToplevelInfo *info = &state.toplevel_infos[activated_index];
        if (info->activated)
        {
            last_activated_info = info;
            break;
        }
    }

    if (last_activated_info != nullptr)
    {
        std::vector<size_t> &possible_indexes = state.toplevel_infos_by_app_id[last_activated_info->app_id];

        for (int i = possible_indexes.size() - 1; i >= 0; --i)
        {
            size_t index = possible_indexes[i];
            if (index != activated_index)
            {
                ToplevelInfo *info = &state.toplevel_infos[index];
                zwlr_foreign_toplevel_handle_v1_activate(info->toplevel, state.seat);
                wl_display_dispatch(display);
                break;
            }
        }
    }

    return 0;
}

// Private implementations

void registry_global_handler(void *data, wl_registry *registry, uint32_t name, const char *interface, uint32_t version)
{
    // printf("Interface: '%s', version: %u name: %u\n", interface, version, name);

    if (strcmp(interface, "wl_seat") == 0)
    {
        state.seat = (wl_seat *)wl_registry_bind(registry, name, &wl_seat_interface, 9);
    }
    else if (strcmp(interface, "zwlr_foreign_toplevel_manager_v1") == 0)
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
    printf("In registry_global_remove_handler - Removed: %u\n", name);
}

void toplevel_manager_toplevel_handler(
    void *data,
    zwlr_foreign_toplevel_manager_v1 *zwlr_foreign_toplevel_manager_v1,
    zwlr_foreign_toplevel_handle_v1 *toplevel
)
{
    state.toplevel_infos.push_back({
        .toplevel = toplevel,
    });
    zwlr_foreign_toplevel_handle_v1_add_listener(
        toplevel,
        &toplevel_handle_listener,
        (void *)(state.toplevel_infos.size() - 1)
    );
}

void toplevel_manager_finished_handler(void *data, zwlr_foreign_toplevel_manager_v1 *zwlr_foreign_toplevel_manager_v1)
{
    printf("In toplevel_manager_finished_handler - toplevel manager done\n");
}

void toplevel_handle_title_handler(
    void *data,
    zwlr_foreign_toplevel_handle_v1 *zwlr_foreign_toplevel_handle_v1,
    const char *title
)
{
    size_t info_index = (size_t)data;

    state.toplevel_infos[info_index].title = title;
}

void toplevel_handle_app_id_handler(
    void *data,
    zwlr_foreign_toplevel_handle_v1 *zwlr_foreign_toplevel_handle_v1,
    const char *app_id
)
{
    size_t info_index = (size_t)data;

    state.toplevel_infos[info_index].app_id = app_id;
    state.toplevel_infos_by_app_id[app_id].push_back(info_index);
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
    struct wl_array *state_array
)
{
    size_t info_index = (size_t)data;

    uint32_t *states    = (uint32_t *)state_array->data;
    size_t states_count = state_array->size / sizeof(uint32_t);

    for (size_t i = 0; i < states_count; ++i)
    {
        switch (states[i])
        {
        case ZWLR_FOREIGN_TOPLEVEL_HANDLE_V1_STATE_ACTIVATED:
            state.toplevel_infos[info_index].activated = true;
            break;
        }
    }
}

void toplevel_handle_done_handler(void *data, struct zwlr_foreign_toplevel_handle_v1 *zwlr_foreign_toplevel_handle_v1)
{
    size_t info_index                     = (size_t)data;
    state.toplevel_infos[info_index].done = true;
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
