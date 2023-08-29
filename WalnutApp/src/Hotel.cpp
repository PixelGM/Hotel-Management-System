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

	// Notification
	bool showSaveNotification = false;
	float notificationDuration = 1.0f;  // Display notification for 1 second
	float notificationTimer = 0.0f;

	const char* filename;
	bool showRoomWindow = false;
	bool firstOpen = true;
	char clientName[256] = "";
	char clientID[256] = "";

public:
	Room(const char* roomFileName) : filename(roomFileName) {}

	bool IsWindowOpen() const { return showRoomWindow; }
	void SetWindowOpenState(bool openState) { showRoomWindow = openState; }

	void DrawRoomWindow(const char* roomTitle, std::function<void()> saveStatesCallback)
	{
		if (showRoomWindow)
		{
			if (!ImGui::Begin(roomTitle, &showRoomWindow))
			{
				ImGui::End();
				return;
			}

			if (firstOpen)
			{
				LoadFromFile(filename, clientName, clientID);
				firstOpen = false;
			}

			ImGui::InputText("Insert Client Name", clientName, IM_ARRAYSIZE(clientName));
			ImGui::InputText("Insert Client ID", clientID, IM_ARRAYSIZE(clientID));

			if (ImGui::Button("Save Data"))
			{
				SaveToFile(filename, clientName, clientID);
				showSaveNotification = true; // Set the flag when data is saved
				notificationTimer = 0.0f;   // Reset the timer
			}

			// Handle the save notification
			if (showSaveNotification)
			{
				ImGui::Text("Saved!");
				notificationTimer += ImGui::GetIO().DeltaTime; // Increment the timer

				if (notificationTimer > notificationDuration)
				{
					showSaveNotification = false; // Turn off notification after the desired duration
				}
			}

			/*if (ImGui::Button("Manually Load Data"))
			{
				LoadFromFile(filename, clientName, clientID);
			}*/

			ImGui::End();

			if (!showRoomWindow)  // This means the window was closed during this frame
			{
				saveStatesCallback();
			}
		}
		else
		{
			firstOpen = true;
		}
	}

	void ToggleRoomWindowVisibility()
	{
		showRoomWindow = !showRoomWindow;
	}
};


// int main()
class ExampleLayer : public Walnut::Layer
{
private:
	// This is the single line you need to modify when adding a new room.
	// Just add a new entry into the roomInfos vector.
	std::vector<std::pair<const char*, std::string>> roomInfos = {
		{"clientDataRoom1.json", "Hotel A Box - Room 1"},
		{"clientDataRoom2.json", "Hotel A Box - Room 2"},
		{"clientDataRoom3.json", "Hotel A Box - Room 3"}
		// Add more rooms here...
	};

	std::vector<Room> rooms;
	const char* roomsStateFilename = "roomStates.json";
	

public:
	ExampleLayer()
	{
		for (const auto& info : roomInfos)
		{
			rooms.emplace_back(info.first);
		}

		LoadAllRoomStates();
	}

	void SaveAllRoomStates()
	{
		json j;
		j["room1"] = rooms[0].IsWindowOpen();
		j["room2"] = rooms[1].IsWindowOpen();
		j["room3"] = rooms[2].IsWindowOpen();

		std::ofstream file(roomsStateFilename);
		if (file.is_open())
		{
			file << j.dump();
			file.close();
		}
	}

	void LoadAllRoomStates()
	{
		std::ifstream file(roomsStateFilename);
		if (file.is_open())
		{
			json j;
			file >> j;

			for (size_t i = 0; i < rooms.size(); i++)
			{
				rooms[i].SetWindowOpenState(j["room" + std::to_string(i + 1)].get<bool>());
			}

			file.close();
		}
	}

	void DrawRoomWindowFor(Room& room, const std::string& title)
	{
		room.DrawRoomWindow(title.c_str(), [this]() { SaveAllRoomStates(); });
	}

	virtual void OnUIRender() override
	{
		float windowWidth = ImGui::GetWindowWidth();
		ImGui::Begin("Hotel A");

		for (size_t i = 0; i < rooms.size(); ++i)
		{
			ImGui::SetCursorPosX(windowWidth / 2.0f - (rooms.size() * 75.0f / 2.0f) + (i * 75.0f));

			if (ImGui::Button(("Room " + std::to_string(i + 1)).c_str(), ImVec2(75.0f, 0.0f)))
			{
				rooms[i].ToggleRoomWindowVisibility();
				SaveAllRoomStates();
			}

			if (i != rooms.size() - 1)
				ImGui::SameLine();
		}

		ImGui::End();

		for (size_t i = 0; i < rooms.size(); ++i)
		{
			rooms[i].DrawRoomWindow(roomInfos[i].second.c_str(), [this]() { SaveAllRoomStates(); });
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