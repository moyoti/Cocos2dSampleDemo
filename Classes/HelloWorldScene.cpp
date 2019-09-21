/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
//这里枚举出来的几个变量类似于设定在物理碰撞层面上的图层，用来在添加碰撞器时指定哪些物体可以相互碰撞，cocos2d支持32位标签，所以最多可以有32层
enum class PhysicsCategory {
	None = 0,
	Monster = (1 << 0),    // 1
	Projectile = (1 << 1), // 2
	All = PhysicsCategory::Monster | PhysicsCategory::Projectile // 3
};

Scene* HelloWorld::createScene()
{
	//对此场景开启物理引擎
	auto scene = Scene::createWithPhysics();
	//设置物理引擎的重力方向为0，意味着不使用重力
	scene->getPhysicsWorld()->setGravity(Vec2(0, 0));
	//设置debug时是否显示出碰撞器，不显示可以改为DEBUGDRAW_NONE
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	//创建渲染当前场景
	auto layer = HelloWorld::create();
	//将当前场景载入
	scene->addChild(layer);
    return scene;
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	//初始化场景，若失败则直接return false
	if (!Scene::init()) {
		return false;
	}
	/*
	CCEGLView类中的函数：
	getFrameSize():这个函数返回了屏幕分辨率的大小，在真机中自然是恒定的，当然在调试程序的时候可以利用setFrameSize()函数设置成某一分辨率大小来模拟各种各样的安卓分辨率尺寸。
	getDesignResolutionSize():获得设置的设计分辨率的尺寸，如果没有设置过那么获得值和getFrameSize()相同
	getVisibleSize():获得在当前显示模式下设计分辨率的可见范围，只有在显示模式为kResolutionNoBorder的情况下才有所不同，因为其他情况下都是将设计分辨率全部显示，而kResolutionNoBorder的时候会造成窗口拉伸而超出屏幕的显示范围。
	getVisibleOrigin():和上面的函数一样只有在显示模式为kResolutionNoBorder时候不返回(0,0),而是返回屏幕原点对应的设计分辨率坐标值。
	CCDirector中的函数：
	getWinSize():返回的m_obWinSizeInPoints就是设置的设计分辨率的数值，也就是openGL的view尺寸(不是像素个数!因为一个点不一定对应一个像素点),如果你调用过setDesignResolutionSize()函数，那么在setDesignResolutionSize()中有如下代码CCDirector::sharedDirector()->m_obWinSizeInPoints = getDesignResolutionSize();就是将设置的设计分辨率传递给m_obWinSizeInPoints保存，如果没调用过则在CCDirector::sharedDirector()->setOpenGLView(pEGLView);的时候有如下代码m_obWinSizeInPoints = m_pobOpenGLView->getDesignResolutionSize();显然这时m_obWinSizeInPoints也等于设备分辨率。
	getWinSizeInPixels():返回的是openGL的view的像素尺寸，这个值和m_obWinSizeInPoints和m_fContentScaleFactor有关，而m_fContentScaleFactor就是一个点对应的像素点个数。
	这个值可以理解为你的图片资源和你的设计分辨率的比值，这个值是用来绘制图片的，也就只控制怎么将图片资源绘制到你的窗口分辨下。如果是用一套资源按不同的屏幕分辨率缩放可以不用理会。多套资源的情况下通过调用setContentScaleFactor()去设置，例子可以参考cocos2dx的helloCpp。可以发现在绘制图片时就是通过宏CC_CONTENT_SCALE_FACTOR()->#define CC_CONTENT_SCALE_FACTOR() CCDirector::sharedDirector()->getContentScaleFactor()去控制。
	getVisibleSize()/getVisibleOrigin():就是通过CCEGLView指针m_pobOpenGLView去调用CCEGLView对应的函数。
	*/
	auto origin = Director::getInstance()->getVisibleOrigin();
	auto winSize = Director::getInstance()->getVisibleSize();
	//创建一个方形作为背景
	auto background = DrawNode::create();
	background->drawSolidRect(origin, winSize, Color4F(0.6, 0.6, 0.6, 1.0));
	this->addChild(background);
	//创建玩家精灵并设置位置和大小添加入场景中
	_player = Sprite::create("PurpleMonster.png");
	_player->setPosition(Vec2(winSize.width*0.1, winSize.height*0.5));
	_player->setScale(0.3f);
	this->addChild(_player);
	//srand设置随机数，令随机数变化，若不调用此函数随机数是不会变化的
	srand((unsigned int)time(nullptr));
	//设置一个日程令程序每1.5秒调用一次addMonster函数
	this->schedule(schedule_selector(HelloWorld::addMonster), 1.5);
	/*
	EventListenerTouchOneByOne:此类型对每个触摸事件调用一次回调方法
	EventListenerTouchAllAtOnce:此类型对所有的触摸事件调用一次回调方法
	<-每个事件监听器支持4个回调，但你只需要为自己关心的事件绑定方法。->
	onTouchBegan:手指第一次碰到屏幕时被调用。如果你使用的是EventListenerTouchOneByOne，你必须返回true才能获取另外3个触摸事件
	onTouchMoved:手指接触屏幕并移动（保持接触）时被调用
	onTouchEnded:手指离开屏幕时被调用
	onTouchCancelled:在特定的结束事件处理的环境中被调用，如你正在触屏的时候，一个电话打了进来打断了这个app进程
	*/
	//添加点击事件监听器
	auto eventListener = EventListenerTouchOneByOne::create();
	eventListener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
	this->getEventDispatcher() ->addEventListenerWithSceneGraphPriority(eventListener, _player);
	//添加碰撞器事件监听器
	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(HelloWorld::onContactBegan, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);

	return true;
}

