#include <utility>

#ifndef LEAGUE_SHOPPER_ENV_CPP
#define LEAGUE_SHOPPER_ENV_CPP


namespace env {
    class EnemyData {
    private:
        int armor;
        int magicResistance;

    public:
        EnemyData(const int& armor, const int& magicResistance) {
            this->armor = armor;
            this->magicResistance = magicResistance;
        }

        int getArmor() const {
            return armor;
        }

        int getMagicResistance() const {
            return magicResistance;
        }
    };

    enum DamageType {
        PHYSICAL,
        MAGIC,
        TRUE
    };

    class Ability {
    private:
        DamageType damageType;
        int baseDamage;
        double bonusAdScale;
        double apScale;

    public:
        Ability(const DamageType& damageType, const int& baseDamage,
                const double& bonusAdScale, const double& apScale) {
            this->damageType = damageType;
            this->baseDamage = baseDamage;
            this->bonusAdScale = bonusAdScale;
            this->apScale = apScale;
        }

        DamageType GetDamageType() const {
            return damageType;
        }

        int GetBaseDamage() const {
            return baseDamage;
        }

        double GetBonusAdScale() const {
            return bonusAdScale;
        }

        double GetApScale() const {
            return apScale;
        }
    };

    class DamageOutput {
    public:
        int physicalDamage = 0;
        int magicDamage    = 0;
        int trueDamage     = 0;
    };

    class Champion {
    private:
        std::string name;
        int baseAd;

    protected:
        std::vector<Ability> abilities;

    public:
        Champion(const std::string& name, const int& baseAd) {
            this->name = name;
            this->baseAd = baseAd;
        }

        std::string GetName() const {
            return name;
        }

        std::vector<Ability> GetAbilities() const {
            return abilities;
        }

        int GetBaseAd() const {
            return baseAd;
        }

        DamageType GetMainDamageType() const {
            int physicalDmg = 0;
            int magicDmg = 0;

            for (const Ability& ability : abilities) {
                // 250 - просто число "из головы" - достаточно большое, чтобы иметь значение при
                //       подсчёте, но не слишком большое, чтобы выходить за реальные рамки игры.
                //       Используется для того, чтобы учитывать не только базовый урон умений, но
                //       и урон, масштабирующийся от бонусных показателей урона (AD/AP).
                int damage = ability.GetBaseDamage()
                        + (int) (250 * ability.GetBonusAdScale() + 250 * ability.GetApScale());

                // Чистый урон не учитываем.
                if (ability.GetDamageType() == DamageType::PHYSICAL)
                    physicalDmg += damage;
                else if (ability.GetDamageType() == DamageType::MAGIC)
                    magicDmg += damage;
            }

            return physicalDmg > magicDmg ? DamageType::PHYSICAL : DamageType::MAGIC;
        }
    };

    class GameState {
    private:
        Champion champ;
        int curBonusAd, curAp;
        std::vector<EnemyData> enemies;

    public:
        GameState(Champion playerChamp, const int& curBonusAd, const int& curAp,
                  const std::vector<EnemyData>& enemies) : champ(std::move(playerChamp)) {
            this->curBonusAd = curBonusAd;
            this->curAp = curAp;
            this->enemies = enemies;
        }

        Champion GetChampion() const {
            return champ;
        }

        int GetCurBonusAd() const {
            return curBonusAd;
        }

        int GetTotalAd() const {
            return champ.GetBaseAd() + curBonusAd;
        }

        int GetCurAp() const {
            return curAp;
        }

        std::vector<EnemyData> GetEnemies() const {
            return enemies;
        }
    };

    class Talon : public Champion {
    public:
        Talon() : Champion("Талон", 121) {
            abilities.emplace_back(Ability( // P
                    DamageType::PHYSICAL, 245, 2.0, 0.0));
            abilities.emplace_back(Ability( // Q
                    DamageType::PHYSICAL, 165, 1.1, 0.0));
            abilities.emplace_back(Ability( // W
                    DamageType::PHYSICAL, 230, 1.1, 0.0));
        }
    };

    class Zoe : public Champion {
    public:
        Zoe() : Champion("Зои", 114) {
            abilities.emplace_back(Ability( // P
                    DamageType::MAGIC, 130, 0.2, 0.0));
            abilities.emplace_back(Ability( // Q
                    DamageType::MAGIC, 775, 1.5, 0.0));
            abilities.emplace_back(Ability( // W
                    DamageType::MAGIC, 195, 0.4, 0.0));
            abilities.emplace_back(Ability( // E (сон)
                    DamageType::MAGIC, 220, 0.4, 0.0));
            abilities.emplace_back(Ability( // E (пробуждение - макс. урон)
                    DamageType::TRUE, 220, 0.4, 0.0));
        }
    };

    std::vector<Champion> GetAvailableChampions() {
        std::vector<Champion> champions;

        champions.emplace_back(Talon());
        champions.emplace_back(Zoe());

        return champions;
    }
}


#endif //LEAGUE_SHOPPER_ENV_CPP
