/*
 *  Gwork
 *  Copyright (c) 2012 Facepunch Studios
 *  Copyright (c) 2013-2016 Billy Quith
 *  See license in Gwork.h
 */

//#include <Gwork/Macros.h>
#include <Gwork/Platform.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>

#include <Gwork/Input/Allegro5.h>


static Gwk::Input::Allegro     g_GworkInput;
static ALLEGRO_EVENT_QUEUE*     g_event_queue = NULL;
static ALLEGRO_DISPLAY*         g_display = NULL;
static Gwk::String             gs_ClipboardEmulator;

static const ALLEGRO_SYSTEM_MOUSE_CURSOR g_CursorConversion[] =
{
    ALLEGRO_SYSTEM_MOUSE_CURSOR_ARROW,      // IDC_ARROW
    ALLEGRO_SYSTEM_MOUSE_CURSOR_EDIT,       // IDC_IBEAM
    ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_N,   // IDC_SIZENS
    ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_E,   // IDC_SIZEWE
    ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_N,   // IDC_SIZENWSE
    ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_E,   // IDC_SIZENESW
    ALLEGRO_SYSTEM_MOUSE_CURSOR_MOVE,       // IDC_SIZEALL
    ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE, // IDC_NO
    ALLEGRO_SYSTEM_MOUSE_CURSOR_BUSY,       // IDC_WAIT
    ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK        // IDC_HAND
};


void Gwk::Platform::Sleep(unsigned int iMS)
{
    al_rest(iMS*0.001);
}

void Gwk::Platform::SetCursor(unsigned char iCursor)
{
    al_set_system_mouse_cursor(g_display, g_CursorConversion[iCursor]);
}

Gwk::String Gwk::Platform::GetClipboardText()
{
    return gs_ClipboardEmulator;
}

bool Gwk::Platform::SetClipboardText(const Gwk::String& str)
{
    gs_ClipboardEmulator = str;
    return true;
}

float Gwk::Platform::GetTimeInSeconds()
{
    return al_get_time();
}

bool Gwk::Platform::FileOpen(const String& Name,
                             const String& StartPath,
                             const String& Extension,
                             String& filePathOut)
{
    ALLEGRO_FILECHOOSER* chooser =
        al_create_native_file_dialog(StartPath.c_str(),
                                     Name.c_str(),
                                     "*.*",  //
                                     // Extension.c_str(),
                                     ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);

    if (al_show_native_file_dialog(g_display, chooser))
    {
        if (al_get_native_file_dialog_count(chooser) != 0)
        {
            if (pHandler && fnCallback)
            {
                Gwk::Event::Information info;
                info.Control        = NULL;
                info.ControlCaller  = NULL;
                info.String         = al_get_native_file_dialog_path(chooser, 0);
                (pHandler->*fnCallback)(info);
            }
        }
    }

    al_destroy_native_file_dialog(chooser);
    return true;
}

bool Gwk::Platform::FileSave(const String& Name,
                             const String& StartPath,
                             String& filePathOut);
{
    ALLEGRO_FILECHOOSER* chooser = al_create_native_file_dialog(StartPath.c_str(),
                                                                Name.c_str(),
                                                                "*.*",  //
                                                                        // Extension.c_str(),
                                                                ALLEGRO_FILECHOOSER_SAVE);

    if (al_show_native_file_dialog(g_display, chooser))
    {
        if (al_get_native_file_dialog_count(chooser) != 0)
        {
            if (pHandler && fnCallback)
            {
                filePathOut = al_get_native_file_dialog_path(chooser, 0);
            }
        }
    }

    al_destroy_native_file_dialog(chooser);
    return true;
}

bool Gwk::Platform::FolderOpen(const String& Name,
                               const String& StartPath,
                               String& filePathOut);                               
{
    ALLEGRO_FILECHOOSER* chooser = al_create_native_file_dialog(StartPath.c_str(),
                                                                Name.c_str(),
                                                                "*.*",  //
                                                                        // Extension.c_str(),
                                                                ALLEGRO_FILECHOOSER_FOLDER);

    if (al_show_native_file_dialog(g_display, chooser))
    {
        if (al_get_native_file_dialog_count(chooser) != 0)
        {
            if (pHandler && fnCallback)
            {
                filePathOut = al_get_native_file_dialog_path(chooser, 0);
            }
        }
    }

    al_destroy_native_file_dialog(chooser);
    return true;
}

