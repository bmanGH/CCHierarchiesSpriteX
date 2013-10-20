//
// CCHierarchiesSprite功能性单元测试场景
//

#include "CCHierarchiesSpriteFunctionalTestScene.h"
#include "CCHierarchiesSpriteRuntime.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#   include <mach/mach.h>
#endif

USING_NS_CC;
USING_NS_CC_EXT;

static void report_memory (void) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    struct task_basic_info info;
    mach_msg_type_number_t size = sizeof(info);
    kern_return_t kerr = task_info(mach_task_self(),
                                   TASK_BASIC_INFO,
                                   (task_info_t)&info,
                                   &size);
    if( kerr == KERN_SUCCESS ) {
        CCLOG("Memory in use %u : %uKB : %uMB", info.resident_size, info.resident_size / 1024, info.resident_size / 1024 / 1024);
        CCLOG("VM in use %u : %uKB : %uMB", info.virtual_size, info.virtual_size / 1024, info.virtual_size / 1024 / 1024);
    } else {
        CCLOG("Error with task_info(): %s", mach_error_string(kerr));
    }
#endif
}

CCScene* CCHierarchiesSpriteFunctionalTestScene::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    CCHierarchiesSpriteFunctionalTestScene *layer = CCHierarchiesSpriteFunctionalTestScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

CCHierarchiesSpriteFunctionalTestScene::~CCHierarchiesSpriteFunctionalTestScene () {
    CC_SAFE_RELEASE(_hSprites);
}

bool CCHierarchiesSpriteFunctionalTestScene::init()
{
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    
    this->setTouchEnabled(true);
    
    this->scheduleUpdate();
    
    _hSprites = new CCArray();
    _hSprites->init();
    
    // CCHierarchiesSpriteFilter 测试
//    CCDirector::sharedDirector()->getScheduler()->setTimeScale(0.5);
    
    CCLayerColor* bgLayer = CCLayerColor::create(ccc4(64, 64, 64, 255));
    this->addChild(bgLayer, -1000);
    
    this->loadSprites();
    
    return true;
}


#pragma mark - Update

void CCHierarchiesSpriteFunctionalTestScene::update (float delta) {
    
}


#pragma mark - Draw

void CCHierarchiesSpriteFunctionalTestScene::draw () {
    
}


#pragma mark - Touch callback

void CCHierarchiesSpriteFunctionalTestScene::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent) {
    // 点击测试
//    CCTouch* touch = (CCTouch*)pTouches->anyObject();
//    CCPoint touchPoint = touch->getLocation();
//    CCLOG("hit at %f, %f", touchPoint.x, touchPoint.y);
//    
//    CCObject* iter = NULL;
//    CCARRAY_FOREACH(_hSprites, iter) {
//        CCHierarchiesSprite* spr = dynamic_cast<CCHierarchiesSprite*>(iter);
//        if (spr && spr->boundingBox().containsPoint(touchPoint)) {
//            CCLOG("hit %s with %u * fxattack", spr->getAnimationName(), spr->getEventContentCount("fxattack"));
//        }
//    }
    
    // 资源管理测试
    if (_hSprites->count() > 0) {
        CCLOG("release unused resource BEGIN");
        report_memory();
        
        CCObject* iter = NULL;
        CCARRAY_FOREACH(_hSprites, iter) {
            CCHierarchiesSprite* spr = dynamic_cast<CCHierarchiesSprite*>(iter);
            spr->removeFromParent();
        }
        _hSprites->removeAllObjects();
        
        CCHierarchiesSpriteRuntime::sharedHierarchiesSpriteRuntime()->releaseUnusedResource();
        CCTextureCache::sharedTextureCache()->removeUnusedTextures();
        
        report_memory();
        CCLOG("release unused resource END");
    }
    else {
        this->loadSprites();
    }
    
    // 运行中切换动画文件测试
//    CCLOG("switch aimation file BEGIN");
//    report_memory();
//    
//    CCObject* iter = NULL;
//    CCARRAY_FOREACH(_hSprites, iter) {
//        CCHierarchiesSprite* spr = dynamic_cast<CCHierarchiesSprite*>(iter);
//        spr->cleanup();
//        
//        spr->setSpriteFile("test_new_anim/hanim_test_lvmeng/lumengquan.hsheet", "test_new_anim/hanim_test_lvmeng/lumengquan.hanims");
//        
//        CCAction* action = CCRepeatForever::create(CCHierarchiesAnimate::createWholeAnimation(spr->getAnimationName()));
//        spr->runAction(action);
//    }
    
    report_memory();
    CCLOG("switch animation file END");
}

