#pragma once

// For use by QCat application
//defines
#include "QCatDefines.h"

#include "QCat/Core/QCatCore.h"
#include "QCat/Core/Application.h"
#include "QCat/Core/Input.h"
#include "QCat/Layer/Layer.h"
#include "QCat/Core/Log.h"
#include "QCat/Core/Assert.h"

#include "QCat/Uitiliy/Timestep.h"

#include "QCat/Core/Input.h"
#include "QCat/Core/KeyCode.h"
#include "QCat/OrthographicCameraController.h"


#include "QCat/ImGui/ImGuiLayer.h"
#include "QCat/Scene/Scene.h"
#include "QCat/Scene/Entity.h"
#include "QCat/Scene/ScriptableEntity.h"
#include "QCat/Scene/Components.h"
//--Utility------------------------------//
#include "QCat/Uitiliy/Math.h"
//---------------------------------------//

//--Renderer-----------------------------//
#include "QCat/Renderer/enums.h"
#include "QCat/Renderer/Renderer.h"
#include "QCat/Renderer/Renderer2D.h"
#include "QCat/Renderer/RenderCommand.h"

#include "QCat/Renderer/Buffer.h"
#include "QCat/Renderer/Shader.h"
#include "QCat/Renderer/Texture.h"
#include "QCat/Renderer/FrameBuffer.h"
#include "QCat/Renderer/FrameBufferEx.h"
#include "QCat/Renderer/VertexArray.h"
#include "QCat/Renderer/OrthographicCamera.h"
#include "QCat/Renderer/SamplerState.h"
#include "QCat/Renderer/Mesh.h"
//--RenderGraph & Pass-----------------------------//
#include "QCat/RenderGraph/RenderGraph.h"
#include "QCat/RenderGraph/Pass.h"
//--API----------------------------------//
#include "API/Opengl/OpenGLShader.h"
#include "API/DirectX11/DX11_Shader.h"
#include "API/DirectX11/DX11_Buffer.h"
#include "API/DirectX11/DX11_Blender.h"
//---------------------------------------//

//lib//
#include <queue>


