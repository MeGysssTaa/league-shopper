#define VERSION std::wstring(L"1.0.0")

#define TITLE L"&e\n" \
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
    _setmode(_fileno(stdout), _O_U16TEXT); // поддержка Юникода

    cli::PrintLn(TITLE);
    cli::PrintLn(L"&e            LeagueShopper v" + VERSION + L"\n");

    std::vector<items::Item*> allItems = items::GetAvailableItems();
    game::GameState* gameState = cli::ReadGameState();
    bool verbose = cli::ReadVerbose();

    if (verbose)
        cli::PrintLn(L"Активирован режим подробного вывода (вычисления займут больше времени).");

    cli::PrintLn();
    cli::PrintGameState(gameState);
    calc::Evaluate(verbose, gameState, allItems);
    cli::PrintLn(L"до связи");
}
