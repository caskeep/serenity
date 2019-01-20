#include "GWindow.h"
#include "GEvent.h"
#include "GEventLoop.h"
#include <SharedGraphics/GraphicsBitmap.h>
#include <LibC/gui.h>
#include <LibC/stdio.h>
#include <LibC/stdlib.h>
#include <AK/HashMap.h>

static HashMap<int, GWindow*>* s_windows;

static HashMap<int, GWindow*>& windows()
{
    if (!s_windows)
        s_windows = new HashMap<int, GWindow*>;
    return *s_windows;
}

GWindow* GWindow::from_window_id(int window_id)
{
    auto it = windows().find(window_id);
    if (it != windows().end())
        return (*it).value;
    return nullptr;
}

GWindow::GWindow(GObject* parent)
    : GObject(parent)
{
    GUI_WindowParameters wparams;
    wparams.rect = { { 100, 400 }, { 140, 140 } };
    wparams.background_color = 0xffc0c0;
    strcpy(wparams.title, "GWindow");
    m_window_id = gui_create_window(&wparams);
    if (m_window_id < 0) {
        perror("gui_create_window");
        exit(1);
    }

    GUI_WindowBackingStoreInfo backing;
    int rc = gui_get_window_backing_store(m_window_id, &backing);
    if (rc < 0) {
        perror("gui_get_window_backing_store");
        exit(1);
    }

    m_backing = GraphicsBitmap::create_wrapper(backing.size, backing.pixels);

    windows().set(m_window_id, this);
}

GWindow::~GWindow()
{
}

void GWindow::set_title(String&& title)
{
    dbgprintf("GWindow::set_title \"%s\"\n", title.characters());
    GUI_WindowParameters params;
    int rc = gui_get_window_parameters(m_window_id, &params);
    ASSERT(rc == 0);
    strcpy(params.title, title.characters());;
    rc = gui_set_window_parameters(m_window_id, &params);
    ASSERT(rc == 0);
    m_title = move(title);
}

void GWindow::set_rect(const Rect& rect)
{
    dbgprintf("GWindow::set_rect %d,%d %dx%d\n", m_rect.x(), m_rect.y(), m_rect.width(), m_rect.height());
    GUI_WindowParameters params;
    int rc = gui_get_window_parameters(m_window_id, &params);
    ASSERT(rc == 0);
    params.rect = rect;
    rc = gui_set_window_parameters(m_window_id, &params);
    ASSERT(rc == 0);
    m_rect = rect;
}

void GWindow::event(GEvent& event)
{
}

bool GWindow::is_visible() const
{
    return false;
}

void GWindow::close()
{
}

void GWindow::show()
{
}

void GWindow::update()
{
    gui_invalidate_window(m_window_id, nullptr);
}

void GWindow::set_main_widget(GWidget* widget)
{
    if (m_main_widget == widget)
        return;
    m_main_widget = widget;
    update();
}