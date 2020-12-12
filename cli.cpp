#ifndef LEAGUE_SHOPPER_CLI_CPP
#define LEAGUE_SHOPPER_CLI_CPP


#include <iostream>
#include <vector>
#include "env.cpp"
#include "calc.cpp"

namespace cli {
    env::Champion ReadChampion() {
        std::vector<env::Champion> champions = env::GetAvailableChampions();
        std::cout << "�������� ������ �������� �� ������ (������� ����� - ����� � �������):" << std::endl;
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
            std::cout << "��������� �������: " << champ.GetName() << std::endl << std::endl;

            return champ;
        } catch (const std::exception&) {
            // std::invalid_argument - ������� �� �����.
            // std::out_of_range     - ������� �����, ������ ��� �������� � ������� = (index - 1).
            // ������ ���������� ��� ���� �� ������.
            std::cout << "�������������� �������. ����� ������ �����, "
                         "������� � ������� ����� ������ ������� ���������." << std::endl;

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
            std::cout << "������ �����. ����������, ������� ����� ������������� �����." << std::endl;
            return ReadNaturalNum(message);
        }
    }

    env::EnemyData ReadEnemyData(const int& num) {
        std::string input;
        std::cout << std::endl << "������� ���������� � ���������� � " << num << std::endl;

        env::EnemyData enemyData(
                ReadNaturalNum("�����:"),
                ReadNaturalNum("������������� �����:")
        );

        std::cout << std::endl;

        return enemyData;
    }

    std::vector<env::EnemyData> ReadEnemies() {
        std::cout << "������ ���� �� �������?" << std::endl;
        std::vector<env::EnemyData> enemies;

        for (int i = 1; i <= 5; i++)
            enemies.push_back(ReadEnemyData(i));

        return enemies;
    }

    env::GameState ReadGameState() {
        env::Champion champ = ReadChampion();
        int curBonusAd = ReadNaturalNum("������� � ��� � ������ ������ �������� ���� �����?");
        int curAp = ReadNaturalNum("������� � ��� � ������ ������ ���� ������?");
        std::vector<env::EnemyData> enemies = ReadEnemies();

        return env::GameState(champ, curBonusAd, curAp, enemies);
    }

    calc::ItemCalculator* ReadCalc(std::vector<calc::ItemCalculator*> calculators) {
        std::cout << "��� ����� ���������?" << std::endl;
        std::cout << "�������� ��� ������������ �� ������ (������� ����� - ����� � �������):" << std::endl;

        for (int i = 0; i < calculators.size(); i++)
            std::cout << "    (" << i + 1 << ") " << calculators[i]->GetDesc() << "." << std::endl;

        std::string input;
        std::getline(std::cin, input);

        try {
            int inputInt = std::stoi(input);

            if (inputInt == 0)
                return nullptr; // ����� �� ���������

            int index = inputInt - 1;

            if (index < 0 || index >= calculators.size())
                throw std::out_of_range("index out of range: " + std::to_string(index));

            return calculators[index];
        } catch (const std::exception&) {
            // std::invalid_argument - ������� �� �����.
            // std::out_of_range     - ������� �����, ������ ��� ������������ � ������� = (index - 1).
            // ������ ���������� ��� ���� �� ������.
            std::cout << "�������������� ��� ������������. ����� ������ �����, "
                         "������� � ������� ����� ��������� ������� ������������." << std::endl;

            return ReadCalc(calculators);
        }
    }
}


#endif //LEAGUE_SHOPPER_CLI_CPP
