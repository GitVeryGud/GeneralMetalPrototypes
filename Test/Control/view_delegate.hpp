//
//  view_delegate.hpp
//  Test
//
//  Created by Gustavo Binder on 02/07/24.
//


#ifndef view_delegate_hpp
#define view_delegate_hpp

#include <Metal/Metal.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>
#include "View/renderer.hpp"

class MyMTKViewDelegate : public MTK::ViewDelegate
{
public:
    MyMTKViewDelegate( MTL::Device* pDevice );
    
    virtual ~MyMTKViewDelegate() override;
    virtual void drawInMTKView( MTK::View* pView ) override;

private:
    Renderer* _pRenderer;
};

#endif /* view_delegate_hpp */

