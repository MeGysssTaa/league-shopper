#pragma once


#include <string>
#include <vector>

namespace game {

    class EnemyData {
    private:
        std::wstring name;
        int armor;
        int magicResistance;

    public:
        EnemyData(const std::wstring& name, const int& armor, const int& magicResistance);

        std::wstring GetName() const;
        int GetArmor() const;
        int GetMagicResistance() const;
    };

    class DamageOutput {
    public:
        int physicalDamage, magicDamage, trueDamage;

        DamageOutput();
        DamageOutput(const int& physicalDamage, const int& magicDamage, const int& trueDamage);

        int GetTotalDamage() const;

        bool operator>(const DamageOutput& other) const;
        bool operator==(const DamageOutput& other) const;
        DamageOutput* operator+=(const DamageOutput& other);
        DamageOutput* operator*(const int& factor);
    };

    class Ability {
    private:
        std::wstring button;
        int usesPerEnemy;
        DamageOutput* baseDamage;

        double physicalDmgTotalAdScale, physicalDmgBonusAdScale, physicalDmgApScale,
               magicDmgTotalAdScale,    magicDmgBonusAdScale,    magicDmgApScale,
               trueDmgTotalAdScale,     trueDmgBonusAdScale,     trueDmgApScale;

    public:
        Ability(const std::wstring& button, const int& usesPerEnemy, DamageOutput* baseDamage);
        ~Ability();

        int GetUsesPerEnemy() const;
        DamageOutput* GetDamage(const bool& verbose, const int& totalAd,
                                const int& bonusAd, const int& ap) const;

        void SetPhysicalDmgTotalAdScale(const double& n);
        void SetPhysicalDmgBonusAdScale(const double& n);
        void SetPhysicalDmgApScale(const double& n);
        void SetMagicDmgTotalAdScale(const double& n);
        void SetMagicDmgBonusAdScale(const double& n);
        void SetMagicDmgApScale(const double& n);
        void SetTrueDmgTotalAdScale(const double& n);
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

    class Nocturne : public Champion {
    public:
        Nocturne();
    };

    class Zoe : public Champion {
    public:
        Zoe();
    };

    class Lux : public Champion {
    public:
        Lux();
    };

    class Fizz : public Champion {
    public:
        Fizz();
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
