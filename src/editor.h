#pragma once

struct Editor {
    //will have pointers to systems
    bool active;

    void update();

    static void activate_editor(Editor *editor);
};
