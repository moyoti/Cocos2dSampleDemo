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
//����ö�ٳ����ļ��������������趨��������ײ�����ϵ�ͼ�㣬�����������ײ��ʱָ����Щ��������໥��ײ��cocos2d֧��32λ��ǩ��������������32��
enum class PhysicsCategory {
	None = 0,
	Monster = (1 << 0),    // 1
	Projectile = (1 << 1), // 2
	All = PhysicsCategory::Monster | PhysicsCategory::Projectile // 3
};

Scene* HelloWorld::createScene()
{
	//�Դ˳���������������
	auto scene = Scene::createWithPhysics();
	//���������������������Ϊ0����ζ�Ų�ʹ������
	scene->getPhysicsWorld()->setGravity(Vec2(0, 0));
	//����debugʱ�Ƿ���ʾ����ײ��������ʾ���Ը�ΪDEBUGDRAW_NONE
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	//������Ⱦ��ǰ����
	auto layer = HelloWorld::create();
	//����ǰ��������
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
	//��ʼ����������ʧ����ֱ��return false
	if (!Scene::init()) {
		return false;
	}
	/*
	CCEGLView���еĺ�����
	getFrameSize():���������������Ļ�ֱ��ʵĴ�С�����������Ȼ�Ǻ㶨�ģ���Ȼ�ڵ��Գ����ʱ���������setFrameSize()�������ó�ĳһ�ֱ��ʴ�С��ģ����ָ����İ�׿�ֱ��ʳߴ硣
	getDesignResolutionSize():������õ���Ʒֱ��ʵĳߴ磬���û�����ù���ô���ֵ��getFrameSize()��ͬ
	getVisibleSize():����ڵ�ǰ��ʾģʽ����Ʒֱ��ʵĿɼ���Χ��ֻ������ʾģʽΪkResolutionNoBorder������²�������ͬ����Ϊ��������¶��ǽ���Ʒֱ���ȫ����ʾ����kResolutionNoBorder��ʱ�����ɴ��������������Ļ����ʾ��Χ��
	getVisibleOrigin():������ĺ���һ��ֻ������ʾģʽΪkResolutionNoBorderʱ�򲻷���(0,0),���Ƿ�����Ļԭ���Ӧ����Ʒֱ�������ֵ��
	CCDirector�еĺ�����
	getWinSize():���ص�m_obWinSizeInPoints�������õ���Ʒֱ��ʵ���ֵ��Ҳ����openGL��view�ߴ�(�������ظ���!��Ϊһ���㲻һ����Ӧһ�����ص�),�������ù�setDesignResolutionSize()��������ô��setDesignResolutionSize()�������´���CCDirector::sharedDirector()->m_obWinSizeInPoints = getDesignResolutionSize();���ǽ����õ���Ʒֱ��ʴ��ݸ�m_obWinSizeInPoints���棬���û���ù�����CCDirector::sharedDirector()->setOpenGLView(pEGLView);��ʱ�������´���m_obWinSizeInPoints = m_pobOpenGLView->getDesignResolutionSize();��Ȼ��ʱm_obWinSizeInPointsҲ�����豸�ֱ��ʡ�
	getWinSizeInPixels():���ص���openGL��view�����سߴ磬���ֵ��m_obWinSizeInPoints��m_fContentScaleFactor�йأ���m_fContentScaleFactor����һ�����Ӧ�����ص������
	���ֵ�������Ϊ���ͼƬ��Դ�������Ʒֱ��ʵı�ֵ�����ֵ����������ͼƬ�ģ�Ҳ��ֻ������ô��ͼƬ��Դ���Ƶ���Ĵ��ڷֱ��¡��������һ����Դ����ͬ����Ļ�ֱ������ſ��Բ�����ᡣ������Դ�������ͨ������setContentScaleFactor()ȥ���ã����ӿ��Բο�cocos2dx��helloCpp�����Է����ڻ���ͼƬʱ����ͨ����CC_CONTENT_SCALE_FACTOR()->#define CC_CONTENT_SCALE_FACTOR() CCDirector::sharedDirector()->getContentScaleFactor()ȥ���ơ�
	getVisibleSize()/getVisibleOrigin():����ͨ��CCEGLViewָ��m_pobOpenGLViewȥ����CCEGLView��Ӧ�ĺ�����
	*/
	auto origin = Director::getInstance()->getVisibleOrigin();
	auto winSize = Director::getInstance()->getVisibleSize();
	//����һ��������Ϊ����
	auto background = DrawNode::create();
	background->drawSolidRect(origin, winSize, Color4F(0.6, 0.6, 0.6, 1.0));
	this->addChild(background);
	//������Ҿ��鲢����λ�úʹ�С����볡����
	_player = Sprite::create("PurpleMonster.png");
	_player->setPosition(Vec2(winSize.width*0.1, winSize.height*0.5));
	_player->setScale(0.3f);
	this->addChild(_player);
	//srand�������������������仯���������ô˺���������ǲ���仯��
	srand((unsigned int)time(nullptr));
	//����һ���ճ������ÿ1.5�����һ��addMonster����
	this->schedule(schedule_selector(HelloWorld::addMonster), 1.5);
	/*
	EventListenerTouchOneByOne:�����Ͷ�ÿ�������¼�����һ�λص�����
	EventListenerTouchAllAtOnce:�����Ͷ����еĴ����¼�����һ�λص�����
	<-ÿ���¼�������֧��4���ص�������ֻ��ҪΪ�Լ����ĵ��¼��󶨷�����->
	onTouchBegan:��ָ��һ��������Ļʱ�����á������ʹ�õ���EventListenerTouchOneByOne������뷵��true���ܻ�ȡ����3�������¼�
	onTouchMoved:��ָ�Ӵ���Ļ���ƶ������ֽӴ���ʱ������
	onTouchEnded:��ָ�뿪��Ļʱ������
	onTouchCancelled:���ض��Ľ����¼�����Ļ����б����ã��������ڴ�����ʱ��һ���绰���˽�����������app����
	*/
	//��ӵ���¼�������
	auto eventListener = EventListenerTouchOneByOne::create();
	eventListener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
	this->getEventDispatcher() ->addEventListenerWithSceneGraphPriority(eventListener, _player);
	//�����ײ���¼�������
	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(HelloWorld::onContactBegan, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);

	return true;
}

