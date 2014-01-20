#include "StdAfx.h"
#include "game.h"
#include "config.h"
#include "util.h"

Root            *m_root				= NULL;    /// Root 
SceneManager    *m_scene_mgr		= NULL;    /// ����������
Camera          *m_camera			= NULL;    /// �����
Viewport        *m_view_port		= NULL;    /// ��ͼ
RenderWindow    *m_render_window	= NULL;	   /// ��Ⱦ����
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
	//����root
	String pluginsPath;
	// only use plugins.cfg if not static
#ifndef OGRE_STATIC_LIB
	pluginsPath = m_resource_path + "plugins.cfg";
#endif

	m_root = new Root(pluginsPath, m_resource_path + "ogre.cfg", m_resource_path + "Ogre.log");

	if (m_root == NULL)
		return false;

	//������Դ
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

	//����
	if(m_root->restoreConfig())
        m_root->initialise(false);
    else		//����Ҳ���orge.cfg�ļ�, ��ʾһ�����ÿ�
    {
        if (m_root->showConfigDialog() )
            m_root->initialise(false);
    }

	//��������
	NameValuePairList misc;
    misc["externalWindowHandle"] = StringConverter::toString( (size_t)hWnd);
    m_render_window = m_root->createRenderWindow( "My sub render window", 800, 600, false, &misc );
    if (m_render_window == NULL)
        return false;

	m_scene_mgr = m_root->createSceneManager(ST_GENERIC, "ExampleSMInstance");

	//��Ӱ��
	m_camera = m_scene_mgr->createCamera("PlayerCam");

	//z:���� x:ǰ�� y:����
    // Position it at 500 in Z direction
	Vector3 pos = SEConfig::read("camera", "pos").to_vector3();
	Vector3 dir = SEConfig::read("camera", "dir").to_vector3();
	m_camera->setPosition(pos);
	m_camera->setDirection(dir);
    m_camera->setNearClipDistance(2);


	//��ͼ
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
	//ͬ������
	if(curr_frame_ > sync_frame_)	//���û�յ��������ظ���ͬ��֡, �ұ���֡������һ��Ҫͬ����֡, ����֡
		return false;

	if(curr_frame_ == sync_frame_)		//����֡����ͬ��֡ʱ, ����ͬ����Ϣ.
	{
		printf("==%d, %d, %d\n", curr_frame_, next_frame_, sync_frame_);

		std::stringstream ss;
		ss << curr_frame_ << "," << send_key_;
		net_.send(ss.str().c_str());

		memset(send_key_, 0, SYNC_FRAME_RATE);	//������͵Ŀ�����Ϣ

		next_frame_ = curr_frame_ + SYNC_FRAME_RATE;
	}

	curr_frame_++;

	//ÿ֡�߼�
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
	//��֡����
	static const float TIME_PER_FRAME = 1000/60;
	static uint last_frame = GetTickCount();			//��һ�β���ȷ, Ϊ�˰Ѵ���д��ͬһ��, ����.
	while(GetTickCount() < last_frame + TIME_PER_FRAME)
		Sleep(1);
	last_frame += TIME_PER_FRAME;
#endif

	/*
	��֡�߼���֤��ÿ���̶�ʱ������һ��update, ��ͬ�����ƽ�����, ���ǻᰴ�̶���ʱ��������update, ֻ��û����sleep��һ��. 
	����, ��֡ʱ, ������ͣ�Ų���, ������, �����ٴβ���, ���ٶ�Ҳ�������, ��Ϊ�����ʱ����Ҳ�䳤��. ֻ�ǻ���ָй��ϵĿ�,
	������Ϊ��������֡, һ��һͣ.
	����ǲ���ÿ����һ֡���ö�����ǰ��һ֡�ķ�ʽ(ÿ�δ���̶�ʱ����). ��ô������֡��, �����Ĳ��žͻ�ӿ�, ��Ϊ��ʱ
	sleep����ִ��(GetTickCount�ܴ���), Ҳ���ǲ��ٶ�֡��, update��Ƶ�ʱ����, ����ǰ�Ƶ�Ƶ��Ҳ�����.
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

        m_root->renderOneFrame();		//û����һ��, ����ʱ�����
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
			//���¶��ֲ�����Ϣ
			for(int i = 0; i < vec3[1].length(); i++)
				recv_key_.push_back(vec3[1][i]);
			printf("++++recv_key %s\n", vec3[1].c_str());
		}
	}

	return true;
}