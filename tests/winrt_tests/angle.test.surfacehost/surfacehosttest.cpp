//
// Copyright (c) 2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
#include "pch.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace angle_test_surfacehost
{
    TEST_CLASS(SurfaceHostTests)
    {
    public:
        TEST_METHOD(InitializingSurfaceHostWithNullNativeWindowTypeFails)
        {
            rx::SurfaceHost host(nullptr);
            Assert::IsFalse(host.initialize());
        }
        TEST_METHOD(InitializingSurfaceHostWithInvalidIInspectableFails)
        {
            ComPtr<IAmInspectable> notCoreWindow = Make<GenericIInspectable>();
            rx::SurfaceHost host(notCoreWindow.Get());
            Assert::IsFalse(host.initialize());
        }
        TEST_METHOD(InitializingSurfaceHostWithNonIInspectableFails)
        {
            ComPtr<IAmUnknown> notIInspectable = Make<GenericIUnknown>();
            rx::SurfaceHost host((IInspectable*)notIInspectable.Get());
            Assert::IsFalse(host.initialize());
        }
        TEST_METHOD(InitializingSurfaceHostWithValidCoreWindowSucceeds)
        {
            ComPtr<ABI::Windows::UI::Core::ICoreWindow> coreWindow = Make<MockCoreWindow>();
            rx::SurfaceHost host(coreWindow.Get());
            Assert::IsTrue(host.initialize());
        }
        TEST_METHOD(InitializingSurfaceHostWithValidSwapChainPanelSucceeds)
        {
            ComPtr<ABI::Windows::UI::Xaml::Controls::ISwapChainPanel> swapChainPanel = Make<MockSwapChainPanel>();
            rx::SurfaceHost host(swapChainPanel.Get());
            Assert::IsTrue(host.initialize());
        }
        TEST_METHOD(CoreWindowSurfaceHostCreateSwapChainWithInvalidParamsFailsWithInvalidArg)
        {
            ComPtr<DXGISwapChain> swapChain;
            ComPtr<DXGIFactory> factory = Make<MockDXGIFactory>();
            ComPtr<ID3D11Device> device = Make<MockD3DDevice>();
            ComPtr<ABI::Windows::UI::Core::ICoreWindow> coreWindow = Make<MockCoreWindow>();
            rx::SurfaceHost host(coreWindow.Get());
            Assert::IsTrue(host.initialize());
            Assert::AreEqual(E_INVALIDARG, host.createSwapChain(nullptr, factory.Get(), DXGI_FORMAT_UNKNOWN, 1, 1, swapChain.GetAddressOf()), L"Unexpected success using null device");
            Assert::AreEqual(E_INVALIDARG, host.createSwapChain(device.Get(), nullptr, DXGI_FORMAT_UNKNOWN, 1, 1, swapChain.GetAddressOf()), L"Unexpected success using null factory");
            Assert::AreEqual(E_INVALIDARG, host.createSwapChain(device.Get(), factory.Get(), DXGI_FORMAT_UNKNOWN, 0, 1, swapChain.GetAddressOf()), L"Unexpected success using 0 width");
            Assert::AreEqual(E_INVALIDARG, host.createSwapChain(device.Get(), factory.Get(), DXGI_FORMAT_UNKNOWN, 1, 0, swapChain.GetAddressOf()), L"Unexpected success using 0 height");
            Assert::AreEqual(E_INVALIDARG, host.createSwapChain(device.Get(), factory.Get(), DXGI_FORMAT_UNKNOWN, 1, 1, nullptr), L"Unexpected success using null swapchain");
        }

        TEST_METHOD(SwapChainPanelSurfaceHostCreateSwapChainWithInvalidParamsFailsWithInvalidArg)
        {
            ComPtr<DXGISwapChain> swapChain;
            ComPtr<DXGIFactory> factory = Make<MockDXGIFactory>();
            ComPtr<ID3D11Device> device = Make<MockD3DDevice>();
            ComPtr<ABI::Windows::UI::Xaml::Controls::ISwapChainPanel> swapChainPanel = Make<MockSwapChainPanel>();
            rx::SurfaceHost host(swapChainPanel.Get());
            Assert::IsTrue(host.initialize());
            Assert::AreEqual(E_INVALIDARG, host.createSwapChain(nullptr, factory.Get(), DXGI_FORMAT_UNKNOWN, 1, 1, swapChain.GetAddressOf()), L"Unexpected success using null device");
            Assert::AreEqual(E_INVALIDARG, host.createSwapChain(device.Get(), nullptr, DXGI_FORMAT_UNKNOWN, 1, 1, swapChain.GetAddressOf()), L"Unexpected success using null factory");
            Assert::AreEqual(E_INVALIDARG, host.createSwapChain(device.Get(), factory.Get(), DXGI_FORMAT_UNKNOWN, 0, 1, swapChain.GetAddressOf()), L"Unexpected success using 0 width ");
            Assert::AreEqual(E_INVALIDARG, host.createSwapChain(device.Get(), factory.Get(), DXGI_FORMAT_UNKNOWN, 1, 0, swapChain.GetAddressOf()), L"Unexpected success using 0 height ");
            Assert::AreEqual(E_INVALIDARG, host.createSwapChain(device.Get(), factory.Get(), DXGI_FORMAT_UNKNOWN, 1, 1, nullptr), L"Unexpected success using null swapchain");
        }

        TEST_METHOD(SwapChainPanelSurfaceHostCreateSwapChainUsesValidDesc)
        {
            DXGI_SWAP_CHAIN_DESC1 expectedDesc = 
            {
                1, 1,                               // Width/Height
                DXGI_FORMAT_UNKNOWN,                // DXGI_FORMAT
                FALSE,                              // Stereo
                { 1, 0 },                           // DXGI_SAMPLE_DESC
                DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER,    // DXGI_USAGE
                2,                                  // BufferCount
                DXGI_SCALING_STRETCH,               // DXGI_SCALING
                DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL,   // DXGI_SWAP_EFFECT
                DXGI_ALPHA_MODE_IGNORE,             // DXGI_ALPHA_MODE
                0                                   // Flags
            };
            DXGI_SWAP_CHAIN_DESC1 desc;

            ComPtr<DXGISwapChain> swapChain;
            ComPtr<DXGIFactory> factory = Make<MockDXGIFactory>();
            ComPtr<ID3D11Device> device = Make<MockD3DDevice>();
            ComPtr<ABI::Windows::UI::Xaml::Controls::ISwapChainPanel> swapChainPanel = Make<MockSwapChainPanel>();
            rx::SurfaceHost host(swapChainPanel.Get());
            Assert::IsTrue(host.initialize());
            Assert::AreEqual(S_OK, host.createSwapChain(device.Get(), factory.Get(), DXGI_FORMAT_UNKNOWN, 1, 1, swapChain.GetAddressOf()));
            Assert::AreEqual(S_OK, swapChain->GetDesc1(&desc));
            Assert::AreEqual(expectedDesc.Width, desc.Width);
            Assert::AreEqual(expectedDesc.Height, desc.Height);
            Assert::AreEqual(expectedDesc.Stereo, desc.Stereo);
            Assert::AreEqual(expectedDesc.SampleDesc.Count, desc.SampleDesc.Count);
            Assert::AreEqual(expectedDesc.SampleDesc.Quality, desc.SampleDesc.Quality);
            Assert::AreEqual(expectedDesc.BufferUsage, desc.BufferUsage);
            Assert::AreEqual(expectedDesc.BufferCount, desc.BufferCount);
            Assert::AreEqual(expectedDesc.Flags, desc.Flags);
            // Compare enum values.  
            // Assert::AreEqual does not understand DirectX enum values and requires a custom ToString implementation.
            Assert::IsTrue(expectedDesc.Format == desc.Format, L"Unexpected DXGI_FORMAT");
            Assert::IsTrue(expectedDesc.Scaling == desc.Scaling, L"Unexpected DXGI_SCALING");
            Assert::IsTrue(expectedDesc.SwapEffect == desc.SwapEffect, L"Unexpected DXGI_SWAP_EFFECT");
            Assert::IsTrue(expectedDesc.AlphaMode == desc.AlphaMode, L"Unexpected DXGI_ALPHA_MODE");
        }

        TEST_METHOD(CoreWindowSurfaceHostCreateSwapChainUsesValidDesc)
        {
            DXGI_SWAP_CHAIN_DESC1 expectedDesc =
            {
                1, 1,                               // Width/Height
                DXGI_FORMAT_UNKNOWN,                // DXGI_FORMAT
                FALSE,                              // Stereo
                { 1, 0 },                           // DXGI_SAMPLE_DESC
                DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER,    // DXGI_USAGE
                2,                                  // BufferCount
                DXGI_SCALING_NONE,                  // DXGI_SCALING
                DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL,   // DXGI_SWAP_EFFECT
                DXGI_ALPHA_MODE_UNSPECIFIED,        // DXGI_ALPHA_MODE
                0                                   // Flags
            };
            DXGI_SWAP_CHAIN_DESC1 desc;

            ComPtr<DXGISwapChain> swapChain;
            ComPtr<DXGIFactory> factory = Make<MockDXGIFactory>();
            ComPtr<ID3D11Device> device = Make<MockD3DDevice>();
            ComPtr<ABI::Windows::UI::Core::ICoreWindow> coreWindow = Make<MockCoreWindow>();
            rx::SurfaceHost host(coreWindow.Get());
            Assert::IsTrue(host.initialize());
            Assert::AreEqual(S_OK, host.createSwapChain(device.Get(), factory.Get(), DXGI_FORMAT_UNKNOWN, 1, 1, swapChain.GetAddressOf()));
            Assert::AreEqual(S_OK, swapChain->GetDesc1(&desc));
            Assert::AreEqual(expectedDesc.Width, desc.Width);
            Assert::AreEqual(expectedDesc.Height, desc.Height);
            Assert::AreEqual(expectedDesc.Stereo, desc.Stereo);
            Assert::AreEqual(expectedDesc.SampleDesc.Count, desc.SampleDesc.Count);
            Assert::AreEqual(expectedDesc.SampleDesc.Quality, desc.SampleDesc.Quality);
            Assert::AreEqual(expectedDesc.BufferUsage, desc.BufferUsage);
            Assert::AreEqual(expectedDesc.BufferCount, desc.BufferCount);
            Assert::AreEqual(expectedDesc.Flags, desc.Flags);
            // Compare enum values.  
            // Assert::AreEqual does not understand DirectX enum values and requires a custom ToString implementation.
            Assert::IsTrue(expectedDesc.Format == desc.Format, L"Unexpected DXGI_FORMAT");
            Assert::IsTrue(expectedDesc.Scaling == desc.Scaling, L"Unexpected DXGI_SCALING");
            Assert::IsTrue(expectedDesc.SwapEffect == desc.SwapEffect, L"Unexpected DXGI_SWAP_EFFECT");
            Assert::IsTrue(expectedDesc.AlphaMode == desc.AlphaMode, L"Unexpected DXGI_ALPHA_MODE");
        }

        TEST_METHOD(CoreWindowSizeChangedEventIsUnregisteredOnDestruction)
        {
            ComPtr<MockCoreWindow> coreWindow = Make<MockCoreWindow>();
            {
                rx::SurfaceHost host(coreWindow.Get());
                Assert::IsTrue(host.initialize());
                Assert::AreEqual(1, coreWindow->GetSizeChangeRegistrationCount());
            }
            Assert::AreEqual(0, coreWindow->GetSizeChangeRegistrationCount());
        }
        TEST_METHOD(SwapChainPanelSizeChangedEventIsUnregisteredOnDestruction)
        {
            ComPtr<MockSwapChainPanel> swapChainPanel = Make<MockSwapChainPanel>();
            {
                ComPtr<ABI::Windows::UI::Xaml::Controls::ISwapChainPanel> panel;
                Assert::AreEqual(S_OK, swapChainPanel.As(&panel));
                Assert::IsNotNull(panel.Get());
                rx::SurfaceHost host(panel.Get());
                Assert::IsTrue(host.initialize());
                Assert::AreEqual(1, swapChainPanel->GetSizeChangeRegistrationCount());
            }
            Assert::AreEqual(0, swapChainPanel->GetSizeChangeRegistrationCount());
        }

        // If this test is run on Windows Phone devices, an expected LogicalDpi
        // value of 96.0f will be used because IDisplayPropertiesStatics::get_LogicalDpi
        // fails with HRESULT_FROM_WIN32(ERROR_NOT_FOUND).
        TEST_METHOD(CoreWindowCurrentBoundsIsQueriedOnInitialization)
        {
            ComPtr<MockCoreWindow> coreWindow = Make<MockCoreWindow>();
            ABI::Windows::Foundation::Rect expectedBounds = { 0, 0, 1024, 768 };
            coreWindow->SetExpectedBounds(expectedBounds);

            rx::SurfaceHost host(coreWindow.Get());
            Assert::IsTrue(host.initialize());
            Assert::AreEqual(1, coreWindow->GetBoundsQueryCount());

            RECT clientRect = { 0, 0, 0, 0 };
            RECT expectedRect = { 0, 0, 1024, 768 };
            Assert::IsTrue(host.getClientRect(&clientRect));
            Assert::IsTrue(clientRect == expectedRect);

            Assert::AreEqual(1, coreWindow->GetBoundsQueryCount());
        }
        TEST_METHOD(SwapChainPanelCurrentBoundsIsQueriedOnInitialization)
        {
            ComPtr<MockSwapChainPanel> swapChainPanel = Make<MockSwapChainPanel>();
            ABI::Windows::Foundation::Rect expectedBounds = { 0, 0, 1024, 768 };
            swapChainPanel->SetExpectedBounds(expectedBounds);

            ComPtr<ABI::Windows::UI::Xaml::Controls::ISwapChainPanel> panel;
            Assert::AreEqual(S_OK, swapChainPanel.As(&panel));
            Assert::IsNotNull(panel.Get());
            rx::SurfaceHost host(panel.Get());
            Assert::IsTrue(host.initialize());
            Assert::AreEqual(1, swapChainPanel->GetBoundsQueryCount());

            RECT clientRect = { 0, 0, 0, 0 };
            RECT expectedRect = { 0, 0, 1024, 768 };
            Assert::IsTrue(host.getClientRect(&clientRect));
            Assert::IsTrue(clientRect == expectedRect);

            Assert::AreEqual(1, swapChainPanel->GetBoundsQueryCount());
        }
        TEST_METHOD(CoreWindowSurfaceIsIconicAlwaysReturnsFalse)
        {
            ComPtr<ABI::Windows::UI::Core::ICoreWindow> coreWindow = Make<MockCoreWindow>();
            rx::SurfaceHost host(coreWindow.Get());
            Assert::IsTrue(host.initialize());
            Assert::IsFalse(host.isIconic());
        }
        TEST_METHOD(SwapChainPanelSurfaceIsIconicAlwaysReturnsFalse)
        {
            ComPtr<ABI::Windows::UI::Xaml::Controls::ISwapChainPanel> swapChainPanel = Make<MockSwapChainPanel>();
            rx::SurfaceHost host(swapChainPanel.Get());
            Assert::IsTrue(host.initialize());
            Assert::IsFalse(host.isIconic());
        }

        // If this test is run on Windows Phone devices, an expected LogicalDpi
        // value of 96.0f will be used because IDisplayPropertiesStatics::get_LogicalDpi
        // fails with HRESULT_FROM_WIN32(ERROR_NOT_FOUND).
        TEST_METHOD(CoreWindowSizeChangedEventSignalUpdatesClientRectToExpectedValue)
        {
            // Create core window with 800x600 bounds
            ComPtr<MockCoreWindow> coreWindow = Make<MockCoreWindow>();
            ABI::Windows::Foundation::Rect expectedBounds = { 0, 0, 800, 600 };
            coreWindow->SetExpectedBounds(expectedBounds);

            // Create surface, expecting to see 800x600 bounds
            rx::SurfaceHost host(coreWindow.Get());
            Assert::IsTrue(host.initialize());

            RECT clientRect = { 0, 0, 0, 0 };
            RECT expectedRect = { 0, 0, 800, 600 };
            Assert::IsTrue(host.getClientRect(&clientRect));
            Assert::IsTrue(clientRect == expectedRect);

            // Update core window to 1024x768 bounds, and cause a sizeChangedEvent to be signaled
            clientRect = { 0, 0, 0, 0 };
            expectedBounds = { 0, 0, 1024, 768 };
            coreWindow->UpdateSizeAndSignalChangedEvent(expectedBounds);

            expectedRect = { 0, 0, 1024, 768 };
            Assert::IsTrue(host.getClientRect(&clientRect));
            Assert::IsTrue(clientRect == expectedRect);
        }

        TEST_METHOD(SwapChainPanelSizeChangedEventSignalUpdatesClientRectToExpectedValue)
        {
            // Create swapchainpanel with 800x600 bounds
            ComPtr<MockSwapChainPanel> swapChainPanel = Make<MockSwapChainPanel>();
            ABI::Windows::Foundation::Rect expectedBounds = { 0, 0, 800, 600 };
            swapChainPanel->SetExpectedBounds(expectedBounds);

            // Create surface, expecting to see 800x600 bounds
            ComPtr<ABI::Windows::UI::Xaml::Controls::ISwapChainPanel> panel;
            Assert::AreEqual(S_OK, swapChainPanel.As(&panel));
            Assert::IsNotNull(panel.Get());
            rx::SurfaceHost host(panel.Get());
            Assert::IsTrue(host.initialize());

            RECT clientRect = { 0, 0, 0, 0 };
            RECT expectedRect = { 0, 0, 800, 600 };
            Assert::IsTrue(host.getClientRect(&clientRect));
            Assert::IsTrue(clientRect == expectedRect);

            // Update core window to 1024x768 bounds, and cause a sizeChangedEvent to be signaled
            clientRect = { 0, 0, 0, 0 };
            expectedBounds = { 0, 0, 1024, 768 };
            swapChainPanel->UpdateSizeAndSignalChangedEvent(expectedBounds);

            expectedRect = { 0, 0, 1024, 768 };
            Assert::IsTrue(host.getClientRect(&clientRect));
            Assert::IsTrue(clientRect == expectedRect);
        }
    };
}