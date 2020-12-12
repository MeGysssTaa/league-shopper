#ifndef LEAGUE_SHOPPER_CLI_CPP
#define LEAGUE_SHOPPER_CLI_CPP


#include <iostream>
#include <vector>
#include "env.cpp"
#include "calc.cpp"

namespace cli {
    env::Champion ReadChampion() {
        std::vector<env::Champion> champions = env::GetAvailableChampions();
        std::cout << "Выберите вашего чемпиона из списка (введите номер - число в скобках):" << std::endl;
        env::Champion c = champions[0];

        for (int i = 0; i < champions.size(); i++)
            std::cout << "    (" << i + 1 << ") " << champions[i].GetName() << "." << std::endl;

        std::string input;
        std::getline(std::cin, input);

        try {
            int index = std::stoi(input) - 1;

            if (index < 0 || index >= champions.size())
                throw std::out_of_range("index out of range: " + std::to_string(index));

            env::Champion champ = champions[index];
            std::cout << "Выбранный чемпион: " << champ.GetName() << std::endl << std::endl;

            return champ;
        } catch (const std::exception&) {
            // std::invalid_argument - Введено не число.
            // std::out_of_range     - Введено число, однако нет чемпиона с номером = (index - 1).
            // Других исключений тут быть не должно.
            std::cout << "Нераспознанный чемпион. Нужно ввести число, "
                         "стоящее в скобках перед именем нужного персонажа." << std::endl;

            return ReadChampion();
        }
    }

    int ReadNaturalNum(const std::string& message) {
        std::string input;
        std::cout << message << std::endl;
        std::getline(std::cin, input);

        try {
            int inputInt = std::stoi(input);

            if (inputInt < 0)
                throw std::invalid_argument("negative input: " + std::to_string(inputInt));

            return inputInt;
        } catch (const std::invalid_argument&) {
            std::cout << "Ошибка ввода. Пожалуйста, введите целое положительное число." << std::endl;
            return ReadNaturalNum(message);
        }
    }

    env::EnemyData ReadEnemyData(const int& num) {
        std::string input;
        std::cout << std::endl << "Введите информацию о противнике № " << num << std::endl;

        env::EnemyData enemyData(
                ReadNaturalNum("Броня:"),
                ReadNaturalNum("Сопротивление магии:")
        );

        std::cout << std::endl;

        return enemyData;
    }

    std::vector<env::EnemyData> ReadEnemies() {
        std::cout << "Против кого вы играете?" << std::endl;
        std::vector<env::EnemyData> enemies;

        for (int i = 1; i <= 5; i++)
            enemies.push_back(ReadEnemyData(i));

        return enemies;
    }

    env::GameState ReadGameState() {
        env::Champion champ = ReadChampion();
        int curBonusAd = ReadNaturalNum("Сколько у вас в данный момент бонусной силы атаки?");
        int curAp = ReadNaturalNum("Сколько у вас в данный момент силы умений?");
        std::vector<env::EnemyData> enemies = ReadEnemies();

        return env::GameState(champ, curBonusAd, curAp, enemies);
    }

    calc::ItemCalculator* ReadCalc(std::vector<calc::ItemCalculator*> calculators) {
        std::cout << "Что нужно посчитать?" << std::endl;
        std::cout << "Выберите тип калькулятора из списка (введите номер - число в скобках):" << std::endl;

        for (int i = 0; i < calculators.size(); i++)
            std::cout << "    (" << i + 1 << ") " << calculators[i]->GetDesc() << "." << std::endl;

        std::string input;
        std::getline(std::cin, input);

        try {
            int inputInt = std::stoi(input);

            if (inputInt == 0)
                return nullptr; // выход из программы

            int index = inputInt - 1;

            if (index < 0 || index >= calculators.size())
                throw std::out_of_range("index out of range: " + std::to_string(index));

            return calculators[index];
        } catch (const std::exception&) {
            // std::invalid_argument - Введено не число.
            // std::out_of_range     - Введено число, однако нет калькулятора с номером = (index - 1).
            // Других исключений тут быть не должно.
            std::cout << "Нераспознанный тип калькулятора. Нужно ввести число, "
                         "стоящее в скобках перед описанием нужного калькулятора." << std::endl;

            return ReadCalc(calculators);
        }
    }
}


#endif //LEAGUE_SHOPPER_CLI_CPP
