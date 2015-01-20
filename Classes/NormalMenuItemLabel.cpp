#include "NormalMenuItemLabel.h"

NS_CC_BEGIN

const unsigned int    kZoomActionTag = 0xc0c05002;

NormalMenuItemLabel * NormalMenuItemLabel::create (Node * label, const ccMenuCallback & callback)
{
    NormalMenuItemLabel * ret = new NormalMenuItemLabel();
    ret->initWithLabel(label, callback);
    ret->autorelease();
    return ret;
}

NormalMenuItemLabel * NormalMenuItemLabel::create (Node * label)
{
    NormalMenuItemLabel * ret = new NormalMenuItemLabel();
    ret->initWithLabel(label, (const ccMenuCallback &) nullptr);
    ret->autorelease();
    return ret;
}

void NormalMenuItemLabel::activate()
{
    if(_enabled) MenuItem::activate();
}

void NormalMenuItemLabel::selected()
{
    if(_enabled)
    {
        MenuItem::selected();
        
        Action *action = getActionByTag(kZoomActionTag);
        if (action) this->stopAction(action);
        else _originalScale = this->getScale();
        
        Action *zoomAction = ScaleTo::create(0.05f, _originalScale * 1.2f);
        zoomAction->setTag(kZoomActionTag);
        this->runAction(zoomAction);
    }
}

void NormalMenuItemLabel::unselected()
{
    if(_enabled)
    {
        MenuItem::unselected();
        this->stopActionByTag(kZoomActionTag);
        Action * zoomAction = ScaleTo::create(0.05f, _originalScale);
        zoomAction->setTag(kZoomActionTag);
        this->runAction(zoomAction);
    }
}

NS_CC_END
