#include "StdAfx.h"
#include "game.h"
#include "config.h"
#include "util.h"

Root            *m_root				= NULL;    /// Root 
SceneManager    *m_scene_mgr		= NULL;    /// 场景管理器
Camera          *m_camera			= NULL;    /// 摄像机
Viewport        *m_view_port		= NULL;    /// 视图
RenderWindow    *m_render_window	= NULL;	   /// 渲染窗口
Ogre::String    m_resource_path;

extern game my_game;

void on_recv(const char* _content)
{
	my_game.net_recv(_content);
}

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

	net_.create(SEConfig::read("server", "ip").to_string().c_str(), SEConfig::read("server", "port").to_int(), on_recv);

	return true;
}

#define RESTRICT_FRAME

bool game::update()
{
	//同步控制
	if(curr_frame_ > sync_frame_)	//如果没收到服务器回复的同步帧, 且本地帧大于下一次要同步的帧, 则锁帧
		return false;

	if(curr_frame_ == sync_frame_)		//本地帧等于同步帧时, 发送同步信息.
	{
		printf("==%d, %d, %d\n", curr_frame_, next_frame_, sync_frame_);

		std::stringstream ss;
		ss << curr_frame_ << "," << send_key_;
		net_.send(ss.str().c_str());

		memset(send_key_, 0, SYNC_FRAME_RATE);	//清掉发送的控制信息

		next_frame_ = curr_frame_ + SYNC_FRAME_RATE;
	}

	curr_frame_++;

	//每帧逻辑
	static uint last_time = GetTickCount();
	float elapse = (GetTickCount() - last_time)/1000.0;
	last_time = GetTickCount();

	ctrl_.update(down_key_, elapse);
	char recv_key = 0;
	if(!recv_key_.empty())
	{
		recv_key = recv_key_.front();
		recv_key_.pop_front();
	}
	rival_.update(recv_key, elapse);

	if(down_key_ != 0)
	{
		for(int i = 0; i < SYNC_FRAME_RATE; i++)
		{
			if(send_key_[i] == 0)
			{
				send_key_[i] = down_key_;
				break;
			}
		}
	}
	down_key_ = 0;

	g_character_mgr.update_animation(elapse);
	m_root->renderOneFrame();

#ifdef RESTRICT_FRAME
	//定帧控制
	static const float TIME_PER_FRAME = 1000/60;
	static uint last_frame = GetTickCount();			//第一次不正确, 为了把代码写在同一块, 算了.
	while(GetTickCount() < last_frame + TIME_PER_FRAME)
		Sleep(1);
	last_frame += TIME_PER_FRAME;
#endif

	/*
	定帧逻辑保证了每隔固定时间运行一次update, 当同步控制解锁后, 还是会按固定的时间来运行update, 只是没有了sleep这一步. 
	另外, 锁帧时, 动画会停着不动, 解锁后, 动画再次播放, 但速度也不会变慢, 因为传入的时间间隔也变长了. 只是会出现感官上的卡,
	这是因为连续的锁帧, 一播一停.
	如果是采用每运行一帧就让动画向前推一帧的方式(每次传入固定时间间隔). 那么当锁完帧后, 动画的播放就会加快, 因为这时
	sleep不再执行(GetTickCount很大了), 也就是不再定帧了, update的频率变快了, 动画前推的频率也变快了.
	*/

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
	ctrl_.set_type(SEConfig::read("game", "action").to_string().c_str());
	rival_.set_type("yinjie");

	return true;
}

bool game::destroy_character()
{
	return true;
}

bool game::net_recv(const char* _content)
{
	static int last_time = GetTickCount();

	std::vector<std::string> vec = SEUtil::splite(_content, ",");
	if(vec.size() < 2)
		return false;

	sync_frame_ = atoi(vec[1].c_str());

	std::vector<std::string> vec2 = SEUtil::splite(vec[0].c_str(), "|");
	std::vector<std::string>::iterator it2 = vec2.begin();
	for(; it2 != vec2.end(); it2++)
	{
		std::vector<std::string> vec3 = SEUtil::splite(it2->c_str(), " ");
		if(vec3.size() > 1 && atoi(vec3[0].c_str()) != net_.get_id())
		{
			//更新对手操作信息
			for(int i = 0; i < vec3[1].length(); i++)
				recv_key_.push_back(vec3[1][i]);
			printf("++++recv_key %s\n", vec3[1].c_str());
		}
	}

	return true;
}