void HelloWorld::addMonster(float dt) {
	//�������ﾫ��
	auto monster = Sprite::create("PurpleMonster.png");
	// 1
	auto monsterSize = monster->getContentSize();//��ȡ��ǰ�����Size
	auto physicsBody = PhysicsBody::createBox(Size(monsterSize.width, monsterSize.height),
		PhysicsMaterial(0.1f, 0.1f, 0.0f));//����������ײ�壬��һ���������ô�С���ڶ������������������
	// 2 ����������ײ��Ϊ��̬�ģ���ζ�Ÿ���ײ�������ܵ�������ײ�����������ã�����˵������
	physicsBody->setDynamic(true);
	// 3 
	physicsBody->setCategoryBitmask((int)PhysicsCategory::Monster);//������ײ����������
	physicsBody->setCollisionBitmask((int)PhysicsCategory::None);//��������ײ�ڼ���һ�����͵���ײ����Ӱ�쵽��ǰ��ײ�������ڵ�ǰ������ײ��Ϊ��̬�ģ���������ط�ֻ��ΪnoneҲĬ��Ϊnone
	physicsBody->setContactTestBitmask((int)PhysicsCategory::Projectile);//�������ײ��������ײʱ����Щ��ײ��������ײʱ�ᷢ����Ϣ֪ͨ
	//����ײ����ӵ���ǰ������
	monster->setPhysicsBody(physicsBody);


	// 1
	auto monsterContentSize = monster->getContentSize();//��ȡ����ģ�͵Ĵ�С������Size���ݽṹ���ڲ�ֻ������ֵfloat width��float height
	auto selfContentSize = this->getContentSize();//��ȡ���������С������Size���ݽṹ
	//�������߶Ⱥ���С�߶ȣ�Ϊ�˲��ù���ģ�Ͳ�������Ļ��Χ
	int minY = monsterContentSize.height / 2;
	int maxY = selfContentSize.height - monsterContentSize.height / 2;
	int rangeY = maxY - minY;
	int randomY = (rand() % rangeY) + minY;
	//���ù������λ�úʹ�С
	monster->setPosition(Vec2(selfContentSize.width + monsterContentSize.width / 2, randomY));
	monster->setScale(0.3f);
	this->addChild(monster);

	// 2���ù���Ķ���ʱ������������Ϊ2-4��֮��������
	int minDuration = 2.0;
	int maxDuration = 4.0;
	int rangeDuration = maxDuration - minDuration;
	int randomDuration = (rand() % rangeDuration) + minDuration;

	// 3
	auto actionMove = MoveTo::create(randomDuration, Vec2(-monsterContentSize.width / 2, randomY));//����һ���ƶ��Ķ���
	auto actionRemove = RemoveSelf::create();//��һ���ڵ���丸�ڵ����Ƴ����Ӷ���Ч�ؽ���ӳ����С�ɾ������
	monster->runAction(Sequence::create(actionMove, actionRemove, nullptr)); //Sequence�������㰴˳��ʵ��һϵ�е�����������ÿ��ʵ��һ��������ζ�ţ�������ù����ƶ��������������������ض���λ��֮���ٽ������Ļ���Ƴ���
}

bool HelloWorld::onContactBegan(PhysicsContact &contact) {
	//��ǰ�����ᱻ�������Σ��²��������Ϊ������ײ���໥��ײ���Ը�����һ�Σ������й�һ�κ�nodeB�ᱻɾ�������¿�ָ����������������null�ж�
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
	Vec2 touchLocation = touch->getLocation();//��ȡ����������
	Vec2 offset = touchLocation - _player->getPosition();//��ȡ����������ҽ�ɫ֮��Ķ�ά��ֵ

	// 3 ��x����Ĳ�ֵС��0�����κβ���
	if (offset.x < 0) {
		return true;
	}

	// 4 ��������ʼ��
	auto projectile = Sprite::create("projectile.png");
	projectile->setPosition(_player->getPosition());
	projectile->setScale(0.2f);
	//�����ײ����������Ͳ�������������ʱ��ע��
	auto projectileSize = projectile->getContentSize();
	auto physicsBody = PhysicsBody::createCircle(projectileSize.width / 2);
	physicsBody->setDynamic(true);
	physicsBody->setCategoryBitmask((int)PhysicsCategory::Projectile);
	physicsBody->setCollisionBitmask((int)PhysicsCategory::None);
	physicsBody->setContactTestBitmask((int)PhysicsCategory::Monster);
	projectile->setPhysicsBody(physicsBody);

	this->addChild(projectile);

	// 5 ����ά��ֵ��׼��Ϊ��������
	offset.normalize();
	auto shootAmount = offset * 1000;

	// 6 ���ü����ӵ����е�����Ŀ�ĵ�
	auto realDest = shootAmount + projectile->getPosition();

	// 7 �ӵ������˶�
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
