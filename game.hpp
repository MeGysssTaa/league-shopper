#pragma once


#include <string>
#include <vector>

namespace game {

    class EnemyData {
    private:
        int armor;
        int magicResistance;

    public:
        EnemyData(const int& armor, const int& magicResistance);

        int GetArmor() const;
        int GetMagicResistance() const;
    };

    class DamageOutput {
    public:
        int physicalDamage, magicDamage, trueDamage;

        DamageOutput();
        DamageOutput(const int& physicalDamage, const int& magicDamage, const int& trueDamage);

        int GetTotalDamage() const;

        bool operator> (const DamageOutput& other) const;
        bool operator==(const DamageOutput& other) const;
    };

    class Ability {
    private:
        std::wstring button;

        int usesPerEnemy, maxUsesInTeamfight;

        DamageOutput* baseDamage;

        double physicalDmgBonusAdScale, physicalDmgApScale,
               magicDmgBonusAdScale,    magicDmgApScale,
               trueDmgBonusAdScale,     trueDmgApScale;

    public:
        Ability(const std::wstring& button, const int& usesPerEnemy, DamageOutput* baseDamage);
        ~Ability();

        std::wstring GetButton() const;
        int GetUsesPerEnemy() const;
        DamageOutput* GetDamage(const bool& verbose, const int& bonusAd, const int& ap) const;

        void SetPhysicalDmgBonusAdScale(const double& n);
        void SetPhysicalDmgApScale(const double& n);
        void SetMagicDmgBonusAdScale(const double& n);
        void SetMagicDmgApScale(const double& n);
        void SetTrueDmgBonusAdScale(const double& n);
        void SetTrueDmgApScale(const double& n);
    };

    class Champion {
    private:
        std::wstring name;
        int baseAd;

    protected:
        std::vector<Ability*> abilities;

    public:
        Champion(const std::wstring& name, const int& baseAd);
        virtual ~Champion();

        std::wstring GetName() const;
        std::vector<Ability*> GetAbilities() const;
        int GetBaseAd() const;
    };

    class Talon : public Champion {
    public:
        Talon();
    };

    class Zoe : public Champion {
    public:
        Zoe();
    };

    class GameState {
    private:
        Champion* champ;
        int curBonusAd, curAp;
        std::vector<EnemyData*> enemies;

    public:
        GameState(Champion* champ, const int& curBonusAd, const int& curAp,
                  const std::vector<EnemyData*>& enemies);

        ~GameState();

        Champion* GetChampion() const;
        int GetCurBonusAd() const;
        int GetCurAp() const;
        std::vector<EnemyData*> GetEnemies() const;
    };

    std::vector<Champion*> GetAvailableChampions();

}
