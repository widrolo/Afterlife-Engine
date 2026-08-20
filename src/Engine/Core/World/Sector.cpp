#include "Sector.h"

#include <string>

using namespace WEngine;

Sector::Sector(const std::string& sectorName)
	: m_name(sectorName)
{
	LoadArgsFromFile(sectorName);
}

void Sector::Draw()
{

}

void Sector::LoadArgsFromFile(const std::string& sectorName)
{


}
