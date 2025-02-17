#include "LeagueMemoryReader.h"
#include "windows.h"
#include "Utils.h"
#include "Structs.h"
#include <iostream>
#include <fstream>
#include <string>
#include "psapi.h"
#include <limits>
#include <stdexcept>

LeagueMemoryReader::LeagueMemoryReader()
{
	// Some trash object not worth reading
	blacklistedObjectNames.insert("testcube");
	blacklistedObjectNames.insert("testcuberender");
	blacklistedObjectNames.insert("testcuberender10vision");
	blacklistedObjectNames.insert("s5test_wardcorpse");
	blacklistedObjectNames.insert("sru_camprespawnmarker");
	blacklistedObjectNames.insert("sru_plantrespawnmarker");
	blacklistedObjectNames.insert("preseason_turret_shield");
}

bool LeagueMemoryReader::IsLeagueWindowActive() {
	HWND handle = GetForegroundWindow();

	DWORD h;
	GetWindowThreadProcessId(handle, &h);
	return pid == h;
}

bool LeagueMemoryReader::IsHookedToProcess() {
	return Process::IsProcessRunning(pid);
}

void LeagueMemoryReader::HookToProcess() {

	// Find the window
	hWindow = FindWindowA("RiotWindowClass", NULL);
	if (hWindow == NULL) {
		throw WinApiException("League window not found");
	}

	// Get the process ID
	GetWindowThreadProcessId(hWindow, &pid);
	if (pid == NULL) {
		throw WinApiException("Couldn't retrieve league process id");
	}

	// Open the process
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (hProcess == NULL) {
		throw WinApiException("Couldn't open league process");
	}

	// Check architecture
	if (0 == IsWow64Process(hProcess, &is64Bit)) {
		throw WinApiException("Failed to identify if process has 32 or 64 bit architecture");
	}

	HMODULE hMods[1024];
	DWORD cbNeeded;
	if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded)) {
		moduleBaseAddr = (DWORD_PTR)hMods[0];
	}
	else {
		throw WinApiException("Couldn't retrieve league base address");
	}

	blacklistedObjects.clear();
}


void LeagueMemoryReader::ReadRenderer(MemSnapshot& ms) {
	DWORD64 rendererAddr = Mem::ReadDWORD(hProcess, moduleBaseAddr + Offsets::Renderer);
	ms.renderer->LoadFromMem(rendererAddr, moduleBaseAddr, hProcess);

}

void LeagueMemoryReader::FindHoveredObject(MemSnapshot& ms) {
	
	int addrObj = Mem::ReadDWORD(hProcess, (moduleBaseAddr + Offsets::UnderMouseObject) + 0x18);
	int netId = Mem::ReadDWORD(hProcess, addrObj + Offsets::ObjNetworkID);
	
	auto it = ms.objectMap.find(netId);
	if (it != ms.objectMap.end())
		ms.hoveredObject = it->second;
	else
		ms.hoveredObject = nullptr;
}


///		This method reads the game objects from memory. It reads the tree structure of a std::map<int, GameObject*>
/// in this std::map reside Champions, Minions, Turrets, Missiles, Jungle mobs etc. Basically non static objects.
void LeagueMemoryReader::ReadChamps(MemSnapshot& ms) {

	high_resolution_clock::time_point readTimeBegin;
	duration<float, std::milli> readDuration;
	readTimeBegin = high_resolution_clock::now();
	ms.champions.clear();
	ms.others.clear();

	auto HeroList = Mem::ReadDWORD(hProcess, moduleBaseAddr + Offsets::HeroList);
	auto pList = Mem::ReadDWORD(hProcess, HeroList + 0x8);
	UINT pSize = Mem::ReadDWORD(hProcess, HeroList + 0x10);

	// Read objects from the pointers we just read
	std::ifstream file("champnames.txt");
	std::ofstream ofile("champnames.txt");
	if (file.peek() == std::ifstream::traits_type::eof()) {
		std::vector<std::string> names;
		for (unsigned int i = 0; i < pSize; ++i) {
			auto champObject = Mem::ReadDWORD(hProcess, pList + (0x8 * i));

			std::shared_ptr<GameObject> obj;
			obj = std::shared_ptr<GameObject>(new GameObject());
			obj->LoadFromMem(champObject, hProcess, true);
			ms.objectMap[obj->networkId] = obj;
		
			if (ofile.is_open()) {
				ofile << obj->name<< std::endl;
			}
		}
		ofile.close();
	}
	file.close();

	std::ifstream ifile("champnames.txt");
	std::vector<std::string> champs;
	for (unsigned int f = 0; f < pSize; ++f) {
		auto champObject = Mem::ReadDWORD(hProcess, pList + (0x8 * f));

		std::shared_ptr<GameObject> obj;
		obj = std::shared_ptr<GameObject>(new GameObject());
		obj->LoadFromMem(champObject, hProcess, true);
		if (ifile.is_open()) {
			std::string line;
			while (std::getline(ifile, line)) {
				if (!ifile.eof()) {
					champs.push_back(line);
				}
			}
		}
		obj->name = champs[f];
		obj->unitInfo->tags[2] = 1;
		ms.objectMap[obj->networkId] = obj;
		if (obj->name.size() <= 2 || blacklistedObjectNames.find(obj->name) != blacklistedObjectNames.end())
			blacklistedObjects.insert(obj->networkId);
//		else if (obj->HasUnitTags(Unit_Champion) && obj->level > 0) Level offset wrong
		else if (obj->HasUnitTags(Unit_Champion)) {
			obj->LoadChampionFromMem(champObject, hProcess, true);
			ms.champions.push_back(obj);
		}
		else
			ms.others.push_back(obj);
	}
	ifile.close();

	readDuration = high_resolution_clock::now() - readTimeBegin;
	ms.benchmark->readObjectsMs = readDuration.count();
}

