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
        float windowWidth = ImGui::GetWindowWidth();

        // First Window: Hotel A
        ImGui::Begin("Hotel A");
        ImGui::SetCursorPosX(windowWidth / 2.0f - 200.0f);
        RenderRoomButtons();
        ImGui::End();

        // Second Window: Hotel B
        ImGui::Begin("Hotel B");
        ImGui::SetCursorPosX(windowWidth / 2.0f - 200.0f);
        RenderRoomButtons();
        ImGui::End();
    }

    void RenderRoomButtons()
    {
        const char* rooms[] = { "Room 1", "Room 2", "Room 3", "Room 4", "Room 5", "Room 6",
                               "Room A", "Room B", "Room C", "Room D", "Room E", "Room F" };
        for (int i = 0; i < 12; ++i)
        {
            if (i != 0 && i != 6) ImGui::SameLine();
            ImGui::Button(rooms[i], ImVec2(75.0f, 0.0f));
        }
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