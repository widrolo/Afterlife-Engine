#include "SectorWatchWidget.h"

#include <Engine/Core/World/Sector.h>

using namespace WEngine;

void SectorWatchWidget::Setup()
{
    m_widgetName = "SectorWatch";
}

void SectorWatchWidget::RenderInternal()
{
    SetSize({300, 400});
    ImGui::Text("All loaded sectors (except for root)");


}

void SectorWatchWidget::ForceHideSector(Sector& sector)
{

}

void SectorWatchWidget::ForceShowSector(Sector& sector)
{

}

void SectorWatchWidget::ForceUnloadSector(Sector& sector)
{

}
