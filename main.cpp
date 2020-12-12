#include <iostream>
#include "cli.cpp"

#define TITLE "\n" \
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
              "                    |_|   |_|              \n\n\n"

int main() {
    setlocale(LC_ALL, "Russian");

    std::cout << TITLE;
    std::cout << "Добро пожаловать!" << std::endl;

    env::GameState gameState = cli::ReadGameState();
    std::vector<calc::ItemCalculator*> calculators = calc::GetAvailableCalculators();
    calc::ItemCalculator* calc;

    while ((calc = cli::ReadCalc(calculators)) != nullptr) { // nullptr значит, что был введён 0, т.е. выход
        std::cout << calc->GetDesc() << ":" << std::endl;
        calc->Evaluate(gameState);
        std::cout << std::endl;
    }

    std::cout << "До встречи!" << std::endl;
}
