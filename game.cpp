#include "StdAfx.h"
#include "game.h"
#include "config.h"

Root            *m_root				= NULL;    /// Root 
SceneManager    *m_scene_mgr		= NULL;    /// 场景管理器
Camera          *m_camera			= NULL;    /// 摄像机
Viewport        *m_view_port		= NULL;    /// 视图
RenderWindow    *m_render_window	= NULL;	   /// 渲染窗口
Ogre::String    m_resource_path;

game::game(void)
{
}

game::~game(void)
{
}

bool game::init(const HWND& hWnd)
{
	//创建root
	String pluginsPath;
	// only use plugins.cfg if not static
#ifndef OGRE_STATIC_LIB
	pluginsPath = m_resource_path + "plugins.cfg";
#endif

	m_root = new Root(pluginsPath, m_resource_path + "ogre.cfg", m_resource_path + "Ogre.log");

	if (m_root == NULL)
		return false;

	//导入资源
	ConfigFile cf;
    cf.load(m_resource_path + "resources.cfg");

    // Go through all sections & settings in the file
    ConfigFile::SectionIterator seci = cf.getSectionIterator();

    String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        ConfigFile::SettingsMultiMap *settings = seci.getNext();
        ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }

	//配置
	if(m_root->restoreConfig())
        m_root->initialise(false);
    else		//如果找不到orge.cfg文件, 显示一个配置框
    {
        if (m_root->showConfigDialog() )
            m_root->initialise(false);
    }

	//创建窗口
	NameValuePairList misc;
    misc["externalWindowHandle"] = StringConverter::toString( (size_t)hWnd);
    m_render_window = m_root->createRenderWindow( "My sub render window", 800, 600, false, &misc );
    if (m_render_window == NULL)
        return false;

	m_scene_mgr = m_root->createSceneManager(ST_GENERIC, "ExampleSMInstance");

	//摄影机
	m_camera = m_scene_mgr->createCamera("PlayerCam");

	//z:左右 x:前后 y:上下
    // Position it at 500 in Z direction
	Vector3 pos = SEConfig::read("camera", "pos").to_vector3();
	Vector3 dir = SEConfig::read("camera", "dir").to_vector3();
	m_camera->setPosition(pos);
	m_camera->setDirection(dir);
    m_camera->setNearClipDistance(2);


	//视图
	// Create one viewport, entire window
    Viewport* vp = m_render_window->addViewport(m_camera);
    vp->setBackgroundColour(ColourValue(0,0,0));

    // Alter the camera aspect ratio to match the viewport
    m_camera->setAspectRatio(Real(vp->getActualWidth()) / Real(vp->getActualHeight()));

	TextureManager::getSingleton().setDefaultNumMipmaps(5);
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	create_character();

	return true;
}

#define RESTRICT_FRAME

bool game::update()
{
	static uint last_time = GetTickCount();
	uint elapse = GetTickCount() - last_time;

	ctrl_.update(down_key_);
	down_key_ = 0;

	g_character_mgr.update_animation(elapse/1000.0);
	m_root->renderOneFrame();

	last_time = GetTickCount();

#ifdef RESTRICT_FRAME
	//定帧控制
	static const float TIME_PER_FRAME = 1000/60;
	static uint last_frame = GetTickCount();			//第一次不正确, 为了把代码写在同一块, 算了.
	while(GetTickCount() < last_frame + TIME_PER_FRAME)
		Sleep(1);
	last_frame += TIME_PER_FRAME;
#endif

	return true;
}

bool game::release()
{
	if (m_root)
	{
		delete m_root;
		m_root = NULL;
	}

	return true;
}

bool game::resize(const HWND& hWnd)
{
	if (m_camera) 
    { 
        RECT rect; 
        GetClientRect(hWnd,&rect); 

        // notify "render window" instance 
        if (m_render_window)
            m_render_window->windowMovedOrResized(); 

        // Adjust camera's aspect ratio, too 
        if ((rect.bottom - rect.top) != 0)
            m_camera->setAspectRatio((Ogre::Real)m_render_window->getWidth() / (Ogre::Real)m_render_window->getHeight()); 
        m_camera->yaw(Radian(0));

        m_root->renderOneFrame();		//没加这一句, 缩放时会白屏
    }

	return true;
}

bool game::key_down(uint _key)
{
	down_key_ = _key;

	bool ret = true;
	switch(_key)
	{
	case VK_UP:
	case VK_DOWN:
	case VK_RIGHT:
	case VK_LEFT:
		break;
	case 'W':
	case 'A':
	case 'S':
	case 'D':
	case 'J':
	case 'K':
	case 'L':
		break;
	default:
		ret = false;
		break;
	}
	return ret;
}

bool game::key_up(uint _key)
{
	return true;
}

bool game::create_character()
{
	ctrl_.set_type("yinzi");

	return true;
}

bool game::destroy_character()
{
	return true;
}