void LeagueMemoryReader::ReadMinions(MemSnapshot& ms) {
	high_resolution_clock::time_point readTimeBegin;
	duration<float, std::milli> readDuration;
	readTimeBegin = high_resolution_clock::now();

	ms.minions.clear();
	ms.jungle.clear();

	auto MinionList = Mem::ReadDWORD(hProcess, moduleBaseAddr + Offsets::MinionList);
	auto pList = Mem::ReadDWORD(hProcess, MinionList + 0x8);
	UINT pSize = Mem::ReadDWORD(hProcess, MinionList + 0x10);

	// Read objects from the pointers we just read
	for (unsigned int i = 0; i < pSize; ++i) {

		auto champObject = Mem::ReadDWORD(hProcess, pList + (0x8 * i));

		std::shared_ptr<GameObject> obj;
		obj = std::shared_ptr<GameObject>(new GameObject());
		obj->LoadFromMem(champObject, hProcess, true);
		ms.objectMap[obj->networkId] = obj;

		if (obj->name.size() <= 2 || blacklistedObjectNames.find(obj->name) != blacklistedObjectNames.end())
			blacklistedObjects.insert(obj->networkId);
		else if (obj->HasUnitTags(Unit_Minion_Lane))
			ms.minions.push_back(obj);
		else if (obj->HasUnitTags(Unit_Monster))
			ms.jungle.push_back(obj);
		else if ((obj->HasUnitTags(Unit_Ward) && !obj->HasUnitTags(Unit_Plant)))
			ms.others.push_back(obj);
	}

	readDuration = high_resolution_clock::now() - readTimeBegin;
	ms.benchmark->readObjectsMs = readDuration.count();
}

