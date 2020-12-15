#pragma once


#include <iostream>
#include <vector>
#include "game.hpp"
#include "calc.hpp"

namespace cli {

    std::wstring WithPadding(const int& effectiveWidth, std::wstring text);

    void Color(const int& color);

    int FromHex(const wchar_t& c);

    void PrintLn(std::wstring text, const int& effectiveWidth, const std::wstring& suffix);

    void PrintLn(const std::wstring& text);

    void PrintLn();

    std::wstring Trim(const std::wstring& str);

    std::wstring ReadLine();

    game::Champion* ReadChampion();

    int ReadLimitedInt(const std::wstring& message);

    game::EnemyData* ReadEnemyData(const int& num);

    std::vector<game::EnemyData*> ReadEnemies();

    game::GameState* ReadGameState();

    bool EqIgnCase(const std::wstring& a, const std::wstring& b);

    bool ReadVerbose();

    void PrintGameState(const game::GameState* gameState);

}
