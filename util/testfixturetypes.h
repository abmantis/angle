//
// Copyright (c) 2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#ifndef UTIL_TEST_FIXTURE_TYPES_H
#define UTIL_TEST_FIXTURE_TYPES_H

#include "EGLWindow.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>

template<EGLint platform, EGLint platformMajorVersion, EGLint platformMinorVersion, EGLint warp, EGLBoolean useRenderToBackBuffer>
struct Platform
{
    static EGLPlatformParameters GetPlatform()
    {
        return EGLPlatformParameters(platform, platformMajorVersion, platformMinorVersion, warp, useRenderToBackBuffer);
    }
};

// Typedefs of common platform types
#define DEFINE_ANGLE_TEST_PLATFORM(name, platform, majorVersion, minorVersion, useWarp, useRenderToBackBuffer) \
    struct name : public Platform<platform, majorVersion, minorVersion, useWarp, useRenderToBackBuffer> { }

DEFINE_ANGLE_TEST_PLATFORM(D3D9,              EGL_PLATFORM_ANGLE_TYPE_D3D9_ANGLE,  EGL_DONT_CARE, EGL_DONT_CARE, EGL_FALSE, EGL_FALSE);

DEFINE_ANGLE_TEST_PLATFORM(D3D11,             EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE, EGL_DONT_CARE, EGL_DONT_CARE, EGL_FALSE, EGL_FALSE);
DEFINE_ANGLE_TEST_PLATFORM(D3D11_FL11_0,      EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,            11,             0, EGL_FALSE, EGL_FALSE);
DEFINE_ANGLE_TEST_PLATFORM(D3D11_FL10_1,      EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,            10,             1, EGL_FALSE, EGL_FALSE);
DEFINE_ANGLE_TEST_PLATFORM(D3D11_FL10_0,      EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,            10,             0, EGL_FALSE, EGL_FALSE);
DEFINE_ANGLE_TEST_PLATFORM(D3D11_FL9_3,       EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,             9,             3, EGL_FALSE, EGL_TRUE);

DEFINE_ANGLE_TEST_PLATFORM(D3D11_WARP,        EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE, EGL_DONT_CARE, EGL_DONT_CARE,  EGL_TRUE, EGL_FALSE);
DEFINE_ANGLE_TEST_PLATFORM(D3D11_FL11_0_WARP, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,            11,             0,  EGL_TRUE, EGL_FALSE);
DEFINE_ANGLE_TEST_PLATFORM(D3D11_FL10_1_WARP, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,            10,             1,  EGL_TRUE, EGL_FALSE);
DEFINE_ANGLE_TEST_PLATFORM(D3D11_FL10_0_WARP, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,            10,             0,  EGL_TRUE, EGL_FALSE);
DEFINE_ANGLE_TEST_PLATFORM(D3D11_FL9_3_WARP,  EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,             9,             3,  EGL_TRUE, EGL_TRUE);

#undef DEFINE_ANGLE_TEST_PLATFORM

// Test Fixture Type
template<GLint GLESMajorVersion, typename platformT>
struct TestFixture
{
    static EGLint GetGlesMajorVersion()
    {
        return GLESMajorVersion;
    }

    static EGLPlatformParameters GetPlatform()
    {
        return platformT::GetPlatform();
    }
};

// Typedefs of common fixture types
typedef TestFixture<2, D3D9>              ES2_D3D9;

typedef TestFixture<2, D3D11>             ES2_D3D11;
typedef TestFixture<2, D3D11_FL11_0>      ES2_D3D11_FL11_0;
typedef TestFixture<2, D3D11_FL10_1>      ES2_D3D11_FL10_1;
typedef TestFixture<2, D3D11_FL10_0>      ES2_D3D11_FL10_0;
typedef TestFixture<2, D3D11_FL9_3>       ES2_D3D11_FL9_3;

typedef TestFixture<2, D3D11_WARP>        ES2_D3D11_WARP;
typedef TestFixture<2, D3D11_FL11_0_WARP> ES2_D3D11_FL11_0_WARP;
typedef TestFixture<2, D3D11_FL10_1_WARP> ES2_D3D11_FL10_1_WARP;
typedef TestFixture<2, D3D11_FL10_0_WARP> ES2_D3D11_FL10_0_WARP;
typedef TestFixture<2, D3D11_FL9_3_WARP>  ES2_D3D11_FL9_3_WARP;

typedef TestFixture<3, D3D11>             ES3_D3D11;
typedef TestFixture<3, D3D11_FL11_0>      ES3_D3D11_FL11_0;
typedef TestFixture<3, D3D11_FL10_1>      ES3_D3D11_FL10_1;
typedef TestFixture<3, D3D11_FL10_0>      ES3_D3D11_FL10_0;

typedef TestFixture<3, D3D11_WARP>        ES3_D3D11_WARP;
typedef TestFixture<3, D3D11_FL11_0_WARP> ES3_D3D11_FL11_0_WARP;
typedef TestFixture<3, D3D11_FL10_1_WARP> ES3_D3D11_FL10_1_WARP;
typedef TestFixture<3, D3D11_FL10_0_WARP> ES3_D3D11_FL10_0_WARP;

#define ANGLE_TYPED_TEST_CASE(testName, ...) \
    typedef ::testing::Types<__VA_ARGS__> Helper##testName; \
    TYPED_TEST_CASE(testName, Helper##testName);

#endif // UTIL_TEST_FIXTURE_TYPES_H
