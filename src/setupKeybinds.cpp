#include <Geode/Geode.hpp>

#if (defined(GEODE_IS_WINDOWS))
#include <geode.custom-keybinds/include/Keybinds.hpp>

using namespace keybinds;

$execute {
    BindManager::get()->registerBindable({
        // ID, should be prefixed with mod ID
        "clone-and-move-obj-up"_spr,
        // Name
        "Clone And Move Object Up",
        // Description, leave empty for none
        "Clone and move selected object(s) up based on grid size or step size",
        // Default binds
        { },
        // Category; use slashes for specifying subcategories. See the
        // Category class for default categories
        "Editor/Clone and Move"
    });

	BindManager::get()->registerBindable({
        // ID, should be prefixed with mod ID
        "clone-and-move-obj-down"_spr,
        // Name
        "Clone And Move Object Down",
        // Description, leave empty for none
        "Clone and move selected object(s) down based on grid size or step size",
        // Default binds
        { },
        // Category; use slashes for specifying subcategories. See the
        // Category class for default categories
        "Editor/Clone and Move"
    });

	BindManager::get()->registerBindable({
        // ID, should be prefixed with mod ID
        "clone-and-move-obj-left"_spr,
        // Name
        "Clone And Move Object Left",
        // Description, leave empty for none
        "Clone and move selected object(s) left based on grid size or step size",
        // Default binds
        { },
        // Category; use slashes for specifying subcategories. See the
        // Category class for default categories
        "Editor/Clone and Move"
    });

	BindManager::get()->registerBindable({
        // ID, should be prefixed with mod ID
        "clone-and-move-obj-right"_spr,
        // Name
        "Clone And Move Object Right",
        // Description, leave empty for none
        "Clone and move selected object(s) right based on grid size or step size",
        // Default binds
        { },
        // Category; use slashes for specifying subcategories. See the
        // Category class for default categories
        "Editor/Clone and Move"
    });
}

#endif