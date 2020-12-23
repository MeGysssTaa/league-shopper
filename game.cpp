#include "game.hpp"
#include "cli.hpp"

namespace game {

    /*
     * EnemyData
     */
    EnemyData::EnemyData(const std::wstring& name, const int& armor, const int& magicResistance) {
        this->name = name;
        this->armor = armor;
        this->magicResistance = magicResistance;
    }

    std::wstring EnemyData::GetName() const {
        return name;
    }

    int EnemyData::GetArmor() const {
        return armor;
    }

    int EnemyData::GetMagicResistance() const {
        return magicResistance;
    }

    /*
     * DamageOutput
     */
    DamageOutput::DamageOutput() {
        physicalDamage = magicDamage = trueDamage = 0;
    }

    DamageOutput::DamageOutput(const int& physicalDamage, const int& magicDamage, const int& trueDamage) {
        this->physicalDamage = physicalDamage;
        this->magicDamage = magicDamage;
        this->trueDamage = trueDamage;
    }

    int DamageOutput::GetTotalDamage() const {
        return physicalDamage + magicDamage + trueDamage;
    }

    bool DamageOutput::operator> (const DamageOutput& other) const {
        return GetTotalDamage() > other.GetTotalDamage();
    }

    bool DamageOutput::operator==(const DamageOutput& other) const {
        return GetTotalDamage() == other.GetTotalDamage();
    }

    DamageOutput* DamageOutput::operator+=(const DamageOutput& other) {
        physicalDamage += other.physicalDamage;
        magicDamage += other.magicDamage;
        trueDamage += other.trueDamage;

        return this;
    }

    DamageOutput* DamageOutput::operator*(const int& factor) {
        physicalDamage *= factor;
        magicDamage *= factor;
        trueDamage *= factor;

        return this;
    }

    /*
     * Ability
     */
    Ability::Ability(const std::wstring& button, const int& usesPerEnemy, DamageOutput* baseDamage) {
        this->button = button;
        this->usesPerEnemy = usesPerEnemy;
        this->baseDamage = baseDamage;

        physicalDmgTotalAdScale = physicalDmgBonusAdScale = physicalDmgApScale
                = magicDmgTotalAdScale = magicDmgBonusAdScale = magicDmgApScale
                        = trueDmgTotalAdScale = trueDmgBonusAdScale = trueDmgApScale = 0;
    }

    Ability::~Ability() {
        delete baseDamage;
    }

    int Ability::GetUsesPerEnemy() const {
        return usesPerEnemy;
    }

    DamageOutput* Ability::GetDamage(const bool& verbose, const int& totalAd,
                                     const int& bonusAd, const int& ap) const {
        int bonusPhysical
                = (int) (physicalDmgTotalAdScale * totalAd)
                + (int) (physicalDmgBonusAdScale * bonusAd)
                + (int) (physicalDmgApScale      * ap);

        int bonusMagic
                = (int) (magicDmgTotalAdScale * totalAd)
                + (int) (magicDmgBonusAdScale * bonusAd)
                + (int) (magicDmgApScale      * ap);

        int bonusTrue
                = (int) (trueDmgTotalAdScale * totalAd)
                + (int) (trueDmgBonusAdScale * bonusAd)
                + (int) (trueDmgApScale      * ap);

        auto* totalDmg = new DamageOutput(
                baseDamage->physicalDamage + bonusPhysical,
                baseDamage->magicDamage + bonusMagic,
                baseDamage->trueDamage + bonusTrue
        );

        if (verbose) {
            cli::PrintLn(L"&8┃  ┃  ┃  ", 56, L"&8┃  ┃  ┃"); // немного "пустоты" между строк
            cli::PrintLn(L"&8┃  ┃  ┃  &f" + button + L" &7(&c"
                    + std::to_wstring(totalDmg->GetTotalDamage()) + L"&7 всего, без учёта предмета):",
                    56, L"&8┃  ┃  ┃");
            cli::PrintLn(L"&8┃  ┃  ┃      &7Физический     &e"
                    + std::to_wstring(totalDmg->physicalDamage) + L" &7= &e"
                    + std::to_wstring(baseDamage->physicalDamage) + L" &7+ &e"
                    + std::to_wstring(bonusPhysical),
                    56, L"&8┃  ┃  ┃");
            cli::PrintLn(L"&8┃  ┃  ┃      &7Магический     &d"
                    + std::to_wstring(totalDmg->magicDamage) + L" &7= &d"
                    + std::to_wstring(baseDamage->magicDamage) + L" &7+ &d"
                    + std::to_wstring(bonusMagic),
                    56, L"&8┃  ┃  ┃");
            cli::PrintLn(L"&8┃  ┃  ┃      &7Чистый         &f"
                    + std::to_wstring(totalDmg->trueDamage) + L" &7= &f"
                    + std::to_wstring(baseDamage->trueDamage) + L" &7+ &f"
                    + std::to_wstring(bonusTrue),
                    56, L"&8┃  ┃  ┃");
        }

        return totalDmg;
    }

    void Ability::SetPhysicalDmgTotalAdScale(const double& n) {
        physicalDmgTotalAdScale = n;
    }

    void Ability::SetPhysicalDmgBonusAdScale(const double& n) {
        physicalDmgBonusAdScale = n;
    }

    void Ability::SetPhysicalDmgApScale(const double& n) {
        physicalDmgApScale = n;
    }

    void Ability::SetMagicDmgTotalAdScale(const double& n) {
        magicDmgTotalAdScale = n;
    }

    void Ability::SetMagicDmgBonusAdScale(const double& n) {
        magicDmgBonusAdScale = n;
    }

    void Ability::SetMagicDmgApScale(const double& n) {
        magicDmgApScale = n;
    }

