#ifdef TESTING
#include "Version_Test.h"
#include "Engine/Types/Version.h"

#include <stdexcept>

using namespace WTest;

void Version_TC::SetupCollection()
{
    m_name = "Version";

    AddTest<Version_Comparison_Test>();
    AddTest<Version_String_Test>();
}

void Version_Comparison_Test::Setup()
{
    m_name = "Comparison";
}

bool Version_Comparison_Test::Run()
{
    WEngine::Version zero = WEngine::Version(0, 0, 0, WEngine::VersionKind::Release);
    WEngine::Version zero2 = WEngine::Version(0, 0, 0, WEngine::VersionKind::Release);
    WEngine::Version majorDiff = WEngine::Version(1, 0, 0, WEngine::VersionKind::Release);
    WEngine::Version minorDiff = WEngine::Version(0, 1, 0, WEngine::VersionKind::Release);
    WEngine::Version patchDiff = WEngine::Version(0, 0, 1, WEngine::VersionKind::Release);
    WEngine::Version kindDiff = WEngine::Version(0, 0, 0, WEngine::VersionKind::Alpha);

    // comparison operators "==" and "!=" for equality
    if (!(zero == zero2))
        failTest(0);
    if (zero != zero2)
        failTest(1);

    // comparison operators "==" and "!=" for inequality of different obvious kinds
    if (zero == majorDiff)
        failTest(2);
    if (zero == minorDiff)
        failTest(3);
    if (zero == patchDiff)
        failTest(4);
    if (zero == kindDiff)
        failTest(5);

    // comparison operator "<". By definition: if < constitutes in failure, then so does > >= and <=
    if (majorDiff < minorDiff)
        failTest(6);
    if (minorDiff < patchDiff)
        failTest(7);

    return true;
}

void Version_String_Test::Setup()
{
    m_name = "String";
}

bool Version_String_Test::Run()
{
    WEngine::Version zero = WEngine::Version(0, 0, 0, WEngine::VersionKind::Release);
    WEngine::Version majorDiff = WEngine::Version(1, 0, 0, WEngine::VersionKind::Release);
    WEngine::Version minorDiff = WEngine::Version(0, 1, 0, WEngine::VersionKind::Release);
    WEngine::Version patchDiff = WEngine::Version(0, 0, 1, WEngine::VersionKind::Release);
    WEngine::Version kindDiff = WEngine::Version(0, 0, 0, WEngine::VersionKind::Alpha);

    if (zero.ToString() == majorDiff.ToString())
        failTest(0);
    if (zero.ToString() == minorDiff.ToString())
        failTest(1);
    if (zero.ToString() == patchDiff.ToString())
        failTest(2);
    if (zero.ToString() == kindDiff.ToString())
        failTest(3);

    if (minorDiff.ToString() == patchDiff.ToString())
        failTest(4);
    if (minorDiff.ToString() == kindDiff.ToString())
        failTest(5);

    if (patchDiff.ToString() == kindDiff.ToString())
        failTest(6);

    return true;
}

#endif