static bool InitAllegro()
{
    if (al_is_system_installed())
        return true;
    
	if (!al_init())
        return false;
	
    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
    if (!event_queue)
        return false;
	
    al_init_image_addon();
    al_init_font_addon();
    al_init_primitives_addon();
    al_init_ttf_addon();
    al_install_mouse();
    al_install_keyboard();
    al_init_native_dialog_addon();
//    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    
    return true;
}

void* Gwk::Platform::CreatePlatformWindow(int x, int y, int w, int h,
                                           const Gwk::String& strWindowTitle)
{
    // Check Allegro has been initialised.
    if (!InitAllegro())
        return NULL;

    al_set_new_window_position(x, y);
    al_set_new_display_flags(ALLEGRO_WINDOWED|ALLEGRO_FRAMELESS);
    ALLEGRO_DISPLAY* display = al_create_display(w, h);

    if (!display)
        return NULL;

    g_display = display;
    al_set_window_title(display, strWindowTitle.c_str());     // invisible as
                                                              // frameless?
    g_event_queue = al_create_event_queue();

    if (!g_event_queue)
        return NULL;

    al_init_image_addon();
    al_init_font_addon();
    al_init_primitives_addon();
    al_init_ttf_addon();
    al_install_mouse();
    al_install_keyboard();
    al_register_event_source(g_event_queue, al_get_display_event_source(display));
    al_register_event_source(g_event_queue, al_get_mouse_event_source());
    al_register_event_source(g_event_queue, al_get_keyboard_event_source());
    
    return display;
}

void Gwk::Platform::DestroyPlatformWindow(void* pPtr)
{
    ALLEGRO_DISPLAY* display = (ALLEGRO_DISPLAY*)pPtr;
    al_destroy_display(display);
    al_destroy_event_queue(g_event_queue);
    g_event_queue = NULL;
}

bool Gwk::Platform::MessagePump(void* pWindow)
{
    static bool firstCall = true;

    if (firstCall)
    {
        firstCall = false;
        g_GworkInput.Initialize(ptarget);
    }

    ALLEGRO_EVENT ev;
    while (al_get_next_event(g_event_queue, &ev))
    {
        g_GworkInput.ProcessMessage(ev);
    }
    
    return false;
}

void Gwk::Platform::SetBoundsPlatformWindow(void* pPtr, int x, int y, int w, int h)
{
    ALLEGRO_DISPLAY* display = (ALLEGRO_DISPLAY*)pPtr;
    al_set_window_position(display, x, y);

    if (al_get_display_width(display) != w || al_get_display_height(display) != h)
        al_resize_display(display, w, h);
}

void Gwk::Platform::SetWindowMaximized(void* pPtr, bool bMax, Gwk::Point& pNewPos,
                                        Gwk::Point& pNewSize)
{
    ALLEGRO_DISPLAY* display = (ALLEGRO_DISPLAY*)pPtr;

    if (bMax)
    {
        // Go full screen allowing for any other screen real estate.
        ALLEGRO_MONITOR_INFO info;
        al_get_monitor_info(0, &info);
        int w = info.x2-info.x1, h = info.y2-info.y1;
#if defined (ALLEGRO_MACOSX)
        const int c_titleBarHeight = 20;
        al_resize_display(display, w, h-c_titleBarHeight);
        al_set_window_position(display, 0, c_titleBarHeight);
#else
        al_resize_display(display, w, h);
        al_set_window_position(display, 0, 0);
#endif
    }
    else
    {
        // Restore to a reasonable size.
        ALLEGRO_MONITOR_INFO info;
        al_get_monitor_info(0, &info);
        int w = info.x2-info.x1, h = info.y2-info.y1;
        al_resize_display(display, w/2, h/2);
        al_set_window_position(display, w/4, h/4);
    }

    al_get_window_position(display, &pNewPos.x, &pNewPos.y);
    pNewSize.x = al_get_display_width(display);
    pNewSize.y = al_get_display_height(display);
}

void Gwk::Platform::SetWindowMinimized(void* pPtr, bool bMinimized)
{
}

bool Gwk::Platform::HasFocusPlatformWindow(void* pPtr)
{
    return true;
}

void Gwk::Platform::GetDesktopSize(int& w, int& h)
{
    if (!al_is_system_installed())
        al_init();

    ALLEGRO_MONITOR_INFO info;
    al_get_monitor_info(0, &info);
    w = info.x2-info.x1;
    h = info.y2-info.y1;
}

void Gwk::Platform::GetCursorPos(Gwk::Point& po)
{
    ALLEGRO_MOUSE_STATE mouse;
    al_get_mouse_state(&mouse);
    int wx, wy;
    al_get_window_position(g_display, &wx, &wy);
    po.x = mouse.x+wx;
    po.y = mouse.y+wy;
}
