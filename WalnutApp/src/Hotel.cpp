#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"


#include <fstream>
#include "Walnut/json.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <climits>

using namespace std;

// Notes:
	// Long Button
	// ImGui::Button("Room 1", ImVec2(150.0f, 0.0f));

// Using the library's namespace for brevity
using json = nlohmann::json;

// Global JSON object to hold all client data
json allClients;


int levenshtein_distance(const string& s1, const string& s2);

void search_keyword(const string& keyword, const vector<string>& keywords, vector<string>& suggestions);

void SaveToFile(const char* roomFileName, const char* clientName, const char* clientID, const char* contactNumber, const char* email, const char* passportID, const char* paymentMethod);

void LoadFromFile(const char* roomFileName, char* clientNameBuffer, char* clientIDBuffer, char* contactNumberBuffer, char* emailBuffer, char* passportIDBuffer, char* paymentMethodBuffer);




// Function to calculate the Levenshtein distance between two strings
int levenshtein_distance(const string& s1, const string& s2) {
	int m = s1.size(), n = s2.size();
	vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));

	for (int i = 1; i <= m; ++i) dp[i][0] = i;
	for (int j = 1; j <= n; ++j) dp[0][j] = j;

	for (int i = 1; i <= m; ++i) {
		for (int j = 1; j <= n; ++j) {
			int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
			dp[i][j] = min({ dp[i - 1][j] + 1, dp[i][j - 1] + 1, dp[i - 1][j - 1] + cost });
		}
	}
	return dp[m][n];
}

// Function to search for a keyword in a list of keywords
void search_keyword(const string& keyword, const vector<string>& keywords) 
{
	int min_distance = INT_MAX;
	string closest_match;
	int exact_matches = 0;

	for (const auto& k : keywords) {
		int distance = levenshtein_distance(keyword, k);
		if (distance == 0) {
			++exact_matches;
			closest_match = k;
		}
		if (distance < min_distance) {
			min_distance = distance;
			closest_match = k;
		}
	}

	if (exact_matches == 1) {
		cout << "Exact match found: " << closest_match << endl;
	}
	else {
		cout << "Did you mean: " << closest_match << "?" << endl;
	}
}

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
				if (allClients.find(roomFileName) != allClients.end()) {
					strcpy(clientNameBuffer, allClients[roomFileName]["Client Name"].get<std::string>().c_str());
					strcpy(clientIDBuffer, allClients[roomFileName]["Client ID"].get<std::string>().c_str());
					strcpy(contactNumberBuffer, allClients[roomFileName]["Contact Number"].get<std::string>().c_str());
					strcpy(emailBuffer, allClients[roomFileName]["Email"].get<std::string>().c_str());
					strcpy(passportIDBuffer, allClients[roomFileName]["Passport/ID"].get<std::string>().c_str());
					strcpy(paymentMethodBuffer, allClients[roomFileName]["Payment Method"].get<std::string>().c_str());
				}
            }
        } catch (json::parse_error& e) {
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

	// UI Render
	virtual void OnUIRender() override
	{
		float windowWidth = ImGui::GetWindowWidth();
		char search[256] = "";

		ImGui::Begin("Hotel A");

		ImGui::SetCursorPosX(windowWidth / 2.0f - 500.0f);
		ImGui::InputText("Search", search, IM_ARRAYSIZE(search));

		vector<string> roomNames;
		for (const auto& info : roomInfos) {
			roomNames.push_back(info.second);
		}

		vector<string> suggestions;
		search_keyword(search, roomNames, suggestions);

		for (const auto& suggestion : suggestions) {
			ImGui::Text(suggestion.c_str());
		}

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

	std::unique_ptr<Walnut::Application> app = std::make_unique<Walnut::Application>(spec);
	Walnut::Application* rawApp = app.get();

	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([rawApp]()
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit"))
				{
					rawApp->Close();
				}
				ImGui::EndMenu();
			}
		});
	return app.release();
}