void LeagueMemoryReader::ReadMissiles(MemSnapshot& ms) {
	high_resolution_clock::time_point readTimeBegin;
	duration<float, std::milli> readDuration;
	readTimeBegin = high_resolution_clock::now();

	ms.missiles.clear();

	
	static char buff[0x500];

	UINT64 missile_list = Mem::ReadDWORD(hProcess, moduleBaseAddr + Offsets::MissileList);
	UINT64 numMissiles, rootNode;
	memcpy(&numMissiles, buff + Offsets::MissileMapCount, sizeof(UINT64));
	memcpy(&rootNode, buff + Offsets::MissileMapRoot, sizeof(UINT64));

	std::queue<UINT64> nodesToVisit;
	std::set<UINT64> missilesVisited;

	UINT64 root = missile_list;
	std::cout << "root: " << root << std::endl;
	int counter = 1;

	nodesToVisit.push(root);

	while (nodesToVisit.size() > 0)
	{
		UINT64 node = nodesToVisit.front(); nodesToVisit.pop();

		auto pissObject = Mem::ReadDWORD(hProcess, node);

		std::shared_ptr<GameObject> obj;
		obj = std::shared_ptr<GameObject>(new GameObject());
		obj->LoadFromMem(pissObject, hProcess, true);

		obj->LoadMissileFromMem(pissObject, hProcess, true);

		ms.missiles.push_back(obj);


		if (missilesVisited.find(node) != missilesVisited.end())
			break;
		else
			missilesVisited.insert(node);
		std::cout << "node: " << node << std::endl;
		UINT64 nd1 = Mem::ReadDWORD(hProcess, node + 0x0);
		UINT64 nd2 = Mem::ReadDWORD(hProcess, node + 0x8);
		UINT64 nd3 = Mem::ReadDWORD(hProcess, node + 0x10);
		if (nd1 != root && nd1)
		{
			std::cout << "nd1: " << nd1 << std::endl;
			nodesToVisit.push(nd1);
		}
		if (nd2 != root && nd2)
		{
			std::cout << "nd2: " << nd2 << std::endl;
			nodesToVisit.push(nd2);
		}
		if (nd3 != root && nd3)
		{
			std::cout << "nd3: " << nd3 << std::endl;
			nodesToVisit.push(nd3);
		}
	}






























	//UINT64 childNode1, childNode2, childNode3, node;
	//while (nodesToVisit.size() > 0 && visitedNodes.size() < numMissiles * 2) {
	//	node = nodesToVisit.front();
	//	nodesToVisit.pop();
	//	visitedNodes.insert(node);

	//	Mem::Read(hProcess, node, buff, 0x50);
	//	memcpy(&childNode1, buff, sizeof(UINT64));
	//	memcpy(&childNode2, buff + 4, sizeof(UINT64));
	//	memcpy(&childNode3, buff + 8, sizeof(UINT64));

	//	if (visitedNodes.find(childNode1) == visitedNodes.end())
	//		nodesToVisit.push(childNode1);
	//	if (visitedNodes.find(childNode2) == visitedNodes.end())
	//		nodesToVisit.push(childNode2);
	//	if (visitedNodes.find(childNode3) == visitedNodes.end())
	//		nodesToVisit.push(childNode3);

	//	UINT64 netId = 0;
	//	memcpy(&netId, buff + Offsets::MissileMapKey, sizeof(UINT64));

	//	// Actual missiles net_id start from 0x40000000 and throught the game this id will be incremented by 1 for each missile.
	//	// So we use this information to check if missiles are valid.
	//	/*if (netId - (UINT64)0x40000000 > 0x100000)
	//		continue;*/

	//	UINT64 addr;
	//	memcpy(&addr, buff + Offsets::MissileMapVal, sizeof(UINT64));
	//	if (addr == 0)
	//		continue;

	//	// The MissileClient is wrapped around another object
	//	addr = Mem::ReadDWORD(hProcess, addr + 0x4);
	//	if (addr == 0)
	//		continue;

	//	addr = Mem::ReadDWORD(hProcess, addr + 0x10);
	//	if (addr == 0)
	//		continue;

	//	// At this point addr is the address of the MissileClient
		//auto champObject = Mem::ReadDWORD(hProcess, addr);
		//std::shared_ptr<GameObject> obj;
	//	obj = std::shared_ptr<GameObject>(new GameObject());
	//	obj->LoadFromMem(champObject, hProcess, true);
	//	ms.objectMap[obj->networkId] = obj;

	//	// Check one more time that we read a valid missile
	//	if (obj->networkId != netId)
	//		continue;

		//obj->LoadMissileFromMem(champObject, hProcess, true);
		//obj->LoadMissileFromMem(obj, hProcess, true);
		//ms.missiles.push_back(std::move(obj));
		//ms.missiles.push_back(obj);

	readDuration = high_resolution_clock::now() - readTimeBegin;
	ms.benchmark->readObjectsMs = readDuration.count();
	std::cout << "end of this shit" << std::endl;
}

void LeagueMemoryReader::ReadTurrets(MemSnapshot& ms) {
	high_resolution_clock::time_point readTimeBegin;
	duration<float, std::milli> readDuration;
	readTimeBegin = high_resolution_clock::now();

	ms.turrets.clear();

	auto TurretList = Mem::ReadDWORD(hProcess, moduleBaseAddr + Offsets::TurretList);
	auto pList = Mem::ReadDWORD(hProcess, TurretList + 0x8);
	UINT pSize = Mem::ReadDWORD(hProcess, TurretList + 0x10);

	// Read objects from the pointers we just read
	for (unsigned int i = 0; i < pSize; ++i) {

		auto champObject = Mem::ReadDWORD(hProcess, pList + (0x8 * i));

		std::shared_ptr<GameObject> obj;
		obj = std::shared_ptr<GameObject>(new GameObject());
		obj->LoadFromMem(champObject, hProcess, true);
		ms.objectMap[obj->networkId] = obj;

		if (obj->name.size() <= 2 || blacklistedObjectNames.find(obj->name) != blacklistedObjectNames.end())
			blacklistedObjects.insert(obj->networkId);
		else if ((obj->HasUnitTags(Unit_Ward) && !obj->HasUnitTags(Unit_Plant)))
			ms.others.push_back(obj);
		else
			ms.turrets.push_back(obj);
	}

	readDuration = high_resolution_clock::now() - readTimeBegin;
	ms.benchmark->readObjectsMs = readDuration.count();
}

