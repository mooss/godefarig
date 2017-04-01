#include "container_overload.h"

std::ostream& operator<<(std::ostream& os, const glm::mat4& matrice)
{
    os << matrice[0][0] << "  " << matrice[0][1] << "  " << matrice[0][2] << "  " << matrice[0][3] << "\n"
       << matrice[1][0] << "  " << matrice[1][1] << "  " << matrice[1][2] << "  " << matrice[1][3] << "\n"
       << matrice[2][0] << "  " << matrice[2][1] << "  " << matrice[2][2] << "  " << matrice[2][3] << "\n"
       << matrice[3][0] << "  " << matrice[3][1] << "  " << matrice[3][2] << "  " << matrice[3][3];
    return os;
}

std::ostream& operator<<(std::ostream& os, const glm::mat3& matrice)
{
    os << matrice[0][0] << "  " << matrice[0][1] << "  " << matrice[0][2] << "  " << "\n"
       << matrice[1][0] << "  " << matrice[1][1] << "  " << matrice[1][2] << "  " << "\n"
       << matrice[2][0] << "  " << matrice[2][1] << "  " << matrice[2][2] << "  ";
    return os;
}

std::ostream& operator<<(std::ostream& os, const glm::vec3& vecteur)
{
    os << "[" << vecteur[0] << "; " << vecteur[1] << "; " << vecteur[2] << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const glm::vec4& vecteur)
{
    os << "[" << vecteur[0] << "; " << vecteur[1] << "; " << vecteur[2] << "; " << vecteur[3] << "]";
    return os;
}
