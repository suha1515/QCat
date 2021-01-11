#include <QCat.h>

// ---- Entry Point----//
#include "QCat/Core/EntryPoint.h"
// --------------------//

#include "EditorLayer.h"

namespace QCat
{
	class QCatEditor : public Application
	{
	public:
		QCatEditor()
			:Application("QCat Editor")
		{
			//PushLayer(new ExamLayer());
			PushLayer(new EditorLayer());
		}
		~QCatEditor()
		{
		}
	};

	Application* CreateApplication()
	{
		return new QCatEditor();
	}
}