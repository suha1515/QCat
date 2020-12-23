#pragma once

#include "QCat/Core/QCatCore.h"
#include "QCat/Events/Event.h"
#include "QCat/Uitiliy/Timestep.h"
namespace QCat
{
	class QCAT_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach(){}
		virtual void OnDetach(){}
		virtual void OnUpdate(Timestep ts){}
		virtual void OnImGuiRender(){}
		virtual void OnEvent(Event& event){}

		inline const std::string& GetName() const { return m_DebugName; }
	private:
		std::string m_DebugName;
	};
}

