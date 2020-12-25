#define LOG_FILE_NAME "league_shopper.log"
#define MAX_INT_INPUT 5000
#define HEX std::wstring(L"0123456789abcdef")
#define DEFAULT_COLOR 0x7
#define BOLD_WHITE 0xF
#define COLOR_CODES_PATTERN L"&[\\da-fA-F]"


#include <windows.h>
#include <locale>
#include <regex>
#include <fstream>
#include "cli.hpp"

namespace cli {

    HANDLE _consoleHandle = nullptr;
    FILE* _logFile = nullptr;

    void SetupLogging() {
        _consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        std::remove(LOG_FILE_NAME);
        _logFile = std::fopen( LOG_FILE_NAME, "w+,ccs=UTF-8");
    }

    void OnExit() {
        std::fclose(_logFile);
        delete _logFile;
    }

    std::wstring WithPadding(const int& effectiveWidth, std::wstring text) {
        if (effectiveWidth < 1)
            throw std::invalid_argument("effectiveWidth cannot be smaller than 1");

        int textLen = std::regex_replace(text, std::wregex(COLOR_CODES_PATTERN), L"").length();

        if (textLen > effectiveWidth)
            throw std::invalid_argument("effective text length (without color codes) "
                                        "cannot be greater than effectiveWidth");

        for (; textLen < effectiveWidth; textLen++)
            text += L" ";

        return text;
    }

    void Color(const int& color) {
        if (color < 0 || color > 255)
            throw std::invalid_argument("color must be in range 0..255");

        SetConsoleTextAttribute(_consoleHandle, color);
    }

    std::wstring::size_type FromHex(const wchar_t& c) {
        return HEX.find(c);
    }

    void PrintLn(std::wstring text, const int& effectiveWidth, const std::wstring& suffix) {
        if (text.empty())
            throw std::invalid_argument("text cannot be empty (use pure cli::PrintLn() instead)");

        if (effectiveWidth != -1)
            text = WithPadding(effectiveWidth, text); // дополняем пробелами до нужной длины (если нужно)

        std::wstring::size_type offset = 0; // отступ слева от начала текста при выводе очередной части сообщения
        Color(DEFAULT_COLOR); // сброс цвета на белый при каждом выводе

        for (std::wstring::size_type i = 0; i < text.length() - 1; i++) {
            auto hexCol = FromHex(text[i + 1]); // -1, если это не шестнадцатеричная цифра (0..9 | a..f)

            if (text[i] == '&' && hexCol != std::wstring::npos) { // нашли цветовой код (например, "&e" (=0xE=14))
                // Выводим текст слева от нового цветового кода.
                std::wstring prev = text.substr(offset, i - offset);
                const wchar_t* prevCStr = prev.c_str();
                std::wcout << prev;
                std::fwrite(prevCStr, wcslen(prevCStr) * sizeof(wchar_t), 1, _logFile);

                Color(hexCol); // устанавливаем цвет для вывода текста справа от нового цветового кода
                offset = i + 2; // пропускаем сам цветовой код при следующем выводе
                i++; // перескакиваем через следующий символ (цветовой код после '&')
            }
        }

        // Выводим оставшийся текст (если есть).
        std::wstring remainder = text.substr(offset, text.length());
        const wchar_t* remainderCStr = remainder.c_str();
        std::wcout << remainder;
        std::fwrite(remainderCStr, wcslen(remainderCStr) * sizeof(wchar_t), 1, _logFile);

        if (suffix.empty()) {
            // Суффикс не указан, завершаем вывод новой строкой.
            std::wcout << std::endl;
            std::fwrite(L"\n", wcslen(L"\n") * sizeof(wchar_t), 1, _logFile);
        } else
            PrintLn(suffix); // выводим суффикс (с поддержкой цветовых кодов)

        Color(DEFAULT_COLOR); // сброс цвета на белый после каждого вывода
    }

    void PrintLn(const std::wstring& text) {
        PrintLn(text, -1, L""); // вывод текста без суффикса и контроля длины (отступа)
    }

    void PrintLn() {
        // Просто выводим пустую строку.
        std::wcout << std::endl;
        std::fwrite(L"\n", wcslen(L"\n") * sizeof(wchar_t), 1, _logFile);
    }

    std::wstring Trim(const std::wstring& str) {
        if (str.empty())
            return str;

        std::wstring::size_type lOffset = 0;
        std::wstring::size_type nChars = str.length();

        for (const wchar_t& c : str) {
            if (std::iswspace(c)) {
                lOffset++;
                nChars--;
            } else break;
        }

        for (int i = str.length() - 1; i >= 0; i--) {
            if (std::iswspace(str[i]))
                nChars--;
            else break;
        }

        return str.substr(lOffset, nChars);
    }

    std::wstring ReadLine() {
        Color(BOLD_WHITE);
        std::wstring input;
        std::getline(std::wcin, input);
        input = Trim(input);
        Color(DEFAULT_COLOR);

        return input.empty() ? ReadLine() : input;
    }

