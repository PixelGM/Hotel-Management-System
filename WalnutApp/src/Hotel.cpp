#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"


#include <fstream>
#include "Walnut/json.hpp"
#include <iostream>

// Notes:
	// Long Button
	// ImGui::Button("Room 1", ImVec2(150.0f, 0.0f));

// Using the library's namespace for brevity
using json = nlohmann::json;

// Global JSON object to hold all client data
json allClients;

void SaveToFile(const char* roomFileName, const char* clientName, const char* clientID, const char* contactNumber, const char* email, const char* passportID, const char* paymentMethod) {
	allClients[roomFileName]["Client Name"] = clientName;
	allClients[roomFileName]["Client ID"] = clientID;
	allClients[roomFileName]["Contact Number"] = contactNumber;
	allClients[roomFileName]["Email"] = email;
	allClients[roomFileName]["Passport/ID"] = passportID;
	allClients[roomFileName]["Payment Method"] = paymentMethod;

	std::ofstream file("AllClients.json");
	if (file.is_open()) {
		file << allClients.dump();
		file.close();
	}
}

void LoadFromFile(const char* roomFileName, char* clientNameBuffer, char* clientIDBuffer, char* contactNumberBuffer, char* emailBuffer, char* passportIDBuffer, char* paymentMethodBuffer) {
	std::ifstream file("AllClients.json");
	if (file.is_open()) {
		try {
			file >> allClients;
			if (allClients.find(roomFileName) != allClients.end()) {
				json roomData = allClients[roomFileName];
				if (roomData.find("Client Name") != roomData.end()) {
					strcpy(clientNameBuffer, roomData["Client Name"].get<std::string>().c_str());
				}
				// ... (similar checks for other fields)
			}
		}
		catch (json::parse_error& e) {
			// Handle exceptions here
			std::cerr << "JSON parse error: " << e.what() << '\n';
		}
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
	char contactNumber[256] = "";
	char email[256] = "";
	char passportID[256] = "";
	char paymentMethod[256] = "";

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
				LoadFromFile(filename, clientName, clientID, contactNumber, email, passportID, paymentMethod);
				firstOpen = false;
			}

			ImGui::InputText("Client Name", clientName, IM_ARRAYSIZE(clientName));
			ImGui::InputText("Client ID", clientID, IM_ARRAYSIZE(clientID));
			ImGui::InputText("Contact Number", contactNumber, IM_ARRAYSIZE(contactNumber));
			ImGui::InputText("Email", email, IM_ARRAYSIZE(email));
			ImGui::InputText("Passport/ID", passportID, IM_ARRAYSIZE(passportID));
			ImGui::InputText("Payment Method", paymentMethod, IM_ARRAYSIZE(paymentMethod));

			if (ImGui::Button("Save Data"))
			{
				SaveToFile(filename, clientName, clientID, contactNumber, email, passportID, paymentMethod);
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
	// Add Rooms here
	std::vector<std::pair<const char*, std::string>> roomInfos = {
		{"Room 1", "Hotel A Box - Room 1"},
		{"Room 2", "Hotel A Box - Room 2"},
		{"Room 3", "Hotel A Box - Room 3"},
		{"Room 4", "Hotel A Box - Room 4"},
		{"Room 5", "Hotel A Box - Room 5"},
		{"Room 6", "Hotel A Box - Room 6"},
		{"Room 7", "Hotel A Box - Room 7"},
		{"Room 8", "Hotel A Box - Room 8"},
		{"Room 9", "Hotel A Box - Room 9"}
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

		for (size_t i = 0; i < rooms.size(); ++i)
		{
			j["room" + std::to_string(i + 1)] = rooms[i].IsWindowOpen();
		}

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

			for (size_t i = 0; i < rooms.size(); ++i)
			{
				if (j.find("room" + std::to_string(i + 1)) != j.end())  // check if key exists
				{
					rooms[i].SetWindowOpenState(j["room" + std::to_string(i + 1)].get<bool>());
				}
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