void CCHierarchiesSpriteFunctionalTestScene::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent) {
}

void CCHierarchiesSpriteFunctionalTestScene::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent) {
}

void CCHierarchiesSpriteFunctionalTestScene::ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent) {
}

#pragma mark - CCHierarchiesSpriteEventDelegate

void CCHierarchiesSpriteFunctionalTestScene::onEventContent (CCHierarchiesSprite* sprite, const char* eventContent) {
    // 动画事件测试
    CCLOG("on event content [%s] [%s]", sprite->getAnimationName(), eventContent);
}


void CCHierarchiesSpriteFunctionalTestScene::loadSprites () {
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    
    CCLOG("load sprites BEGIN");
    report_memory();
    
#define SPRITE_ARRAY_COL_COUNT (4)
#define SPRITE_ARRAY_ROW_COUNT (4)
    for (int i = 0; i < SPRITE_ARRAY_ROW_COUNT; i++) {
        for (int j = 1; j <= SPRITE_ARRAY_COL_COUNT; j++) {
            CCHierarchiesSprite* spr = NULL;
            
            switch (j % 4) {
                case 0: {
                    spr = CCHierarchiesSprite::create("test_new_anim/hanim_test_lvmeng/lumengquan.hsheet",
                                                      "test_new_anim/hanim_test_lvmeng/lumengquan.hanims",
                                                      this);
                    break;
                }
                case 1: {
                    spr = CCHierarchiesSprite::create("test_new_anim/hanim_test_taishici/taishiciquan.hsheet",
                                                      "test_new_anim/hanim_test_taishici/taishiciquan.hanims",
                                                      this);
                    spr->setOpacity(128);
                    break;
                }
                case 2: {
                    spr = CCHierarchiesSprite::create("test_new_anim/hanim_test_zhangfei/zhangfeiquan.hsheet",
                                                      "test_new_anim/hanim_test_zhangfei/zhangfeiquan.hanims",
                                                      this);
//                    spr = CCHierarchiesSprite::create("test_new_anim/hanim_test_xiahouyuan/xiahouyuanquan.hsheet",
//                                                      "test_new_anim/hanim_test_xiahouyuan/xiahouyuanquan.hanims",
//                                                      this);
                    spr->setFlipX(true);
                    break;
                }
                case 3: {
//                    spr = CCHierarchiesSprite::create("test_new_anim/hanim_test_baoxiang/chest_fx.hsheet",
//                                                      "test_new_anim/hanim_test_baoxiang/chest_fx.hanims",
//                                                      this);
                    spr = CCHierarchiesSprite::create("test_new_anim/hanim_test_zhangjiao/zhangjiaoquan.hsheet",
                                                      "test_new_anim/hanim_test_zhangjiao/zhangjiaoquan.hanims",
                                                      this);
                    spr->setColor(ccRED);
                    break;
                }
//                case 4: {
//                    spr = CCHierarchiesSprite::create("test_new_anim/hanim_test_guanyu/guanyuquan.hsheet",
//                                                      "test_new_anim/hanim_test_guanyu/guanyuquan.hanims",
//                                                      this);
//                    break;
//                }
            }
            
            spr->setPosition(ccp(winSize.width / (SPRITE_ARRAY_COL_COUNT + 1) * j, winSize.height / (SPRITE_ARRAY_ROW_COUNT + 1) * i));
            
            CCAction* action = NULL;
//            action = CCHierarchiesAnimate::createWholeAnimation(spr->getAnimationName());
            action = CCRepeatForever::create(CCHierarchiesAnimate::createWholeAnimation(spr->getAnimationName()));
            spr->runAction(action);
            
            spr->setZOrder((SPRITE_ARRAY_ROW_COUNT - i) * SPRITE_ARRAY_COL_COUNT + j);
            this->addChild(spr);
            
            _hSprites->addObject(spr);
        }
    }
    
    CCLOG("load sprites END");
    report_memory();
}