void LeagueMemoryReader::ReadMinimap(MemSnapshot & snapshot) {
	int minimapObj = Mem::ReadDWORD(hProcess, moduleBaseAddr + Offsets::MinimapObject);
	int minimapHud = Mem::ReadDWORD(hProcess, minimapObj + Offsets::MinimapObjectHud);

	static char buff[0x80];
	Mem::Read(hProcess, minimapHud, buff, 0x80);
	memcpy(&snapshot.minimapPos, buff + Offsets::MinimapHudPos, sizeof(Vector2));
	memcpy(&snapshot.minimapSize, buff + Offsets::MinimapHudSize, sizeof(Vector2));
}

void LeagueMemoryReader::FindPlayerChampion(MemSnapshot & snapshot) {
	int netId = 0;
	Mem::Read(hProcess, Mem::ReadDWORD(hProcess, moduleBaseAddr + Offsets::LocalPlayer) + Offsets::ObjNetworkID, &netId, sizeof(int));
	auto it = snapshot.objectMap.find(netId);
	if (it != snapshot.objectMap.end())
		snapshot.player = it->second;
	else
		snapshot.player = (snapshot.champions.size() > 0 ? snapshot.champions[0] : nullptr);
}

void LeagueMemoryReader::ClearMissingObjects(MemSnapshot & ms) {
	auto it = ms.objectMap.begin();
	while (it != ms.objectMap.end()) {
		if (ms.updatedThisFrame.find(it->first) == ms.updatedThisFrame.end()) {
			it = ms.objectMap.erase(it);
		}
		else
			++it;
	}
}

void LeagueMemoryReader::GetMousePos(MemSnapshot& ms) {
	// MousePos
	auto Mouse = Mem::ReadDWORD(hProcess, moduleBaseAddr + Offsets::HudInstance);
	auto Mouse1 = Mem::ReadDWORD(hProcess, Mouse + 0x28); // maybe 0x38 
	Mem::Read(hProcess, Mouse1 + 0x20, &ms.mousePos.x, sizeof(float));
	Mem::Read(hProcess, Mouse1 + 0x24, &ms.mousePos.y, sizeof(float));
	Mem::Read(hProcess, Mouse1 + 0x28, &ms.mousePos.z, sizeof(float));

	// Checking ping
	DWORD64 pingInstance = Mem::ReadDWORD(hProcess, moduleBaseAddr + Offsets::Ping);
	DWORD64 pingOff = Mem::ReadDWORD(hProcess, pingInstance + Offsets::OffPing);
	Mem::Read(hProcess, pingOff + Offsets::ShowPing, &ms.ping, sizeof(int));
	// Checking chat
	DWORD64 chatInstance = Mem::ReadDWORD(hProcess, moduleBaseAddr + Offsets::Chat);
	Mem::Read(hProcess, chatInstance + Offsets::ChatIsOpen, &ms.isChatOpen, sizeof(bool));
}

void LeagueMemoryReader::MakeSnapshot(MemSnapshot& ms) {
	
	Mem::Read(hProcess, moduleBaseAddr + Offsets::GameTime, &ms.gameTime, sizeof(float));

	if (ms.gameTime > 2) {
		ms.updatedThisFrame.clear();
		ReadRenderer(ms);
		ReadMinimap(ms);
		ReadChamps(ms);
		ReadMinions(ms);
		ReadMissiles(ms);
		ReadTurrets(ms);
		ClearMissingObjects(ms);
		FindPlayerChampion(ms);
		FindHoveredObject(ms);
		GetMousePos(ms);

		ms.map = std::shared_ptr<MapObject>(MapObject::Get(ms.turrets.size() > 10 ? SUMMONERS_RIFT : HOWLING_ABYSS));
	}
}
