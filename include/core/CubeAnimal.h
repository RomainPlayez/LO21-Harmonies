#pragma once
#include <string>

/**
 * @class CubeAnimal
 * @brief Représente un cube animal à poser sur le plateau.
 */
class CubeAnimal {
private:
    std::string _nomEspece;

    bool _estEsprit;

public:
    /**
     * @brief Constructeur du CubeAnimal.
     * @param nom Le nom de l'espèce animale.
     * @param estEsprit pour différencier les assets.
     */
    explicit CubeAnimal(const std::string& nom, bool estEsprit=false);

    /**
     * @brief Récupère le nom de l'espèce.
     * @return Une référence constante vers la chaîne de caractères.
     */
    const std::string& getNomEspece() const;

    bool isEsprit() const { return _estEsprit; }

    /**
     * @brief Modifie le type du cube en cours de partie.
     */
    void setEstEsprit(bool estEsprit) { _estEsprit = estEsprit; }
};