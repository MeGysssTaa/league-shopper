#ifndef LEAGUE_SHOPPER_CALC_CPP
#define LEAGUE_SHOPPER_CALC_CPP


#include <iostream>
#include "items.cpp"

namespace calc {
    std::vector<items::Item> GetAppropriateItems(const env::Champion& champ) {
        return champ.GetMainDamageType() == env::DamageType::PHYSICAL
                ? items::GetAvailableAdItems() : items::GetAvailableApItems();
    }

    env::DamageOutput ComputeDamageOutput(const env::GameState& gameState,
                                          const env::EnemyData& enemy, const items::Item& item) {
        env::DamageOutput damage;

        // ��������� ���� �� ������.
        // �������, ��� ������ ������ ����� ������������ ������ ����� ����� ��������,
        // ��� ���� ������ ������ ����� ������������ �� �������� (���� ��� ���� ����� �����������).
        // ������ ������ ����� ������� ���������.
        for (const auto& ability : gameState.GetChampion().GetAbilities()) {
            int bonusAd = gameState.GetCurBonusAd();
            int ap = item.AmplifyChampionAp(gameState.GetCurAp());
            int abilityDamage = ability.GetBaseDamage()
                    + (int) (bonusAd * ability.GetBonusAdScale() + ap * ability.GetApScale());

            abilityDamage = item.AmplifyAbility(abilityDamage, gameState);

            switch (ability.GetDamageType()) {
                case env::DamageType::PHYSICAL:
                    damage.physicalDamage += abilityDamage;
                    break;

                case env::MAGIC:
                    damage.magicDamage += abilityDamage;
                    break;

                case env::TRUE:
                    damage.trueDamage += abilityDamage;
                    break;
            }
        }

        // ��������� ���� �� ��������.
        // �������, ��� ���� ������� 3 ���������, ���� �� ������� ����� ������� ���������.
        env::DamageOutput empoweredAaDmg = item.AmplifyAutoAttack(gameState);
        damage.physicalDamage += empoweredAaDmg.physicalDamage; // ���� ��������� ��������
        damage.magicDamage += empoweredAaDmg.magicDamage;       // ����� ���� ��������� (���. + ���.)
        damage.physicalDamage += 2 * gameState.GetTotalAd(); // � 2 ������� ���������

        /*
         * to_kill_raw = ...
         * to_kill_def = to_kill_raw * (1 + armor / 100)
         */

        // ��������� �������� ���������� �����.
        // 1.0 - RAW
        // 2.0 - RES    =>    RES = 2.0 * RAW / 1.0
        //todo

        return damage;
    }

    class ItemCalculator {
    private:
        std::string desc;

    protected:
        explicit ItemCalculator(const std::string& desc) {
            this->desc = desc;
        }

    public:
        std::string GetDesc() const {
            return desc;
        }

        virtual void Evaluate(const env::GameState& gameState) = 0;
    };

    class DamageToSquishestItemCalc : public ItemCalculator {
    public:
        DamageToSquishestItemCalc()
                : ItemCalculator("������������ ���� �� �������� ���������� ����")  {}

        void Evaluate(const env::GameState& gameState) override {
            int squishestEnemyIdx;
            int highest;

            for (int i = 0; i < gameState.GetEnemies().size(); i++) {
                int highestDamage;
                items::Item* bestItem;
                std::vector<items::Item> items = GetAppropriateItems(gameState.GetChampion());

                for (const auto& item : items) {

                }
            }
        }
    };

    class DamageToTankestItemCalc : public ItemCalculator {
    public:
        DamageToTankestItemCalc()
                : ItemCalculator("������������ ���� �� �������� ���������� ����")  {}

        void Evaluate(const env::GameState& gameState) override {
            //todo 2
        }
    };

    class OverallTeamfightDamageItemCalc : public ItemCalculator {
    public:
        OverallTeamfightDamageItemCalc()
                : ItemCalculator("������������ ����� ���� � ��������� ���������")  {}

        void Evaluate(const env::GameState& gameState) override {
            //todo 3
        }
    };

    std::vector<ItemCalculator*> GetAvailableCalculators() {
        std::vector<ItemCalculator*> calculators;

        calculators.emplace_back(new DamageToSquishestItemCalc());
        calculators.emplace_back(new DamageToTankestItemCalc());
        calculators.emplace_back(new OverallTeamfightDamageItemCalc());

        return calculators;
    }
}


#endif //LEAGUE_SHOPPER_CALC_CPP
