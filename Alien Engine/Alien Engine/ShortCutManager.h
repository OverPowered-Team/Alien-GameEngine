#pragma once

#include "SDL/include/SDL_scancode.h"
#include <functional>
#include <vector>
#include "Globals.h"

enum class ShortCutType {
	COMPLETE,
	TWO_KEYS,
	ONE_KEY,

	NONE
};

enum class ShortCutStateChange {
	WAITING_KEY_DOWN,
	WAITING_KEY_REPEAT,
	WAITING_EXTRA_KEY_REPEAT,

	NONE
};
struct ShortCut {

	ShortCut(const char* order_name, const SDL_Scancode& key1_down, std::function<void()> funct, const SDL_Scancode& key2_repeat = SDL_SCANCODE_UNKNOWN, const SDL_Scancode& key3_repeat_extra = SDL_SCANCODE_UNKNOWN);

	// functions
	void SetShortcutKeys(const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat = SDL_SCANCODE_UNKNOWN, const SDL_Scancode& key3_repeat_extra = SDL_SCANCODE_UNKNOWN);
	const char* GetNameScancodes();
	const char* GetNameOrder();

	const char* GetKeyDownName();
	const char* GetKeyRepeatName();
	const char* GetExtraKeyRepeatName();
	const SDL_Scancode& GetScancode(const uint& index);

	SDL_Scancode* GetScancodesArray();

	// variables
	SDL_Scancode key1_down = SDL_SCANCODE_UNKNOWN;
	SDL_Scancode key2_repeat = SDL_SCANCODE_UNKNOWN; 
	SDL_Scancode key3_repeat_extra = SDL_SCANCODE_UNKNOWN;
	std::function<void()> funct = nullptr;
	ShortCutType type = ShortCutType::NONE;
	ShortCutStateChange state = ShortCutStateChange::NONE;

	private:

	// functions
	const char* GetShortcutName();

	// variables
	const char* name = nullptr;
	const char* order_name = nullptr;
	char shortcut_char[50];
	SDL_Scancode codes[3] = { SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UNKNOWN };

};

class ShortCutManager {

public:

	ShortCutManager();
	~ShortCutManager();

	void UpdateShortCuts();
	ShortCut* AddShortCut(const char* order_name, const SDL_Scancode& key1_down, std::function<void()> funct, const SDL_Scancode& key2_repeat = SDL_SCANCODE_UNKNOWN, const SDL_Scancode& key3_repeat_extra = SDL_SCANCODE_UNKNOWN);
	std::vector<ShortCut*> GetShortCuts();
	void OrderShortCuts();
	static bool SortByType(const ShortCut* shortcut1, const ShortCut* shortcut2);

private:

	void ChangeKey();
	void ShortCutClicked();

private:

	std::vector<ShortCut*> shortcuts;


};
