//
//  app_delegate.hpp
//  Test
//
//  Created by Gustavo Binder on 02/07/24.
//

#ifndef app_delegate_hpp
#define app_delegate_hpp

#include <Metal/Metal.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>
#include "view_delegate.hpp"

class MyAppDelegate : public NS::ApplicationDelegate
{
    public:
        ~MyAppDelegate();
        virtual void applicationDidFinishLaunching( NS::Notification* pNotification ) override;
        virtual bool applicationShouldTerminateAfterLastWindowClosed( NS::Application* pSender ) override;

    private:
        NS::Window* _pWindow;
        MTK::View* _pMtkView;
        MTL::Device* _pDevice;
        MyMTKViewDelegate* _pViewDelegate = nullptr;
};

#endif /* app_delegate_hpp */
