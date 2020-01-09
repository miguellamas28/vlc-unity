#include "PlatformBase.h"
#include "RenderAPI.h"
#include "Log.h"

#include <map>
#include <windows.h>

extern "C" {
#include <stdlib.h>
#include <unistd.h>
#include <vlc/vlc.h>
#include <string.h>
}

static UnityGfxRenderer s_DeviceType = kUnityGfxRendererNull;

static int   g_TextureWidth  = 0;
static int   g_TextureHeight = 0;
static int   g_TextureRowPitch = 0;

libvlc_instance_t * inst;
libvlc_media_player_t * mp;

static IUnityGraphics* s_Graphics = NULL;
static std::map<libvlc_media_player_t*,RenderAPI*> contexts = {};
static IUnityInterfaces* s_UnityInterfaces = NULL;

static UINT Width;
static UINT Height;
static void* Hwnd;

/** LibVLC's API function exported to Unity
 *
 * Every following functions will be exported to. Unity We have to
 * redeclare the LibVLC's function for the keyword
 * UNITY_INTERFACE_EXPORT and UNITY_INTERFACE_API
 */

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetPluginPath(char* path)
{
    DEBUG("SetPluginPath \n");
    auto e = _putenv_s("VLC_PLUGIN_PATH", path);
    if(e != 0)
        DEBUG("_putenv_s failed");
    else DEBUG("_putenv_s succeeded");
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API Print(char* toPrint)
{
    DEBUG("PRINTING \n");
    DEBUG("%s", toPrint);
}

extern "C"
{
    typedef void(*FuncCallBack)(const char* message, int size);
    static FuncCallBack callbackInstance = nullptr;
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API RegisterDebugCallback(FuncCallBack cb) {
    callbackInstance = cb;
}

void static Log(const char* message) {
    if (callbackInstance != nullptr)
        callbackInstance(message, (int)strlen(message));
}

extern "C" libvlc_media_player_t* UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
CreateAndInitMediaPlayer(libvlc_instance_t* libvlc)
{
    if(libvlc == NULL)
    {
        Log("libvlc is NULL, aborting...");
        return NULL;
    }

    inst = libvlc;

    Log("LAUNCH");

    if (inst == NULL) {
        Log("LibVLC is not instanciated");
        return NULL;
    }

    if(mp != NULL)
    {
        Log("mp is already setup...");
        return mp;
    }

    mp = libvlc_media_player_new(inst);
    RenderAPI* s_CurrentAPI;

    if (mp == NULL) {
        Log("Error initializing media player");
        goto err;
    }

    Log("Calling... Initialize Render API \n");

    s_DeviceType = s_Graphics->GetRenderer();

    Log("Calling... CreateRenderAPI \n");

    s_CurrentAPI = CreateRenderAPI(s_DeviceType);
    
    if(s_CurrentAPI == NULL)
    {
        Log("s_CurrentAPI is NULL \n");    
    }    
    
    Log("Calling... ProcessDeviceEvent \n");
    
    s_CurrentAPI->ProcessDeviceEvent(kUnityGfxDeviceEventInitialize, s_UnityInterfaces);

    // Log("Calling... setVlcContext s_CurrentAPI=" << s_CurrentAPI " mp=" << mp);
    s_CurrentAPI->setVlcContext(mp);

    contexts[mp] = s_CurrentAPI;

    return mp;
err:
    if ( mp ) {
        Log("Error in CreateAndInitMediaPlayer!!!!!");
        // Stop playing
        libvlc_media_player_stop_async (mp);

        // Free the media_player
        libvlc_media_player_release (mp);
        mp = NULL;
    }    
    return NULL;
}

extern "C" void* UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
getTexture(libvlc_media_player_t* mp, bool * updated)
{
    if(mp == NULL)
        return NULL;

    RenderAPI* s_CurrentAPI = contexts.find(mp)->second;

    if (!s_CurrentAPI) {
        DEBUG("Error, no Render API");
        if (updated)
            *updated = false;
        return nullptr;
    }

    return s_CurrentAPI->getVideoFrame(updated);
}

static void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType);

extern "C" void	UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces)
{
    DEBUG("UnityPluginLoad");
    s_UnityInterfaces = unityInterfaces;
    s_Graphics = s_UnityInterfaces->Get<IUnityGraphics>();
    s_Graphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);

    // Run OnGraphicsDeviceEvent(initialize) manually on plugin load
    OnGraphicsDeviceEvent(kUnityGfxDeviceEventInitialize);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginUnload()
{
  s_Graphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
}


static RenderAPI* EarlyRenderAPI = NULL;

static void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType)
{
    // Create graphics API implementation upon initialization
    if (eventType == kUnityGfxDeviceEventInitialize) {
        DEBUG("Initialise Render API");
        if (EarlyRenderAPI != NULL) {
            DEBUG("*** EarlyRenderAPI != NULL while initialising ***");
            return;
        }

        DEBUG("s_Graphics->GetRenderer() \n");

        s_DeviceType = s_Graphics->GetRenderer();

        DEBUG("CreateRenderAPI(s_DeviceType) \n");

        EarlyRenderAPI = CreateRenderAPI(s_DeviceType);
        return;
    }
    else if (eventType == kUnityGfxDeviceEventShutdown)
	{
	}

    if(EarlyRenderAPI){
        EarlyRenderAPI->ProcessDeviceEvent(eventType, s_UnityInterfaces);
    } else {
        DEBUG("Unable to process event, no Render API");
    }

    // Let the implementation process the device related events
    std::map<libvlc_media_player_t*, RenderAPI*>::iterator it;

    for(it = contexts.begin(); it != contexts.end(); it++)
    {
        RenderAPI* currentAPI = it->second;
        if(currentAPI) {
            DEBUG(" currentAPI->ProcessDeviceEvent(eventType, s_UnityInterfaces); \n");
            currentAPI->ProcessDeviceEvent(eventType, s_UnityInterfaces);
        }
    }
}

static void UNITY_INTERFACE_API OnRenderEvent(int eventID)
{
}

extern "C" UnityRenderingEvent UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetRenderEventFunc()
{
    return OnRenderEvent;
}