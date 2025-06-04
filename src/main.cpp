#include <stdio.h>
#include <string.h>

#include <ext-foreign-toplevel-list-v1.h>
#include <wayland-client.h>

void registry_global_handler(void *data, wl_registry *registry, uint32_t name, const char *interface, uint32_t version);
void registry_global_remove_handler(void *data, wl_registry *registry, uint32_t name);

void toplevel_list_toplevel_handler(
    void *data,
    struct ext_foreign_toplevel_list_v1 *ext_foreign_toplevel_list_v1,
    struct ext_foreign_toplevel_handle_v1 *toplevel
);

struct State
{
    ext_foreign_toplevel_list_v1 *toplevel_list;

    int toplevel_count;
    ext_foreign_toplevel_handle_v1 **toplevels;
};

State state = {};

int main()
{
    printf("Hello, world!\n");

    wl_display *display                    = wl_display_connect(nullptr);
    wl_registry *registry                  = wl_display_get_registry(display);
    wl_registry_listener registry_listener = {
        .global        = registry_global_handler,
        .global_remove = registry_global_remove_handler,
    };

    wl_registry_add_listener(registry, &registry_listener, nullptr);

    for (int i = 0; i < 10; ++i)
    {
        wl_display_dispatch(display);

        if (state.toplevel_list != nullptr)
        {
            break;
        }
    }

    ext_foreign_toplevel_list_v1_destroy(state.toplevel_list);

    return 0;
}

void registry_global_handler(void *data, wl_registry *registry, uint32_t name, const char *interface, uint32_t version)
{
    printf("Interface: '%s', version: %u name: %u\n", interface, version, name);

    if (strcmp(interface, "ext_foreign_toplevel_list_v1") == 0)
    {
        // clang-format off
        state.toplevel_list = (ext_foreign_toplevel_list_v1 *)wl_registry_bind(registry, name, &ext_foreign_toplevel_list_v1_interface, 1);
        // clang-format on

        ext_foreign_toplevel_list_v1_add_listener(state.toplevel_list, nullptr, nullptr);
    }
}

void registry_global_remove_handler(void *data, wl_registry *registry, uint32_t name)
{
    printf("Removed: %u\n", name);
}

void toplevel_list_toplevel_handler(
    void *data,
    struct ext_foreign_toplevel_list_v1 *ext_foreign_toplevel_list_v1,
    struct ext_foreign_toplevel_handle_v1 *toplevel
)
{
}
