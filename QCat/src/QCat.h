#pragma once

// For use by QCat application

#include "QCat/QCatCore.h"
#include "QCat/Application.h"
#include "QCat/Input.h"
#include "QCat/Layer/Layer.h"
#include "QCat/Log.h"
#include "QCat/ImGui/ImGuiLayer.h"


//--Renderer-----------------------------//
#include "Renderer/Renderer.h"
#include "Renderer/RenderCommand.h"

#include "Renderer/Buffer.h"
#include "Renderer/Shader.h"
#include "API/Opengl/OpenGLShader.h"
#include "API/DirectX11/DX11_Shader.h"
#include "Renderer/VertexArray.h"

#include "Renderer/OrthographicCamera.h"
//--Renderer-----------------------------//

// ---- Entry Point----//
#include "QCat//EntryPoint.h"
// --------------------//