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


class Room
{
private:
	const char* filename;
	bool showRoomWindow = false;
	bool firstOpen = true;
	char clientName[256] = "";
	char clientID[256] = "";

public:
	Room(const char* roomFileName) : filename(roomFileName) {}

	void DrawRoomWindow(const char* roomTitle)
	{
		if (showRoomWindow)
		{
			if (!ImGui::Begin(roomTitle, &showRoomWindow)) // Allow closing of the window by pressing its close button
			{
				ImGui::End();
				return;
			}

			if (firstOpen)
			{
				LoadFromFile(filename, clientName, clientID);
				firstOpen = false; // Set the flag to false so we don't load again unless the window is reopened
			}

			if (ImGui::InputText("Insert Client Name", clientName, IM_ARRAYSIZE(clientName)));
			if (ImGui::InputText("Insert Client ID", clientID, IM_ARRAYSIZE(clientID)));

			if (ImGui::Button("Save Data"))
			{
				SaveToFile(filename, clientName, clientID);
			}

			if (ImGui::Button("Manually Load Data"))
			{
				LoadFromFile(filename, clientName, clientID);
			}

			ImGui::End();
		}
		else
		{
			firstOpen = true; // Reset the flag when the window is closed
		}
	}

	void ToggleRoomWindowVisibility()
	{
		showRoomWindow = !showRoomWindow;
	}
};


class ExampleLayer : public Walnut::Layer
{
private:
	Room room1{ "clientDataRoom1.json" }; // Now, you can easily instantiate other rooms like room2, room3...
	Room room2{ "clientDataRoom2.json"};
	Room room3{ "clientDataRoom3.json"};
	//...

public:
	virtual void OnUIRender() override
	{
		float windowWidth = ImGui::GetWindowWidth();
		ImGui::Begin("Hotel A");

		// Set the cursor X position to center the button. (Set Margin)
		ImGui::SetCursorPosX(windowWidth / 2.0f - 200.0f);

		if (ImGui::Button("Room 1", ImVec2(75.0f, 0.0f))) { room1.ToggleRoomWindowVisibility(); }
		ImGui::SameLine();
		if (ImGui::Button("Room 2", ImVec2(75.0f, 0.0f))) { room2.ToggleRoomWindowVisibility(); }
		ImGui::SameLine();
		if (ImGui::Button("Room 3", ImVec2(75.0f, 0.0f))) { room3.ToggleRoomWindowVisibility(); }

		// Here you can easily integrate other rooms' buttons and windows as needed...

		ImGui::End();

		room1.DrawRoomWindow("Hotel A Box - Room 1");
		room2.DrawRoomWindow("Hotel A Box - Room 2");
		room3.DrawRoomWindow("Hotel A Box - Room 3");
		//roomX.DrawRoomWindow("Hotel A Box - Room X");
		//...
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