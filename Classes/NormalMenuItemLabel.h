/*
 * Тот же MenuItemLabel, но с некоторыми переопределенными методами.
 */

#ifndef NORMALMENUITEMLABEL_H
#define NORMALMENUITEMLABEL_H

#include "cocos2d.h"
NS_CC_BEGIN

class NormalMenuItemLabel : public MenuItemLabel
{
public:
    static NormalMenuItemLabel * create (Node * label, const ccMenuCallback & callback);
    static NormalMenuItemLabel * create (Node * label);
    
    virtual void activate () override;
    virtual void selected () override;
    virtual void unselected () override;
    
protected:
    NormalMenuItemLabel () = default;
    virtual ~NormalMenuItemLabel () = default;
    
private:
    NormalMenuItemLabel (const NormalMenuItemLabel &) = delete;
    NormalMenuItemLabel (const NormalMenuItemLabel &&) = delete;
    NormalMenuItemLabel & operator= (const NormalMenuItemLabel &) = delete;
    NormalMenuItemLabel & operator= (const NormalMenuItemLabel &&) = delete;
};

NS_CC_END

#endif // NORMALMENUITEMLABEL_H