void HelloWorld::addMonster(float dt) {
	//创建怪物精灵
	auto monster = Sprite::create("PurpleMonster.png");
	// 1
	auto monsterSize = monster->getContentSize();//获取当前怪物的Size
	auto physicsBody = PhysicsBody::createBox(Size(monsterSize.width, monsterSize.height),
		PhysicsMaterial(0.1f, 0.1f, 0.0f));//创建物理碰撞体，第一个参数设置大小，第二个参数设置物理材质
	// 2 设置物理碰撞器为动态的，意味着该碰撞器不会受到其他碰撞器的物理作用，简单来说不受力
	physicsBody->setDynamic(true);
	// 3 
	physicsBody->setCategoryBitmask((int)PhysicsCategory::Monster);//定义碰撞器所属类型
	physicsBody->setCollisionBitmask((int)PhysicsCategory::None);//定义在碰撞期间哪一种类型的碰撞器会影响到当前碰撞器，由于当前物理碰撞器为动态的，所以这个地方只能为none也默认为none
	physicsBody->setContactTestBitmask((int)PhysicsCategory::Projectile);//定义该碰撞器发生碰撞时与哪些碰撞器产生碰撞时会发送消息通知
	//将碰撞器添加到当前物体上
	monster->setPhysicsBody(physicsBody);


	// 1
	auto monsterContentSize = monster->getContentSize();//获取怪物模型的大小，返回Size数据结构，内部只有两个值float width和float height
	auto selfContentSize = this->getContentSize();//获取场景界面大小，返回Size数据结构
	//设置最大高度和最小高度，为了不让怪物模型不超出屏幕范围
	int minY = monsterContentSize.height / 2;
	int maxY = selfContentSize.height - monsterContentSize.height / 2;
	int rangeY = maxY - minY;
	int randomY = (rand() % rangeY) + minY;
	//设置怪物出现位置和大小
	monster->setPosition(Vec2(selfContentSize.width + monsterContentSize.width / 2, randomY));
	monster->setScale(0.3f);
	this->addChild(monster);

	// 2设置怪物的动作时长，这里设置为2-4秒之间的随机数
	int minDuration = 2.0;
	int maxDuration = 4.0;
	int rangeDuration = maxDuration - minDuration;
	int randomDuration = (rand() % rangeDuration) + minDuration;

	// 3
	auto actionMove = MoveTo::create(randomDuration, Vec2(-monsterContentSize.width / 2, randomY));//创建一个移动的动作
	auto actionRemove = RemoveSelf::create();//将一个节点从其父节点中移除，从而有效地将其从场景中“删除”。
	monster->runAction(Sequence::create(actionMove, actionRemove, nullptr)); //Sequence：允许你按顺序实现一系列的其他动作，每次实现一个。这意味着，你可以让怪物移动穿过整个场景，到达特定的位置之后再将其从屏幕上移除。
}

bool HelloWorld::onContactBegan(PhysicsContact &contact) {
	//当前函数会被调用两次，猜测可能是因为两个碰撞器相互碰撞所以各运行一次，而运行过一次后nodeB会被删除掉导致空指针错误所以这里进行null判断
	auto nodeA = contact.getShapeA()->getBody()->getNode();
	auto nodeB = contact.getShapeB()->getBody()->getNode();
	
	if (nodeA != NULL) {
		nodeA->removeFromParent();
	}
	if (nodeB != NULL) {
		nodeB->removeFromParent();
	}
	
	return true;
}

bool HelloWorld::onTouchBegan(Touch *touch, Event *unused_event) {
	// 1  - Just an example for how to get the  _player object
	//auto node = unused_event->getCurrentTarget();

	// 2
	Vec2 touchLocation = touch->getLocation();//获取触摸点坐标
	Vec2 offset = touchLocation - _player->getPosition();//获取触摸点与玩家角色之间的二维差值

	// 3 若x方向的差值小于0则不作任何操作
	if (offset.x < 0) {
		return true;
	}

	// 4 精灵对象初始化
	auto projectile = Sprite::create("projectile.png");
	projectile->setPosition(_player->getPosition());
	projectile->setScale(0.2f);
	//添加碰撞器，具体解释参照上面怪物添加时的注释
	auto projectileSize = projectile->getContentSize();
	auto physicsBody = PhysicsBody::createCircle(projectileSize.width / 2);
	physicsBody->setDynamic(true);
	physicsBody->setCategoryBitmask((int)PhysicsCategory::Projectile);
	physicsBody->setCollisionBitmask((int)PhysicsCategory::None);
	physicsBody->setContactTestBitmask((int)PhysicsCategory::Monster);
	projectile->setPhysicsBody(physicsBody);

	this->addChild(projectile);

	// 5 将二维差值标准化为方向向量
	offset.normalize();
	auto shootAmount = offset * 1000;

	// 6 设置计算子弹飞行的最终目的地
	auto realDest = shootAmount + projectile->getPosition();

	// 7 子弹飞行运动
	auto actionMove = MoveTo::create(2.0f, realDest);
	auto actionRemove = RemoveSelf::create();
	projectile->runAction(Sequence::create(actionMove, actionRemove, nullptr));

	return true;
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}
