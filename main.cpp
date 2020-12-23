#define VERSION std::wstring(L"1.0.0")

#define TITLE L"\n" \
              "      _                                    \n" \
              "     | |                                   \n" \
              "     | |     ___  __ _  __ _ _   _  ___    \n" \
              "     | |    / _ \\/ _` |/ _` | | | |/ _ \\   \n" \
              "     | |___|  __/ (_| | (_| | |_| |  __/   \n" \
              "   __|______\\___|\\__,_|\\__, |\\__,_|\\___|   \n" \
              "  / ____| |             __/ |              \n" \
              " | (___ | |__   ___  _ |___/ __   ___ _ __ \n" \
              "  \\___ \\| '_ \\ / _ \\| '_ \\| '_ \\ / _ \\ '__|\n" \
              "  ____) | | | | (_) | |_) | |_) |  __/ |   \n" \
              " |_____/|_| |_|\\___/| .__/| .__/ \\___|_|   \n" \
              "                    | |   | |              \n" \
              "                    |_|   |_|              \n"


#include <iostream>
#include <fcntl.h>
#include "cli.hpp"

int main() {
    // Поддержка Юникода. (Также нужно использовать wide-методы, функции и типы.)
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);

    // Настраиваем средства вывода в консоль и файл.
    cli::SetupLogging();

    cli::PrintLn(TITLE);
    cli::PrintLn(L"            LeagueShopper v" + VERSION + L"\n");

    std::vector<items::Item*> allItems = items::GetAvailableItems();
    game::GameState* gameState = cli::ReadGameState();
    bool verbose = cli::ReadVerbose();

    if (verbose)
        cli::PrintLn(L"Активирован режим подробного вывода (программа может работать медленнее).");

    cli::PrintLn();
    cli::EchoGameState(gameState);
    calc::Evaluate(verbose, gameState, allItems);
    cli::PrintLn();
    cli::PrintLn(L"Вы также можете просмотреть результаты "
                 "в файле рядом с &f.exe&7-файлом LeagueShopper."); //fixme вывод в файл ломается от символов Юникода

    // Сохраняем лог.
    cli::OnExit();
}