    game::Champion* ReadChampion() {
        std::vector<game::Champion*> champions = game::GetAvailableChampions();
        PrintLn(L"Выберите вашего чемпиона из списка (введите номер - число в скобках):");

        for (int i = 0; i < champions.size(); i++)
            PrintLn(L"    (" + std::to_wstring(i + 1) + L") " + champions[i]->GetName() + L".");

        std::wstring input = ReadLine();

        try {
            int index = std::stoi(input) - 1;

            if (index < 0 || index >= champions.size())
                throw std::out_of_range("index out of range: " + std::to_string(index));

            return champions[index];
        } catch (const std::exception&) {
            // std::invalid_argument | std::out_of_range
            PrintLn(L"&cОшибка ввода. Нераспознанный чемпион. Нужно ввести число, "
                    "стоящее в скобках перед именем нужного персонажа.");

            return ReadChampion();
        }
    }

    int ReadLimitedInt(const std::wstring& message) {
        PrintLn(message);
        std::wstring input = ReadLine();

        try {
            int inputInt = std::stoi(input);

            if (inputInt < 0 || inputInt > MAX_INT_INPUT)
                throw std::invalid_argument("bad input: " + std::to_string(inputInt));

            return inputInt;
        } catch (const std::exception&) {
            // std::invalid_argument | std::out_of_range
            PrintLn(L"&cОшибка ввода. Нужно ввести целое число в диапазоне"
                    " от 1 до " + std::to_wstring(MAX_INT_INPUT) + L".");
            return ReadLimitedInt(message);
        }
    }

    game::EnemyData* ReadEnemyData(const int& num) {
        std::wstring input;
        PrintLn(L"Введите информацию о противнике № " + std::to_wstring(num));

        auto* enemyData = new game::EnemyData(
                L"№ " + std::to_wstring(num),
                ReadLimitedInt(L"Броня:"),
                ReadLimitedInt(L"Сопротивление магии:")
        );

        PrintLn();

        return enemyData;
    }

    std::vector<game::EnemyData*> ReadEnemies() {
        PrintLn(L"Против кого вы играете?");
        std::vector<game::EnemyData*> enemies;
        int enemiesNum = ReadLimitedInt(L"Количество противников:");

        while (enemiesNum < 1 || enemiesNum > 9) {
            PrintLn(L"&cОшибка ввода. Укажите от 1 до 9 противников.");
            enemiesNum = ReadLimitedInt(L"Количество противников:");
        }

        for (int i = 1; i <= enemiesNum; i++)
            enemies.push_back(ReadEnemyData(i));

        return enemies;
    }

    game::GameState* ReadGameState() {
        game::Champion* champ = ReadChampion();
        int curBonusAd = ReadLimitedInt(L"Сколько у вас в данный момент бонусной силы атаки?");
        int curAp = ReadLimitedInt(L"Сколько у вас в данный момент силы умений?");
        std::vector<game::EnemyData*> enemies = ReadEnemies();

        return new game::GameState(champ, curBonusAd, curAp, enemies);
    }

    bool EqIgnCase(const std::wstring& a, const std::wstring& b) {
        if (a.length() != b.length() || a.empty())
            return false;

        for (std::wstring::size_type i = 0; i < a.length(); i++)
            if (std::towlower(a[i]) != std::towlower(b[i]))
                return false;

        return true;
    }

    bool ReadVerbose() {
        PrintLn(L"Нужны ли вам подробности всех вычислений? &8[&aда&8/&cнет&8]&7");
        std::wstring input = ReadLine();

        if (EqIgnCase(input, L"д") || EqIgnCase(input, L"да")
                || EqIgnCase(input, L"y") || EqIgnCase(input, L"yes"))
            return true;
        else if (EqIgnCase(input, L"н") || EqIgnCase(input, L"нет")
                || EqIgnCase(input, L"n") || EqIgnCase(input, L"no"))
            return false;
        else {
            PrintLn(L"&cОшибка ввода. Нужно ввести &fДА &cили &fНЕТ&c.");
            return ReadVerbose();
        }
    }

    void EchoGameState(const game::GameState* gameState) {
        PrintLn(L"&8┏━━━━━ &aВвод &8━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓");
        PrintLn(L"&8┃  &7Ваш чемпион               &f" + gameState->GetChampion()->GetName(),
                60, L"  &8┃");
        PrintLn(L"&8┃  &7Базовая сила атаки        &e"
                + std::to_wstring(gameState->GetChampion()->GetBaseAd()),
                60, L"  &8┃");
        PrintLn(L"&8┃  &7Доп. сила атаки           &e"
                + std::to_wstring(gameState->GetCurBonusAd()),
                60, L"  &8┃");
        PrintLn(L"&8┃  &7Сила умений               &d"
                + std::to_wstring(gameState->GetCurAp()),
                60, L"  &8┃");
        PrintLn(L"&8┃  &7Количество противников    &c"
                + std::to_wstring(gameState->GetEnemies().size()),
                60, L"  &8┃");
        PrintLn(L"&8┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n\n");
    }

}