    void Ability::SetTrueDmgTotalAdScale(const double& n) {
        trueDmgTotalAdScale = n;
    }

    void Ability::SetTrueDmgBonusAdScale(const double& n) {
        trueDmgBonusAdScale = n;
    }

    void Ability::SetTrueDmgApScale(const double& n) {
        trueDmgApScale = n;
    }

    /*
     * Champion
     */
    Champion::Champion(const std::wstring& name, const int& baseAd) {
        this->name = name;
        this->baseAd = baseAd;
    }

    Champion::~Champion() {
        for (Ability* ability : abilities)
            delete ability;
    }

    std::wstring Champion::GetName() const {
        return name;
    }

    std::vector<Ability*> Champion::GetAbilities() const {
        return abilities;
    }

    int Champion::GetBaseAd() const {
        return baseAd;
    }

    /*
     * Talon
     */
    Talon::Talon() : Champion(L"Талон", 121) {
        auto* p = new Ability(L"P", 1,
                              new DamageOutput(245, 0, 0));
        p->SetPhysicalDmgBonusAdScale(2.0);
        abilities.emplace_back(p);

        auto* q = new Ability(L"Q", 1,
                              new DamageOutput(165, 0, 0));
        q->SetPhysicalDmgBonusAdScale(1.1);
        abilities.emplace_back(q);

        auto* w = new Ability(L"W", 1,
                              new DamageOutput(230, 0, 0));
        w->SetPhysicalDmgBonusAdScale(1.1);
        abilities.emplace_back(w);

        auto* r = new Ability(L"R", 1,
                              new DamageOutput(320, 0, 0));
        r->SetPhysicalDmgBonusAdScale(2.0);
        abilities.emplace_back(r);
    }

    /*
     * Nocturne
     */
    Nocturne::Nocturne() : Champion(L"Ноктюрн", 115) {
        auto* p = new Ability(L"P", 7,
                              new DamageOutput(0, 0, 0));
        p->SetPhysicalDmgTotalAdScale(0.2);
        abilities.emplace_back(p);

        auto* q = new Ability(L"Q", 1,
                              new DamageOutput(245, 0, 0));
        q->SetPhysicalDmgBonusAdScale(0.75);
        abilities.emplace_back(q);

        auto* e = new Ability(L"E", 1,
                              new DamageOutput(0, 260, 0));
        e->SetMagicDmgApScale(1.0);
        abilities.emplace_back(e);
    }

    /*
     * Zoe
     */
    Zoe::Zoe() : Champion(L"Зои", 114) {
        auto* p = new Ability(L"P", 3,
                              new DamageOutput(0, 130, 0));
        p->SetMagicDmgApScale(0.2);
        abilities.emplace_back(p);

        auto* q = new Ability(L"Q", 1,
                              new DamageOutput(0, 550, 0));
        q->SetMagicDmgApScale(1.5);
        abilities.emplace_back(q);

        auto* w = new Ability(L"W", 1,
                              new DamageOutput(0, 195, 0));
        w->SetMagicDmgApScale(0.4);
        abilities.emplace_back(w);

        auto* e = new Ability(L"E", 1,
                              new DamageOutput(0, 220, 220));
        e->SetMagicDmgApScale(0.4);
        e->SetTrueDmgApScale(0.4);
        abilities.emplace_back(e);
    }

    /*
     * Lux
     */
    Lux::Lux() : Champion(L"Люкс", 110) {
        auto* p = new Ability(L"P", 2,
                              new DamageOutput(0, 190, 0));
        p->SetMagicDmgApScale(0.2);
        abilities.emplace_back(p);

        auto* q = new Ability(L"Q", 1,
                              new DamageOutput(0, 260, 0));
        q->SetMagicDmgApScale(0.6);
        abilities.emplace_back(q);

        auto* e = new Ability(L"E", 1,
                              new DamageOutput(0, 240, 0));
        e->SetMagicDmgApScale(0.6);
        abilities.emplace_back(e);

        auto* r = new Ability(L"R", 1,
                              new DamageOutput(0, 500, 0));
        r->SetMagicDmgApScale(1.0);
        abilities.emplace_back(r);
    }

    /*
     * Fizz
     */
    Fizz::Fizz() : Champion(L"Физз", 110) {
        auto* q = new Ability(L"Q", 2,
                              new DamageOutput(0, 70, 0));
        q->SetPhysicalDmgTotalAdScale(1.0);
        q->SetMagicDmgApScale(0.55);
        abilities.emplace_back(q);

        auto* w = new Ability(L"W", 2,
                              new DamageOutput(0, 130, 0));
        w->SetMagicDmgApScale(0.5);
        abilities.emplace_back(w);

        auto* e = new Ability(L"E", 1,
                              new DamageOutput(0, 270, 0));
        e->SetMagicDmgApScale(0.75);
        abilities.emplace_back(e);
    }

    /*
     * GameState
     */
    GameState::GameState(Champion* champ, const int& curBonusAd, const int& curAp,
                         const std::vector<EnemyData*>& enemies) {
        this->champ = champ;
        this->curBonusAd = curBonusAd;
        this->curAp = curAp;
        this->enemies = enemies;
    }

    GameState::~GameState() {
        delete champ;

        for (EnemyData* enemy : enemies)
            delete enemy;
    }

    Champion* GameState::GetChampion() const {
        return champ;
    }

    int GameState::GetCurBonusAd() const {
        return curBonusAd;
    }

    int GameState::GetCurAp() const {
        return curAp;
    }

    std::vector<EnemyData*> GameState::GetEnemies() const {
        return enemies;
    }

    /*
     * <...>
     */
    std::vector<Champion*> GetAvailableChampions() {
        return {
            new Talon(),
            new Nocturne(),
            new Zoe(),
            new Lux(),
            new Fizz()
        };
    }

}
