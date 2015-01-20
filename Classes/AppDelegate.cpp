#include "AppDelegate.h"
#include "Scenes/StartScreen.h"

USING_NS_CC;

AppDelegate::AppDelegate() {}

AppDelegate::~AppDelegate() {}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        // Размер окна программы: ширина, высота.
        std::pair <int, int> frame_size(1600, 1000);
        glview = GLView::create("Kolobok time!");
        glview->setFrameSize(frame_size.first, frame_size.second);
        director->setOpenGLView(glview);
        // Поместим окно в центр экрана.
        auto monitor_size = glview->getMonitorResolution();
        glview->setWindowPosition(
            (monitor_size.first - frame_size.first) / 2, 
            (monitor_size.second - frame_size.second) / 2
        );
        // После этого вызова будут правильно определяться координаты мыши.
        glview->setDesignResolutionSize(frame_size.first, frame_size.second, ResolutionPolicy::SHOW_ALL);
        
        //glview->setMouseMode('h');
    }

    // turn on display FPS
    director->setDisplayStats(false);
    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);
    
    srand(time(nullptr)); // Для случайных величин.
    
    auto scene = StartScreen::create();
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
