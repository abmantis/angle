//
// Copyright (c) 2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
#include "pch.h"

#include "EGLWrapper.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace angle_test_fl9
{
    TEST_CLASS(FL9Tests)
    {
    public:

        // Helper that checks for D3D_FEATURE_LEVEL_9_* by looking for the a hint in the renderer string.
        bool findFL9RendererString()
        {
            std::string rendererString = std::string((char*)glGetString(GL_RENDERER));
            std::transform(rendererString.begin(), rendererString.end(), rendererString.begin(), ::tolower);
            return (rendererString.find(std::string("level_9")) != std::string::npos);
        }

        // Functional test to ensure that the renderer string is correct on all feature levels.
        TEST_METHOD(RendererStringTest)
        {
            EGLWrapper egl;

            egl.InitializeOffscreenSurfaceEGL(EGL_DEFAULT_DISPLAY);

            std::string rendererString = std::string((char*)glGetString(GL_RENDERER));
            std::transform(rendererString.begin(), rendererString.end(), rendererString.begin(), ::tolower);
           
            // Ensure that "software adapter" isn't in the renderer string.
            // This could occur in three scenarios:
            //   1) ANGLE is running on D3D_FEATURE_LEVEL_9_*, is using D3D10Level9, and is misconfigured
            //      (e.g. uses IDXGIAdapter::GetDesc rather than IDXGIAdapter2::GetDesc2)
            //   2) ANGLE is running on D3D_FEATURE_LEVEL_9_*, is using D3D10Level9, and is running on OS without DXGI1.2
            //   3) ANGLE is using D3D_DRIVER_TYPE_SOFTWARE to create the device
            // We can ignore scenario 2 (since this is a Windows Store unit test, so DXGI1.2 is present), 
            // and scenario 3 (since ANGLE doesn't use D3D_DRIVER_TYPE_SOFTWARE).
            Assert::AreEqual(rendererString.find(std::string("software adapter")), std::string::npos);
            
            // Cleanup.
            egl.CleanupEGL();
        }

        // Point Sprite Compilation Test Vertex Shader source
        const std::string pointSpriteCompilationTestVS = SHADER_SOURCE
        (
            void main()
            {
                gl_PointSize = 100.0;
                gl_Position = vec4(1.0, 0.0, 0.0, 1.0);
            }
        );

        // Point Sprite Compilation Test Fragment Shader source
        const std::string pointSpriteCompilationTestFS = SHADER_SOURCE
        (
            precision mediump float;
            void main()
            {
                gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
            }
        );

        // On D3D_FEATURE_LEVEL_9_*, compilation of shaders using point sprites should fail.
        TEST_METHOD(PointSpriteCompilationTestFL9)
        {
            EGLWrapper egl;

            // Shader compilation needs a valid current EGL context.
            egl.InitializeOffscreenSurfaceEGL(EGL_D3D11_FL9_3_ONLY_DISPLAY_ANGLE);

            // Check for D3D_FEATURE_LEVEL_9_* by looking for the hint in the renderer string.
            Assert::IsTrue(findFL9RendererString());

            // Compiling the shaders should succeed. It's linking them that should fail.
            GLuint program = CompileShadersIntoProgram(pointSpriteCompilationTestVS, pointSpriteCompilationTestFS);
            Assert::AreNotEqual((int)program, 0);

            // Linking should fail on D3D_FEATURE_LEVEL_9_*.
            GLint linkStatus;
            glLinkProgram(program);
            glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
            Assert::AreEqual(linkStatus, 0);

            // If the compilation failed, then check that it failed for the expected reason.
            // Do this by checking that the error log mentions Geometry Shaders.
            GLint infoLogLength;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
            std::vector<GLchar> infoLog(infoLogLength);
            glGetProgramInfoLog(program, (GLsizei)infoLog.size(), NULL, infoLog.data());

            std::string logString = std::string(infoLog.begin(), infoLog.end());
            std::transform(logString.begin(), logString.end(), logString.begin(), ::tolower);
            Assert::IsTrue(logString.find(std::string("geometry shader")) != std::string::npos);

            // Cleanup.
            glDeleteProgram(program);
            egl.CleanupEGL();
        }

        // On D3D_FEATURE_LEVEL_10_0+, compilation of shaders using point sprites should succeed.
        TEST_METHOD(PointSpriteCompilationTestFL10Plus)
        {
            EGLWrapper egl;

            // Shader compilation needs a valid current EGL context.
            egl.InitializeOffscreenSurfaceEGL(EGL_DEFAULT_DISPLAY);

            // If this is being run on a FL9 device (e.g. a Windows Phone 8.0-era Phone) then skip the test.
            if (findFL9RendererString())
            {
                return;
            }

            // Compiling the shaders should succeed.
            GLuint program = CompileShadersIntoProgram(pointSpriteCompilationTestVS, pointSpriteCompilationTestFS);
            Assert::AreNotEqual((int)program, 0);

            // Linking should pass on FL10_0+.
            GLint linkStatus;
            glLinkProgram(program);
            glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
            Assert::AreNotEqual(linkStatus, 0);

            // Cleanup.
            glDeleteProgram(program);
            egl.CleanupEGL();
        }

        // On D3D_FEATURE_LEVEL_9_*, calls to Transform Feedback methods should fail.
        TEST_METHOD(TransformFeedbackTest)
        {
            EGLWrapper egl;

            // The API require a valid current EGL context. Force it to be D3D_FEATURE_LEVEL_9_3.
            egl.InitializeOffscreenSurfaceEGL(EGL_D3D11_FL9_3_ONLY_DISPLAY_ANGLE);
            Assert::AreEqual((int)glGetError(), GL_NO_ERROR);

            // Check for D3D_FEATURE_LEVEL_9_* by looking for the hint in the renderer string.
            Assert::IsTrue(findFL9RendererString());

            glGetIntegeri_v(GL_TRANSFORM_FEEDBACK_BUFFER_START, 0, NULL);
            Assert::AreEqual((int)glGetError(), GL_INVALID_OPERATION);

            glGetIntegeri_v(GL_TRANSFORM_FEEDBACK_BUFFER_SIZE, 0, NULL);
            Assert::AreEqual((int)glGetError(), GL_INVALID_OPERATION);

            glGetIntegeri_v(GL_TRANSFORM_FEEDBACK_BUFFER_BINDING, 0, NULL);
            Assert::AreEqual((int)glGetError(), GL_INVALID_OPERATION);

            glGetInteger64i_v(GL_TRANSFORM_FEEDBACK_BUFFER_START, 0, NULL);
            Assert::AreEqual((int)glGetError(), GL_INVALID_OPERATION);

            glGetInteger64i_v(GL_TRANSFORM_FEEDBACK_BUFFER_SIZE, 0, NULL);
            Assert::AreEqual((int)glGetError(), GL_INVALID_OPERATION);

            glGetInteger64i_v(GL_TRANSFORM_FEEDBACK_BUFFER_BINDING, 0, NULL);
            Assert::AreEqual((int)glGetError(), GL_INVALID_OPERATION);

            glBeginTransformFeedback(GL_TRIANGLES);
            Assert::AreEqual((int)glGetError(), GL_INVALID_OPERATION);

            glBeginTransformFeedback(GL_LINES);
            Assert::AreEqual((int)glGetError(), GL_INVALID_OPERATION);

            glBeginTransformFeedback(GL_POINTS);
            Assert::AreEqual((int)glGetError(), GL_INVALID_OPERATION);

            glEndTransformFeedback();
            Assert::AreEqual((int)glGetError(), GL_INVALID_OPERATION);

            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);
            Assert::AreEqual((int)glGetError(), GL_INVALID_OPERATION);

            glTransformFeedbackVaryings(0, 0, NULL, GL_INTERLEAVED_ATTRIBS);
            Assert::AreEqual((int)glGetError(), GL_INVALID_OPERATION);

            glTransformFeedbackVaryings(0, 0, NULL, GL_SEPARATE_ATTRIBS);
            Assert::AreEqual((int)glGetError(), GL_INVALID_OPERATION);

            glGetTransformFeedbackVarying(0, 0, 0, NULL, NULL, NULL, NULL);
            Assert::AreEqual((int)glGetError(), GL_INVALID_VALUE);

            glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
            Assert::AreEqual((int)glGetError(), GL_INVALID_OPERATION);

            glDeleteTransformFeedbacks(0, NULL);
            Assert::AreEqual((int)glGetError(), GL_INVALID_OPERATION);

            glGenTransformFeedbacks(0, NULL);
            Assert::AreEqual((int)glGetError(), GL_INVALID_OPERATION);

            glPauseTransformFeedback();
            Assert::AreEqual((int)glGetError(), GL_INVALID_OPERATION);

            glResumeTransformFeedback();
            Assert::AreEqual((int)glGetError(), GL_INVALID_OPERATION);

            // Cleanup.
            egl.CleanupEGL();
        }
    };
}