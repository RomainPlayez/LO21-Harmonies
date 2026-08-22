#include "core/CubeAnimal.h"

CubeAnimal::CubeAnimal(const std::string& nom, const bool estEsprit) : _nomEspece(nom), _estEsprit(estEsprit)  {
}

const std::string& CubeAnimal::getNomEspece() const {
    return _nomEspece;
}