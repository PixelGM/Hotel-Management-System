#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"


#include <fstream>
#include "Walnut/json.hpp"

// Notes:
	// Long Button
	// ImGui::Button("Room 1", ImVec2(150.0f, 0.0f));

// Using the library's namespace for brevity
using json = nlohmann::json;

void SaveToFile(const char* filename, const char* clientName, const char* clientID) {
	json j;
	j["Client Name"] = clientName;
	j["Client ID"] = clientID;

	std::ofstream file(filename);
	if (file.is_open()) {
		file << j.dump(); // Convert the JSON object to a string and write it to the file
		file.close();
	}
}

void LoadFromFile(const char* filename, char* clientNameBuffer, char* clientIDBuffer) {
	std::ifstream file(filename);
	if (file.is_open()) {
		json j;
		file >> j; // Load the JSON object from the file

		strcpy(clientNameBuffer, j["Client Name"].get<std::string>().c_str());
		strcpy(clientIDBuffer, j["Client ID"].get<std::string>().c_str());

		file.close();
	}
}


class ExampleLayer : public Walnut::Layer
{
private:
	bool showRoom1Window = false;
	char clientName[256] = "";
	char clientID[256] = "";
public:
	// Code here
	virtual void OnUIRender() override
	{
		float windowWidth = ImGui::GetWindowWidth();

		//ImGui::Begin("Hello");
		ImGui::Begin("Hotel A");

		// Set the cursor X position to center the button. (Set Margin)
		ImGui::SetCursorPosX(windowWidth / 2.0f - 200.0f);

		if (ImGui::Button("Room 1", ImVec2(75.0f, 0.0f)))
		{
			showRoom1Window = true;
		}
		
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

		const char* filename = "clientData.json";
		if (showRoom1Window)
		{
			if (!ImGui::Begin("Hotel A Box", &showRoom1Window)) // Allow closing of the new window by pressing its close button
			{
				ImGui::End();
				return;
			}

			// You can put content for this window here.
			/*ImGui::Text("Client Name: %s", dBuf);

			if (ImGui::InputText("Insert Client Name", buf, IM_ARRAYSIZE(buf)));

			if (ImGui::Button("Set"))
			{
				strcpy(dBuf, buf);
				SaveToFile(filename, dBuf);
			}



			ImGui::Text("Client ID: %s", dBuf2);

			if (ImGui::InputText("Insert Client ID", buf2, IM_ARRAYSIZE(buf)));

			if (ImGui::Button("Set"))
			{
				strcpy(dBuf2, buf2);
				SaveToFile(filename, dBuf2);
			}*/

			if (ImGui::InputText("Insert Client Name", clientName, IM_ARRAYSIZE(clientName)));
			if (ImGui::InputText("Insert Client ID", clientID, IM_ARRAYSIZE(clientID)));

			if (ImGui::Button("Save Data")) {
				SaveToFile(filename, clientName, clientID);
			}

			if (ImGui::Button("Load Data")) {
				LoadFromFile(filename, clientName, clientID);
			}


			ImGui::End();
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