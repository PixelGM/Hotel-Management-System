#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
// Notes:
	// Long Button
	// ImGui::Button("Room 1", ImVec2(150.0f, 0.0f));


class ExampleLayer : public Walnut::Layer
{
public:
	// Code here
	virtual void OnUIRender() override
	{
		bool showRoom1Window = false;
		float windowWidth = ImGui::GetWindowWidth();

		//ImGui::Begin("Hello");
		ImGui::Begin("Hotel A");

		// Set the cursor X position to center the button. (Set Margin)
		ImGui::SetCursorPosX(windowWidth / 2.0f - 200.0f);

		ImGui::Button("Room 1", ImVec2(75.0f, 0.0f));
		ImGui::SameLine();
		ImGui::Button("Room 2", ImVec2(75.0f, 0.0f));
		ImGui::SameLine();
		ImGui::Button("Room 3", ImVec2(75.0f, 0.0f));
		ImGui::SameLine();
		ImGui::Button("Room 4", ImVec2(75.0f, 0.0f));
		ImGui::SameLine();
		ImGui::Button("Room 5", ImVec2(75.0f, 0.0f));
		ImGui::SameLine();
		ImGui::Button("Room 6", ImVec2(75.0f, 0.0f));
		
		ImGui::SetCursorPosX(windowWidth / 2.0f - 200.0f);
		ImGui::Button("Room A", ImVec2(75.0f, 0.0f));
		ImGui::SameLine();
		ImGui::Button("Room B", ImVec2(75.0f, 0.0f));
		ImGui::SameLine();
		ImGui::Button("Room C", ImVec2(75.0f, 0.0f));
		ImGui::SameLine();
		ImGui::Button("Room D", ImVec2(75.0f, 0.0f));
		ImGui::SameLine();
		ImGui::Button("Room E", ImVec2(75.0f, 0.0f));
		ImGui::SameLine();
		ImGui::Button("Room F", ImVec2(75.0f, 0.0f));



		ImGui::End();

		//ImGui::Begin("Hotel 6");

		//ImGui::Begin("Dear ImGui Style Editor", &showRoom1Window);
	}
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Walnut Example";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}