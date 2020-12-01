#pragma once

#include "QCat/QCatCore.h"
#include "QCat/Events/Event.h"

namespace QCat
{
	class QCAT_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach(){}
		virtual void OnDetach(){}
		virtual void OnBegin(){}
		virtual void OnUpdate(){}
		virtual void OnRende(){};
		virtual void OnEvent(Event& event){}

		inline const std::string& GetName() const { return m_DebugName; }
	private:
		std::string m_DebugName;
	};
}

