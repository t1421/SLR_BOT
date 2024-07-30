#pragma once

#include <vector>
#include <string>
#include "../API/API/json.hpp"

struct Ability {
    int abilityAffinity;
    std::vector<bool> abilityAvailability;
    std::vector<int> abilityCost;
    std::string abilityDescription;
    std::vector<std::vector<std::string>> abilityDescriptionValues;
    bool abilityHidden;
    std::string abilityIdentifier;
    std::string abilityName;
    std::vector<int> abilityStars;
    int abilityType;
};

struct Card {
    std::string _id;
    std::vector<Ability> abilities;
    int affinity;
    int attackType;
    //std::vector<int> boosters;
    std::string buildingClass;
    std::string cardName;
    std::string cardNameImage;
    std::string cardNameSimple;
    std::string cardSlug;
    std::string category;
    int color;
    int damage;
    int defenseType;
    std::string description;
    int edition;
    int gender;
    int health;
    int maxCharges;
    int movementType;
    int offenseType;
    int officialCardIds;
    int orbsFire;
    int orbsFireFrost;
    int orbsFireNature;
    int orbsFireShadow;
    int orbsFrost;
    int orbsNature;
    int orbsNatureFrost;
    int orbsNeutral;
    int orbsShadow;
    int orbsShadowFrost;
    int orbsShadowNature;
    int orbsTotal;
    int powerCost;
    int promo;
    int rarity;
    std::string spellClass;
    int squadSize;
    int starterCard;
    int type;
    std::string unitClass;
    std::string unitModel;
    std::string unitSpecies;
    //std::vector<std::string> upgradeMaps;
    Card() {};
    Card(const nlohmann::json& jsonData)
        : _id(jsonData["_id"]),
        affinity(jsonData["affinity"]),
        attackType(jsonData["attackType"]),
        //boosters(jsonData["boosters"]),
        buildingClass(jsonData["buildingClass"]),
        cardName(jsonData["cardName"]),
        cardNameImage(jsonData["cardNameImage"]),
        cardNameSimple(jsonData["cardNameSimple"]),
        cardSlug(jsonData["cardSlug"]),
        category(jsonData["category"]),
        color(jsonData["color"]),
        damage(jsonData["damage"].at(3)),
        defenseType(jsonData["defenseType"]),
        description(jsonData["description"]),
        edition(jsonData["edition"]),
        gender(jsonData["gender"]),
        health(jsonData["health"].at(3)),
        maxCharges(jsonData["maxCharges"]),
        movementType(jsonData["movementType"]),
        offenseType(jsonData["offenseType"]),
        officialCardIds(jsonData["officialCardIds"].at(0)),
        orbsFire(jsonData["orbsFire"]),
        orbsFireFrost(jsonData["orbsFireFrost"]),
        orbsFireNature(jsonData["orbsFireNature"]),
        orbsFireShadow(jsonData["orbsFireShadow"]),
        orbsFrost(jsonData["orbsFrost"]),
        orbsNature(jsonData["orbsNature"]),
        orbsNatureFrost(jsonData["orbsNatureFrost"]),
        orbsNeutral(jsonData["orbsNeutral"]),
        orbsShadow(jsonData["orbsShadow"]),
        orbsShadowFrost(jsonData["orbsShadowFrost"]),
        orbsShadowNature(jsonData["orbsShadowNature"]),
        orbsTotal(jsonData["orbsTotal"]),
        powerCost(jsonData["powerCost"].at(3)),
        promo(jsonData["promo"]),
        rarity(jsonData["rarity"]),
        spellClass(jsonData["spellClass"]),
        squadSize(jsonData["squadSize"]),
        starterCard(jsonData["starterCard"]),
        type(jsonData["type"]),
        unitClass(jsonData["unitClass"]),
        unitModel(jsonData["unitModel"]),
        unitSpecies(jsonData["unitSpecies"])
        //upgradeMaps(jsonData["upgradeMaps"])
    {
        for (const auto& abilityData : jsonData["abilities"]) {
            Ability ability;
            ability.abilityAffinity = abilityData["abilityAffinity"];
            //ability.abilityAvailability = abilityData["abilityAvailability"];
            //ability.abilityCost = abilityData["abilityCost"];
            ability.abilityDescription = abilityData["abilityDescription"];
            ability.abilityDescriptionValues = abilityData["abilityDescriptionValues"];
            ability.abilityHidden = abilityData["abilityHidden"];
            ability.abilityIdentifier = abilityData["abilityIdentifier"];
            ability.abilityName = abilityData["abilityName"];
            //ability.abilityStars = abilityData["abilityStars"];
            ability.abilityType = abilityData["abilityType"];
            abilities.push_back(ability);
        }
    }
};


struct BattleTable
{
    unsigned int SizeCounter[4][5];
    //unsigned int Sige;
    unsigned int Flyer;
    void Init()
    {
        for (unsigned int iCounter = 0; iCounter < 5; iCounter++)
            for (unsigned int iSize = 0; iSize < 4; iSize++)
                SizeCounter[iSize][iCounter] = 0;
    };

    unsigned int SumSize(unsigned int iSize)
    {
        unsigned int iReturn = 0;
        for (unsigned int iCounter = 0; iCounter < 5; iCounter++)
            iReturn += SizeCounter[iSize][iCounter];
        return iReturn;
    }

    unsigned int SumCounter(unsigned int iCounter)
    {
        unsigned int iReturn = 0;
        for (unsigned int iSize = 0; iSize < 4; iSize++)
            iReturn += SizeCounter[iSize][iCounter];
        return iReturn;
    }
};